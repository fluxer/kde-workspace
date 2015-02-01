#ifndef __info_h__
#define __info_h__


#include <kio/slavebase.h>

class KIconLoader;

class ManProtocol : public KIO::SlaveBase
{
public:

    ManProtocol( const QByteArray &pool, const QByteArray &app );
    virtual ~ManProtocol();

    virtual void get( const KUrl& url );
    virtual void stat( const KUrl& url );
    virtual void mimetype( const KUrl& url );

protected:

    void decodeURL( const KUrl &url );
    void decodePath( QString path );

private:

    QString   m_bash;
    QString   m_manScript;
    QString   m_page;
};

#endif // __info_h__
