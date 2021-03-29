/**
  * This file is part of the KDE project
  * Copyright (C) 2013 Valentin Rusu <kde@rusu.info>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of the GNU Library General Public
  * License version 2 as published by the Free Software Foundation.
  *
  * This library is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  * Library General Public License for more details.
  *
  * You should have received a copy of the GNU Library General Public License
  * along with this library; see the file COPYING.LIB.  If not, write to
  * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
  * Boston, MA 02110-1301, USA.
  */

#include <QIODevice>
#include <QFile>
#include <QCryptographicHash>

#include <ksavefile.h>
#include <kdebug.h>
#include <kmessagebox.h>
#include <klocalizedstring.h>

#include "backendpersisthandler.h"
#include "kwalletbackend.h"
#include "blowfish.h"
#include "cbc.h"

#include <gcrypt.h>
#include <assert.h>

#define KWALLET_CIPHER_BLOWFISH_ECB 0 // this was the old KWALLET_CIPHER_BLOWFISH_CBC
#define KWALLET_CIPHER_3DES_CBC     1 // unsupported
#define KWALLET_CIPHER_GPG          2 // unsupported
#define KWALLET_CIPHER_BLOWFISH_CBC 3

#define KWALLET_HASH_SHA1       0 // fallback
#define KWALLET_HASH_MD5        1 // unsupported
#define KWALLET_HASH_PBKDF2_SHA512 2 // used since 4.13 version

