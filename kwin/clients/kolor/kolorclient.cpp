#include "kolorclient.h"
#include "kolorbutton.h"

#include <QPainter>
#include <KIcon>
#include <KDebug>

KolorClient::KolorClient(KDecorationBridge* bridge, KDecorationFactory* factory)
    : KCommonDecoration(bridge, factory)
{
}

KolorClient::~KolorClient()
{
}

QString KolorClient::visibleName() const
{
    return QString::fromLatin1("Kolor");
}

KCommonDecorationButton* KolorClient::createButton(ButtonType type)
{
    return new KolorButton(type, this);
}

void KolorClient::paintEvent(QPaintEvent *event)
{
    QWidget* decorationwidget = KCommonDecoration::widget();
    const QIcon decorationicon = KCommonDecoration::icon();
    const QString decorationcaption = KCommonDecoration::caption();

    QPainter painter(decorationwidget);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setClipRegion(event->region());
    const QPalette widgetpalette = decorationwidget->palette();
    painter.fillRect(decorationwidget->rect(), widgetpalette.window().color());

    QList<QSize> decorationiconsize = decorationicon.availableSizes();
    if (!decorationiconsize.isEmpty()) {
        const QRect iconrect = KCommonDecoration::iconGeometry();
        painter.drawPixmap(iconrect.topLeft(), decorationicon.pixmap(decorationiconsize.first()));
    }

    if (!decorationcaption.isEmpty()) {
        painter.drawText(titleRect(), decorationcaption);
    }

    QList<KolorButton*> buttons = decorationwidget->findChildren<KolorButton*>();
    // qDebug() << Q_FUNC_INFO << buttons;
    foreach (KolorButton* button, buttons) {
        QPixmap buttonpixmap = button->buttonPixmap();
        if (!buttonpixmap.isNull()) {
            painter.drawPixmap(button->pos(), buttonpixmap);
        }
    }
}

void KolorClient::init()
{
    KCommonDecoration::init();

    widget()->setAttribute(Qt::WA_NoSystemBackground);
    widget()->setAutoFillBackground(false);
}
