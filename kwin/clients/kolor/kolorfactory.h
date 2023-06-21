#ifndef KOLORFACTORY_H
#define KOLORFACTORY_H

#include <kdecorationfactory.h>
#include <kdeversion.h>

class KolorFactory: public KDecorationFactory
{
public:
    KolorFactory();
    virtual ~KolorFactory();

    KDecoration* createDecoration(KDecorationBridge* bridge) final;
    bool supports(KDecorationDefines::Ability ability) const final;
};

#endif // KOLORFACTORY_H
