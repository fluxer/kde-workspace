#include "kolorfactory.h"
#include "kolorclient.h"

#include <KDebug>

KWIN_DECORATION(KolorFactory)

KolorFactory::KolorFactory()
{
}

KolorFactory::~KolorFactory()
{
}

KDecoration* KolorFactory::createDecoration(KDecorationBridge* bridge)
{
    KolorClient* client = new KolorClient(bridge, this);
    return client->decoration();
}

bool KolorFactory::supports(KDecorationDefines::Ability ability) const
{
    // qDebug() << Q_FUNC_INFO << ability;
    switch (ability) {
        case KDecorationDefines::AbilityButtonMinimize:
        case KDecorationDefines::AbilityButtonMaximize:
        case KDecorationDefines::AbilityButtonClose: {
            return true;
        }
        default: {
            return false;
        }
    }
    Q_UNREACHABLE();
}
