#include <QtCore/QTextStream>
#include <QtCore/QTimer>
#include <QtCore/QMap>
#include <QApplication>

#include <kaboutdata.h>
#include <kcomponentdata.h>
#include <kcmdlineargs.h>
#include <kdebug.h>
#include <kglobal.h>
#include <kstandarddirs.h>
#include <kwallet.h>
#include <QtDBus/QDBusConnectionInterface>
#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusReply>
#include <klocale.h>

#include "kwallettest.h"

static QTextStream _out( stdout, QIODevice::WriteOnly );

void openWallet()
{
    _out << "About to ask for wallet async" << endl;

    // we have no wallet: ask for one.
    KWallet::Wallet *wallet = KWallet::Wallet::openWallet( KWallet::Wallet::NetworkWallet(), 0, KWallet::Wallet::Asynchronous );

    WalletReceiver r;
    r.connect( wallet, SIGNAL( walletOpened(bool) ), SLOT( walletOpened(bool) ) );

    _out << "About to ask for wallet sync" << endl;

    wallet = KWallet::Wallet::openWallet( KWallet::Wallet::NetworkWallet(), 0, KWallet::Wallet::Synchronous );

    _out << "Got sync wallet: " << (wallet != 0) << endl;
    _out << "About to start 30 second event loop" << endl;

    QTimer::singleShot( 30000, qApp, SLOT( quit() ) );
    int ret = qApp->exec();

    if ( ret == 0 ) {
        _out << "Timed out!" << endl;
    } else {
        _out << "Success!" << endl;
    }

    QMap<QString,QString> p;
    ret = wallet->readPasswordList("*", p);
    _out << "readPasswordList returned: " << ret << endl;
    _out << "readPasswordList returned " << p.keys().count() << " entries" << endl;
    QMap<QString, QMap<QString, QString> > q;
    ret = wallet->readMapList("*", q);
    _out << "readMapList returned: " << ret << endl;
    _out << "readMapList returned " << q.keys().count() << " entries" << endl;

    QMap<QString, QByteArray> s;
    ret = wallet->readEntryList("*", s);
    _out << "readEntryList returned: " << ret << endl;
    _out << "readEntryList returned " << s.keys().count() << " entries" << endl;

    delete wallet;
}

void WalletReceiver::walletOpened( bool got )
{
    _out << "Got async wallet: " << got << endl;
    qApp->exit( 1 );
}

int main( int argc, char *argv[] )
{
    KAboutData aboutData("kwalletboth", 0, ki18n("kwalletboth"), "version");
    KComponentData componentData(&aboutData);
    QApplication app( argc, argv );

    // force name with D-BUS
    QDBusReply<QDBusConnectionInterface::RegisterServiceReply> reply
        = QDBusConnection::sessionBus().interface()->registerService( "org.kde.kwalletboth",
                                                            QDBusConnectionInterface::ReplaceExistingService );

    if ( !reply.isValid() ) {
        _out << "D-BUS name request returned " << reply.error().name() << endl;
    }

    openWallet();

    return 0;
}

// vim: set noet ts=4 sts=4 sw=4:

