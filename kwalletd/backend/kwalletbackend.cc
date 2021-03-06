/* This file is part of the KDE project
 *
 * Copyright (C) 2001-2004 George Staikos <staikos@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
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

#include "kwalletbackend.h"

#include <stdlib.h>

#include <kdebug.h>
#include <kglobal.h>
#include <klocale.h>
#include <ksavefile.h>
#include <kstandarddirs.h>
#include <knotification.h>

#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QRegExp>
#include <QCryptographicHash>

#include "blowfish.h"
#include "cbc.h"

#include <assert.h>

#define KWALLET_VERSION_MAJOR 0
// has been 0 until 4.13, 1 until 4.20
#define KWALLET_VERSION_MINOR 2

using namespace KWallet;

#define KWMAGIC "KWALLET\n\r\0\r\n"

class Backend::BackendPrivate
{
};

static void initKWalletDir()
{
    KGlobal::dirs()->addResourceType("kwallet", 0, "share/apps/kwallet");
}

Backend::Backend(const QString& name, bool isPath)
    : d(0)
    , _name(name)
    , _ref(0)
    , _cipherType(KWallet::BACKEND_CIPHER_UNKNOWN)
{
    initKWalletDir();
    if (isPath) {
        _path = name;
    } else {
        _path = KGlobal::dirs()->saveLocation("kwallet") + _name + ".kwl";
    }

    _open = false;
}


Backend::~Backend() {
    if (_open) {
        close();
    }
    delete d;
}

void Backend::setCipherType(BackendCipherType ct)
{
    // changing cipher type on already initialed wallets is not permitted
    assert(_cipherType == KWallet::BACKEND_CIPHER_UNKNOWN);
    _cipherType = ct;
}

int Backend::deref() {
    if (--_ref < 0) {
        kDebug() << "refCount negative!";
        _ref = 0;
    }
    return _ref;
}

bool Backend::exists(const QString& wallet) {
    initKWalletDir();
    QString path = KGlobal::dirs()->saveLocation("kwallet") + '/' + wallet + ".kwl";
    // Note: 60 bytes is presently the minimum size of a wallet file.
    //       Anything smaller is junk.
    return QFile::exists(path) && QFileInfo(path).size() >= 60;
}


QString Backend::openRCToString(int rc) {
    switch (rc) {
        case -255:
            return i18n("Already open.");
        case -2:
            return i18n("Error opening file.");
        case -3:
            return i18n("Not a wallet file.");
        case -4:
            return i18n("Unsupported file format revision.");
        case -42:
            return i18n("Unknown encryption scheme.");
        case -43:
            return i18n("Corrupt file?");
        case -8:
            return i18n("Error validating wallet integrity. Possibly corrupted.");
        case -5:
        case -7:
        case -9:
            return i18n("Read error - possibly incorrect password.");
        case -6:
            return i18n("Decryption error.");
        default:
            return QString();
    }
}


int Backend::open(const QByteArray& password, WId w) {
    if (_open) {
        return -255;  // already open
    }
    
    setPassword(password);
    return openInternal(w);
}

int Backend::openPreHashed(const QByteArray &passwordHash)
{
    if (_open) {
        return -255;  // already open
    }
   
    // check the password hash for correct size (currently fixed)
    if (passwordHash.size() != KWALLET_SHA512_KEYSIZE) {
        return -42; // unsupported encryption scheme
    }
   
    _passhash = passwordHash;

    return openInternal();
}
 
int Backend::openInternal(WId w)
{
    // No wallet existed.  Let's create it.
    // Note: 60 bytes is presently the minimum size of a wallet file.
    //       Anything smaller is junk and should be deleted.
    if (!QFile::exists(_path) || QFileInfo(_path).size() < 60) {
        QFile newfile(_path);
        if (!newfile.open(QIODevice::ReadWrite)) {
            return -2;   // error opening file
        }
        newfile.close();
        _open = true;
        if (sync(w) != 0) {
            return -2;
        }
    }

    QFile db(_path);

    if (!db.open(QIODevice::ReadOnly)) {
        return -2;         // error opening file
    }

    char magicBuf[KWMAGIC_LEN];
    db.read(magicBuf, KWMAGIC_LEN);
    if (memcmp(magicBuf, KWMAGIC, KWMAGIC_LEN) != 0) {
        return -3; // bad magic
    }

    db.read(magicBuf, 4);

    // First byte is major version, second byte is minor version
    if (magicBuf[0] != KWALLET_VERSION_MAJOR) {
        return -4; // unknown version
    }

    if (magicBuf[1] == KWALLET_VERSION_MINOR) {
        kDebug() << "Wallet new enough, using new hash";
    } else if (magicBuf[1] != 0){
        kDebug() << "Wallet is old, sad panda :(";
        return -4; // unknown version
    }

    BackendPersistHandler *phandler = BackendPersistHandler::getPersistHandler(magicBuf);
    if (0 == phandler){
        return 42; // unknown cipher or hash
    }
    return phandler->read(this, db, w);
}

int Backend::sync(WId w) {
    if (!_open) {
        return -255;  // not open yet
    }

    KSaveFile sf(_path);

    if (!sf.open(QIODevice::WriteOnly | QIODevice::Unbuffered)) {
        return -1; // error opening file
    }
    sf.setPermissions(QFile::ReadUser|QFile::WriteUser);

    if (sf.write(KWMAGIC, KWMAGIC_LEN) != KWMAGIC_LEN) {
        sf.abort();
        return -4; // write error
    }

    // Write the version number
    QByteArray version(4, 0);
    version[0] = KWALLET_VERSION_MAJOR;
    version[1] = KWALLET_VERSION_MINOR;

    BackendPersistHandler *phandler = BackendPersistHandler::getPersistHandler(_cipherType);
    if (0 == phandler) {
        return -4; // write error
    }
    int rc = phandler->write(this, sf, version, w);
    if (rc<0) {
        // Oops! wallet file sync filed! Display a notification about that
        // TODO: change kwalletd status flags, when status flags will be implemented
        KNotification *notification = new KNotification( "syncFailed" );
        notification->setText( i18n("Failed to sync wallet <b>%1</b> to disk. Error codes are:\nRC <b>%2</b>\nSF <b>%3</b>. Please file a BUG report using this information to bugs.kde.org").arg(_name).arg(rc).arg(sf.errorString()) );
        notification->sendEvent();
    }
    return rc;
}


int Backend::close(bool save) {
    // save if requested
    if (save) {
        int rc = sync(0);
        if (rc != 0) {
            return rc;
        }
    }

    // do the actual close
    for (FolderMap::ConstIterator i = _entries.constBegin(); i != _entries.constEnd(); ++i) {
        for (EntryMap::ConstIterator j = i.value().constBegin(); j != i.value().constEnd(); ++j) {
            delete j.value();
        }
    }
    _entries.clear();

    // empty the password hash
    _passhash.fill(0);

    _open = false;

    return 0;
}

const QString& Backend::walletName() const {
    return _name;
}


bool Backend::isOpen() const {
    return _open;
}


QStringList Backend::folderList() const {
    return _entries.keys();
}


QStringList Backend::entryList() const {
    return _entries[_folder].keys();
}


Entry *Backend::readEntry(const QString& key) {
    Entry *rc = 0L;

    if (_open && hasEntry(key)) {
        rc = _entries[_folder][key];
    }

    return rc;
}


QList<Entry*> Backend::readEntryList(const QString& key) {
    QList<Entry*> rc;

    if (!_open) {
        return rc;
    }

    QRegExp re(key, Qt::CaseSensitive, QRegExp::Wildcard);

    const EntryMap& map = _entries[_folder];
    for (EntryMap::ConstIterator i = map.begin(); i != map.end(); ++i) {
        if (re.exactMatch(i.key())) {
            rc.append(i.value());
        }
    }
    return rc;
}


bool Backend::createFolder(const QString& f) {
    if (_entries.contains(f)) {
        return false;
    }

    _entries.insert(f, EntryMap());

    QCryptographicHash folderMd5(QCryptographicHash::Md5);
    folderMd5.addData(f.toUtf8());
    _hashes.insert(MD5Digest(folderMd5.result()), QList<MD5Digest>());

    return true;
}


int Backend::renameEntry(const QString& oldName, const QString& newName) {
    EntryMap& emap = _entries[_folder];
    EntryMap::Iterator oi = emap.find(oldName);
    EntryMap::Iterator ni = emap.find(newName);

    if (oi != emap.end() && ni == emap.end()) {
        Entry *e = oi.value();
        emap.erase(oi);
        emap[newName] = e;

        QCryptographicHash folderMd5(QCryptographicHash::Md5);
        folderMd5.addData(_folder.toUtf8());

        HashMap::iterator i = _hashes.find(MD5Digest(folderMd5.result()));
        if (i != _hashes.end()) {
            QCryptographicHash oldMd5(QCryptographicHash::Md5), newMd5(QCryptographicHash::Md5);
            oldMd5.addData(oldName.toUtf8());
            newMd5.addData(newName.toUtf8());
            i.value().removeAll(MD5Digest(oldMd5.result()));
            i.value().append(MD5Digest(newMd5.result()));
        }
        return 0;
    }

    return -1;
}


void Backend::writeEntry(Entry *e) {
    if (!_open) {
        return;
    }

    if (!hasEntry(e->key())) {
        _entries[_folder][e->key()] = new Entry;
    }
    _entries[_folder][e->key()]->copy(e);

    QCryptographicHash folderMd5(QCryptographicHash::Md5);
    folderMd5.addData(_folder.toUtf8());

    HashMap::iterator i = _hashes.find(MD5Digest(folderMd5.result()));
    if (i != _hashes.end()) {
        QCryptographicHash md5(QCryptographicHash::Md5);
        md5.addData(e->key().toUtf8());
        i.value().append(MD5Digest(md5.result()));
    }
}


bool Backend::hasEntry(const QString& key) const {
    return _entries.contains(_folder) && _entries[_folder].contains(key);
}


bool Backend::removeEntry(const QString& key) {
    if (!_open) {
        return false;
    }

    FolderMap::Iterator fi = _entries.find(_folder);
    EntryMap::Iterator ei = fi.value().find(key);

    if (fi != _entries.end() && ei != fi.value().end()) {
        delete ei.value();
        fi.value().erase(ei);
        QCryptographicHash folderMd5(QCryptographicHash::Md5);
        folderMd5.addData(_folder.toUtf8());

        HashMap::iterator i = _hashes.find(MD5Digest(folderMd5.result()));
        if (i != _hashes.end()) {
            QCryptographicHash md5(QCryptographicHash::Md5);
            md5.addData(key.toUtf8());
            i.value().removeAll(MD5Digest(md5.result()));
        }
        return true;
    }

    return false;
}


bool Backend::removeFolder(const QString& f) {
    if (!_open) {
        return false;
    }

    FolderMap::Iterator fi = _entries.find(f);

    if (fi != _entries.end()) {
        if (_folder == f) {
            _folder.clear();
        }

        for (EntryMap::Iterator ei = fi.value().begin(); ei != fi.value().end(); ++ei) {
            delete ei.value();
        }

        _entries.erase(fi);

        QCryptographicHash folderMd5(QCryptographicHash::Md5);
        folderMd5.addData(f.toUtf8());
        _hashes.remove(MD5Digest(folderMd5.result()));
        return true;
    }

    return false;
}


bool Backend::folderDoesNotExist(const QString& folder) const {
    QCryptographicHash md5(QCryptographicHash::Md5);
    md5.addData(folder.toUtf8());
    return !_hashes.contains(MD5Digest(md5.result()));
}


bool Backend::entryDoesNotExist(const QString& folder, const QString& entry) const {
    QCryptographicHash md5(QCryptographicHash::Md5);
    md5.addData(folder.toUtf8());
    HashMap::const_iterator i = _hashes.find(MD5Digest(md5.result()));
    if (i != _hashes.end()) {
        md5.reset();
        md5.addData(entry.toUtf8());
        return !i.value().contains(MD5Digest(md5.result()));
    }
    return true;
}

void Backend::setPassword(const QByteArray &password) {
    _passhash = QCryptographicHash::hash(password, QCryptographicHash::Sha512);
}
