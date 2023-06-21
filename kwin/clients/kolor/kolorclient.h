#ifndef KOLORCLIENT_H
#define KOLORCLIENT_H

#include <kcommondecoration.h>

class KolorClient : public KCommonDecoration
{
    Q_OBJECT
public:
    KolorClient(KDecorationBridge* bridge, KDecorationFactory* factory);
    virtual ~KolorClient();

    QString visibleName() const final;
    KCommonDecorationButton *createButton(ButtonType type) final;
    void paintEvent(QPaintEvent *event) final;

    void init() final;
};

#endif // KOLORCLIENT_H
