#include "kolorbutton.h"

#include <KIcon>
#include <KDebug>

KolorButton::KolorButton(ButtonType type, KCommonDecoration *parent)
    : KCommonDecorationButton(type, parent),
    m_iconmode(QIcon::Normal),
    m_iconstate(QIcon::Off)
{
    setAutoFillBackground(false);
    setAttribute(Qt::WA_NoSystemBackground);
}

KolorButton::~KolorButton()
{
}

void KolorButton::reset(unsigned long changed)
{
    Q_UNUSED(changed);
    // qDebug() << Q_FUNC_INFO << changed << type() << isVisible();
    switch (type()) {
        case ButtonType::MinButton: {
            m_pixmap = KIcon("view-restore").pixmap(size(), m_iconmode, m_iconstate);
            break;
        }
        case ButtonType::MaxButton: {
            m_pixmap = KIcon("view-fullscreen").pixmap(size(), m_iconmode, m_iconstate);
            break;
        }
        case ButtonType::CloseButton: {
            m_pixmap = KIcon("dialog-close").pixmap(size(), m_iconmode, m_iconstate);
            break;
        }
        default: {
            break;
        }
    }
}

void KolorButton::mousePressEvent(QMouseEvent *event)
{
    // qDebug() << Q_FUNC_INFO;
    KCommonDecorationButton::mousePressEvent(event);
    m_iconstate = QIcon::On;
    update();
    decoration()->updateCaption();
}

void KolorButton::mouseReleaseEvent(QMouseEvent *event)
{
    // qDebug() << Q_FUNC_INFO;
    KCommonDecorationButton::mouseReleaseEvent(event);
    m_iconstate = QIcon::Off;
    update();
    decoration()->updateCaption();
}

void KolorButton::enterEvent(QEvent *event)
{
    // qDebug() << Q_FUNC_INFO;
    KCommonDecorationButton::enterEvent(event);
    m_iconmode = QIcon::Selected;
    update();
    decoration()->updateCaption();
}

void KolorButton::leaveEvent(QEvent *event)
{
    // qDebug() << Q_FUNC_INFO;
    KCommonDecorationButton::leaveEvent(event);
    m_iconmode = QIcon::Normal;
    update();
    decoration()->updateCaption();
}

QPixmap KolorButton::buttonPixmap() const
{
    return m_pixmap;
}