namespace KWallet {

typedef char Digest[16];

static BlowfishPersistHandler *blowfishHandler = 0;

BackendPersistHandler *BackendPersistHandler::getPersistHandler(BackendCipherType cipherType)
{
    switch (cipherType){
        case BACKEND_CIPHER_BLOWFISH: {
            if (blowfishHandler == 0) {
                blowfishHandler = new BlowfishPersistHandler;
            }
            return blowfishHandler;
        }
        default: {
            Q_ASSERT(0);
            return 0;
        }
    }
}

BackendPersistHandler *BackendPersistHandler::getPersistHandler(char magicBuf[KWMAGIC_LEN])
{
    if (magicBuf[2] == KWALLET_CIPHER_BLOWFISH_CBC &&
        (magicBuf[3] == KWALLET_HASH_SHA1 || magicBuf[3] == KWALLET_HASH_PBKDF2_SHA512)) {
        if (blowfishHandler == 0) {
            blowfishHandler = new BlowfishPersistHandler();
        }
        return blowfishHandler;
    }
    return 0;    // unknown cipher or hash
}

int BlowfishPersistHandler::write(Backend* wb, KSaveFile& sf, QByteArray& version, WId)
{
    assert(wb->_cipherType == BACKEND_CIPHER_BLOWFISH);

    version[2] = KWALLET_CIPHER_BLOWFISH_CBC;
    if(!wb->_useNewHash) {
        version[3] = KWALLET_HASH_SHA1;
    } else {
        version[3] = KWALLET_HASH_PBKDF2_SHA512; // Since 4.13 we always use PBKDF2_SHA512
    }

    if (sf.write(version, 4) != 4) {
        sf.abort();
        return -4; // write error
    }

    // Holds the hashes we write out
    QByteArray hashes;
    QDataStream hashStream(&hashes, QIODevice::WriteOnly);
    QCryptographicHash md5(QCryptographicHash::Md5);
    hashStream << static_cast<quint32>(wb->_entries.count());

    // Holds decrypted data prior to encryption
    QByteArray decrypted;

    // FIXME: we should estimate the amount of data we will write in each
    // buffer and resize them approximately in order to avoid extra
    // resizes.

    // populate decrypted
    QDataStream dStream(&decrypted, QIODevice::WriteOnly);
    for (Backend::FolderMap::ConstIterator i = wb->_entries.constBegin(); i != wb->_entries.constEnd(); ++i) {
        dStream << i.key();
        dStream << static_cast<quint32>(i.value().count());

        md5.reset();
        md5.addData(i.key().toUtf8());
        hashStream.writeRawData(md5.result().constData(), 16);
        hashStream << static_cast<quint32>(i.value().count());

        for (Backend::EntryMap::ConstIterator j = i.value().constBegin(); j != i.value().constEnd(); ++j) {
            dStream << j.key();
            dStream << static_cast<qint32>(j.value()->type());
            dStream << j.value()->value();

            md5.reset();
            md5.addData(j.key().toUtf8());
            hashStream.writeRawData(md5.result().constData(), 16);
        }
    }

    if (sf.write(hashes, hashes.size()) != hashes.size()) {
        sf.abort();
        return -4; // write error
    }

    // calculate the hash of the file
    QCryptographicHash sha(QCryptographicHash::Sha1);
    BlowFish _bf;
    CipherBlockChain bf(&_bf);

    sha.addData(decrypted);

    // prepend and append the random data
    QByteArray wholeFile;
    long blksz = bf.blockSize();
    long newsize = decrypted.size() +
               blksz            +    // encrypted block
               4                +    // file size
               20;      // size of the SHA hash

    int delta = (blksz - (newsize % blksz));
    newsize += delta;
    wholeFile.resize(newsize);

    const int randBlockSize = blksz+delta;
    char *randomData = (char*) gcry_random_bytes(randBlockSize, GCRY_STRONG_RANDOM);
    QByteArray randBlock(randomData, randBlockSize);
    ::free(randomData);

    for (int i = 0; i < blksz; i++) {
        wholeFile[i] = randBlock[i];
    }

    for (int i = 0; i < 4; i++) {
        wholeFile[(int)(i+blksz)] = (decrypted.size() >> 8*(3-i))&0xff;
    }

    for (int i = 0; i < decrypted.size(); i++) {
        wholeFile[(int)(i+blksz+4)] = decrypted[i];
    }

    for (int i = 0; i < delta; i++) {
        wholeFile[(int)(i+blksz+4+decrypted.size())] = randBlock[(int)(i+blksz)];
    }

    const QByteArray hash = sha.result();
    for (int i = 0; i < 20; i++) {
        wholeFile[(int)(newsize - 20 + i)] = hash[i];
    }

    sha.reset();
    decrypted.fill(0);

    // encrypt the data
    if (!bf.setKey(wb->_passhash.data(), wb->_passhash.size() * 8)) {
        wholeFile.fill(0);
        sf.abort();
        return -2; // encrypt error
    }

    int rc = bf.encrypt(wholeFile.data(), wholeFile.size());
    if (rc < 0) {
        wholeFile.fill(0);
        sf.abort();
        return -2;  // encrypt error
    }

    // write the file
    if (sf.write(wholeFile, wholeFile.size()) != wholeFile.size()) {
        wholeFile.fill(0);
        sf.abort();
        return -4; // write error
    }
    if (!sf.finalize()) {
        kDebug() << "WARNING: wallet sync to disk failed! KSaveFile status was " << sf.errorString();
        wholeFile.fill(0);
        return -4; // write error
    }

    wholeFile.fill(0);

    return 0;
}


int BlowfishPersistHandler::read(Backend* wb, QFile& db, WId)
{
    wb->_cipherType = BACKEND_CIPHER_BLOWFISH;
    wb->_hashes.clear();
    // Read in the hashes
    QDataStream hds(&db);
    quint32 n;
    hds >> n;
    if (n > 0xffff) { // sanity check
        return -43;
    }

    for (size_t i = 0; i < n; ++i) {
        Digest d, d2; // judgment day
        MD5Digest ba;
        QMap<MD5Digest,QList<MD5Digest> >::iterator it;
        quint32 fsz;
        if (hds.atEnd()) return -43;
        hds.readRawData(d, 16);
        hds >> fsz;
        ba = MD5Digest(reinterpret_cast<char *>(d));
        it = wb->_hashes.insert(ba, QList<MD5Digest>());
        for (size_t j = 0; j < fsz; ++j) {
            hds.readRawData(d2, 16);
            ba = MD5Digest(d2);
            (*it).append(ba);
        }
    }

    // Read in the rest of the file.
    QByteArray encrypted = db.readAll();
    assert(encrypted.size() < db.size());

    BlowFish _bf;
    CipherBlockChain bf(&_bf);
    int blksz = bf.blockSize();
    if ((encrypted.size() % blksz) != 0) {
        return -5;     // invalid file structure
    }

    bf.setKey((void *)wb->_passhash.data(), wb->_passhash.size()*8);

    if (!encrypted.data()) {
        wb->_passhash.fill(0);
        encrypted.fill(0);
        return -7; // file structure error
    }

    int rc = bf.decrypt(encrypted.data(), encrypted.size());
    if (rc < 0) {
        wb->_passhash.fill(0);
        encrypted.fill(0);
        return -6;  // decrypt error
    }

    const char *t = encrypted.data();

    // strip the leading data
    t += blksz;    // one block of random data

    // strip the file size off
    long fsize = 0;

    fsize |= (long(*t) << 24) & 0xff000000;
    t++;
    fsize |= (long(*t) << 16) & 0x00ff0000;
    t++;
    fsize |= (long(*t) <<  8) & 0x0000ff00;
    t++;
    fsize |= long(*t) & 0x000000ff;
    t++;

    if (fsize < 0 || fsize > long(encrypted.size()) - blksz - 4) {
        //kDebug() << "fsize: " << fsize << " encrypted.size(): " << encrypted.size() << " blksz: " << blksz;
        encrypted.fill(0);
        return -9;         // file structure error.
    }

    // compute the hash ourself
    QCryptographicHash sha(QCryptographicHash::Sha1);
    sha.addData(t, fsize);
    const QByteArray testhash = sha.result();

    // compare hashes
    int sz = encrypted.size();
    for (int i = 0; i < 20; i++) {
        if (testhash[i] != encrypted[sz - 20 + i]) {
            encrypted.fill(0);
            sha.reset();
            return -8;         // hash error.
        }
    }

    sha.reset();

    // chop off the leading blksz+4 bytes
    QByteArray tmpenc(encrypted.data()+blksz+4, fsize);
    encrypted = tmpenc;
    tmpenc.fill(0);

    // Load the data structures up
    QDataStream eStream(encrypted);

    while (!eStream.atEnd()) {
        QString folder;
        quint32 n;

        eStream >> folder;
        eStream >> n;

        // Force initialisation
        wb->_entries[folder].clear();

        for (size_t i = 0; i < n; ++i) {
            QString key;
            KWallet::Wallet::EntryType et = KWallet::Wallet::Unknown;
            Entry *e = new Entry;
            eStream >> key;
            qint32 x = 0; // necessary to read properly
            eStream >> x;
            et = static_cast<KWallet::Wallet::EntryType>(x);

            switch (et) {
                case KWallet::Wallet::Password:
                case KWallet::Wallet::Stream:
                case KWallet::Wallet::Map:
                    break;
                default: // Unknown entry
                    delete e;
                    continue;
            }

            QByteArray a;
            eStream >> a;
            e->setValue(a);
            e->setType(et);
            e->setKey(key);
            wb->_entries[folder][key] = e;
        }
    }

    wb->_open = true;
    encrypted.fill(0);
    return 0;
}

} // namespace
