#ifndef KOLORBUTTON_H
#define KOLORBUTTON_H

#include <QIcon>

#include <kcommondecoration.h>

class KolorButton : public KCommonDecorationButton
{
    Q_OBJECT
public:
    explicit KolorButton(ButtonType type, KCommonDecoration *parent);
    ~KolorButton();

    void reset(unsigned long changed) final;

    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void enterEvent(QEvent *event) final;
    void leaveEvent(QEvent *event) final;

    QPixmap buttonPixmap() const;

private:
    QPixmap m_pixmap;
    QIcon::Mode m_iconmode;
    QIcon::State m_iconstate;
};

#endif // KOLORBUTTON_H
