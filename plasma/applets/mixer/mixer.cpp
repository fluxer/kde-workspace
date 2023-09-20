/*  This file is part of the KDE project
    Copyright (C) 2023 Ivailo Monev <xakepa10@gmail.com>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License version 2, as published by the Free Software Foundation.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#include "mixer.h"

#include <QTimer>
#include <QApplication>
#include <QDesktopWidget>
#include <QGraphicsGridLayout>
#include <Plasma/TabBar>
#include <Plasma/Frame>
#include <Plasma/Label>
#include <Plasma/Slider>
#include <Plasma/IconWidget>
#include <KIconLoader>
#include <KIcon>
#include <KDebug>

#include <alsa/asoundlib.h>

static const QSizeF s_minimumsize = QSizeF(290, 140);
static const QSizeF s_minimumslidersize = QSizeF(10, 70);
static const int s_svgiconsize = 256;
static const QString s_defaultpopupicon = QString::fromLatin1("audio-card");

static QList<snd_mixer_selem_channel_id_t> kALSAChannelTypes(snd_mixer_elem_t *alsaelement, const bool capture)
{
    QList<snd_mixer_selem_channel_id_t> result;
    static const snd_mixer_selem_channel_id_t alsachanneltypes[] = {
        SND_MIXER_SCHN_FRONT_LEFT,
        SND_MIXER_SCHN_FRONT_RIGHT,
        SND_MIXER_SCHN_REAR_LEFT,
        SND_MIXER_SCHN_REAR_RIGHT,
        SND_MIXER_SCHN_FRONT_CENTER,
        SND_MIXER_SCHN_WOOFER,
        SND_MIXER_SCHN_SIDE_LEFT,
        SND_MIXER_SCHN_SIDE_RIGHT,
        SND_MIXER_SCHN_REAR_CENTER,
        SND_MIXER_SCHN_UNKNOWN
    };

    int counter = 0;
    while (alsachanneltypes[counter] != SND_MIXER_SCHN_UNKNOWN) {
        int alsaresult = 0;
        if (!capture) {
            alsaresult = snd_mixer_selem_has_playback_channel(alsaelement, alsachanneltypes[counter]);
        } else {
            alsaresult = snd_mixer_selem_has_capture_channel(alsaelement, alsachanneltypes[counter]);
        }
        if (alsaresult != 0) {
            result.append(alsachanneltypes[counter]);
        }
        counter++;
    }
    return result;
}

static bool kGetChannelVolumes(snd_mixer_elem_t *alsaelement, snd_mixer_selem_channel_id_t alsaelementchannel, const bool alsahascapture,
                               long *alsavolumemin, long *alsavolumemax, long *alsavolume)
{
    int alsaresult = 0;
    if (alsahascapture) {
        alsaresult = snd_mixer_selem_get_capture_volume_range(alsaelement, alsavolumemin, alsavolumemax);
        if (alsaresult != 0) {
            kWarning() << "Could not get capture channel volume range" << snd_strerror(alsaresult);
            return false;
        }
        alsaresult = snd_mixer_selem_get_capture_volume(alsaelement, alsaelementchannel, alsavolume);
        if (alsaresult != 0) {
            kWarning() << "Could not get capture channel volume" << snd_strerror(alsaresult);
            return false;
        }
        return true;
    }
    alsaresult = snd_mixer_selem_get_playback_volume_range(alsaelement, alsavolumemin, alsavolumemax);
    if (alsaresult != 0) {
        kWarning() << "Could not get playback channel volume range" << snd_strerror(alsaresult);
        return false;
    }
    alsaresult = snd_mixer_selem_get_playback_volume(alsaelement, alsaelementchannel, alsavolume);
    if (alsaresult != 0) {
        kWarning() << "Could not get playback channel volume" << snd_strerror(alsaresult);
        return false;
    }
    return true;
}

static bool kIsMasterChannel(const QString &alsaelementname)
{
    return alsaelementname.contains(QLatin1String("master"), Qt::CaseInsensitive);
}

static QString kIconForChannel(const QString &alsaelementname)
{
    // TODO: more icons
    if (kIsMasterChannel(alsaelementname)) {
        return QString::fromLatin1("mixer-master");
    }
    if (alsaelementname.contains(QLatin1String("capture"), Qt::CaseInsensitive)) {
        return QString::fromLatin1("mixer-capture");
    }
    if (alsaelementname.contains(QLatin1String("pcm"), Qt::CaseInsensitive)) {
        return QString::fromLatin1("mixer-pcm");
    }
    if (alsaelementname.contains(QLatin1String("ac97"), Qt::CaseInsensitive)) {
        return QString::fromLatin1("mixer-ac97");
    }
    return QString::fromLatin1("mixer-line");
}

static int kFixedVolume(const int value, const int alsavolumemax)
{
    const qreal valuefactor = (qreal(alsavolumemax) / 100);
    return qRound(qreal(value) / valuefactor);
}

static int kVolumeStep()
{
    const int appscrolllines = QApplication::wheelScrollLines();
    return qMax(appscrolllines, 1);
}

static int kStepVolume(const int value, const int maxvalue, const int step)
{
    const qreal valuefactor = (qreal(maxvalue) / 100);
    return (value + (valuefactor * step));
}

static QIcon kMixerIcon(QObject *parent, const int value)
{
    QIcon result;
    Plasma::Svg plasmasvg(parent);
    plasmasvg.setImagePath("icons/audio");
    plasmasvg.setContainsMultipleImages(true);
    if (plasmasvg.isValid()) {
        QPixmap iconpixmap(s_svgiconsize, s_svgiconsize);
        iconpixmap.fill(Qt::transparent);
        QPainter iconpainter(&iconpixmap);
        if (value >= 75) {
            plasmasvg.paint(&iconpainter, iconpixmap.rect(), "audio-volume-high");
        } else if (value >= 50) {
            plasmasvg.paint(&iconpainter, iconpixmap.rect(), "audio-volume-medium");
        } else if (value >= 25) {
            plasmasvg.paint(&iconpainter, iconpixmap.rect(), "audio-volume-low");
        } else {
            plasmasvg.paint(&iconpainter, iconpixmap.rect(), "audio-volume-muted");
        }
        result = QIcon(iconpixmap);
    } else {
        result = KIcon(s_defaultpopupicon);
    }
    return result;
}

class MixerTabWidget : public QGraphicsWidget
{
    Q_OBJECT
public:
    MixerTabWidget(Plasma::TabBar *tabbar);
    ~MixerTabWidget();

    bool setup(const int cardnumber, const QByteArray &cardname);
    QIcon mainVolumeIcon();
    void decreaseVolume();
    void increaseVolume();

Q_SIGNALS:
    void mainVolumeChanged();

private Q_SLOTS:
    void slotSliderMovedOrChanged(const int value);

private:
    QGraphicsLinearLayout* m_layout;
    snd_mixer_t* m_alsamixer;
    QString m_mainelement;
    QList<Plasma::Slider*> m_sliders;
};

MixerTabWidget::MixerTabWidget(Plasma::TabBar *tabbar)
    : QGraphicsWidget(tabbar),
    m_layout(nullptr),
    m_alsamixer(nullptr)
{
    setMinimumSize(s_minimumsize);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    m_layout = new QGraphicsLinearLayout(Qt::Horizontal, this);
    setLayout(m_layout);
}

MixerTabWidget::~MixerTabWidget()
{
    if (m_alsamixer) {
        snd_mixer_close(m_alsamixer);
    }
}

bool MixerTabWidget::setup(const int alsacard, const QByteArray &alsacardname)
{
    Q_ASSERT(m_alsamixer == nullptr);
    int alsaresult = snd_mixer_open(&m_alsamixer, 0);
    if (alsaresult != 0) {
        kWarning() << "Could not open mixer" << snd_strerror(alsaresult);
        return false;
    }
    alsaresult = snd_mixer_attach(m_alsamixer, alsacardname.constData());
    if (alsaresult != 0) {
        kWarning() << "Could not attach mixer" << snd_strerror(alsaresult);
        snd_mixer_close(m_alsamixer);
        m_alsamixer = nullptr;
        return false;
    }
    alsaresult = snd_mixer_selem_register(m_alsamixer, nullptr, nullptr);
    if (alsaresult != 0) {
        kWarning() << "Could not register mixer" << snd_strerror(alsaresult);
        snd_mixer_close(m_alsamixer);
        m_alsamixer = nullptr;
        return false;
    }
    alsaresult = snd_mixer_load(m_alsamixer);
    if (alsaresult != 0) {
        kWarning() << "Could not load mixer" << snd_strerror(alsaresult);
        snd_mixer_close(m_alsamixer);
        m_alsamixer = nullptr;
        return false;
    }

    const int smalliconsize = KIconLoader::global()->currentSize(KIconLoader::Small);
    const QSizeF smalliconsizef = QSizeF(smalliconsize, smalliconsize);
    bool hasvalidelement = false;
    QStringList alsaelementnames;
    snd_mixer_elem_t *alsaelement = snd_mixer_first_elem(m_alsamixer);
    for (; alsaelement; alsaelement = snd_mixer_elem_next(alsaelement)) {
        if (snd_mixer_elem_empty(alsaelement)) {
            continue;
        }
        const bool alsahasplayback = snd_mixer_selem_has_playback_volume(alsaelement);
        const bool alsahascapture = snd_mixer_selem_has_capture_volume(alsaelement);
        const uint alsaelementindex = snd_mixer_selem_get_index(alsaelement);
        const QString alsaelementname = QString::fromLocal8Bit(snd_mixer_selem_get_name(alsaelement));
        if (!alsahasplayback && !alsahascapture) {
            // no volume to mix
            kDebug() << "Skipping" << alsaelementindex << alsaelementname << "due to lack of volume";
            continue;
        }
        // qDebug() << Q_FUNC_INFO << alsaelementindex << alsaelementname << alsahasplayback << alsahascapture;

        const QList<snd_mixer_selem_channel_id_t> alsaelementchannels = kALSAChannelTypes(alsaelement, alsahascapture);
        if (alsaelementchannels.size() < 1) {
            kWarning() << "Element has no channels" << alsaelementindex << alsaelementname;
            continue;
        }

        hasvalidelement = true;
        Plasma::Frame* frame = new Plasma::Frame(this);
        frame->setFrameShadow(Plasma::Frame::Sunken);
        frame->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
        // TODO: maybe elide frame text
        frame->setText(alsaelementname);
        QGraphicsGridLayout* framelayout = new QGraphicsGridLayout(frame);
        int columncounter = 0;
        foreach (const snd_mixer_selem_channel_id_t alsaelementchannel, alsaelementchannels) {
            long alsavolumemin = 0;
            long alsavolumemax = 0;
            long alsavolume = 0;
            const bool gotvolumes = kGetChannelVolumes(
                alsaelement, alsaelementchannel, alsahascapture,
                &alsavolumemin, &alsavolumemax, &alsavolume
            );
            if (!gotvolumes) {
                continue;
            }
            const QString alsaelementchannelname = QString::fromLocal8Bit(snd_mixer_selem_channel_name(alsaelementchannel));
            Plasma::Slider* slider = new Plasma::Slider(frame);
            slider->setProperty("_k_index", alsaelementindex);
            slider->setProperty("_k_name", alsaelementname);
            slider->setProperty("_k_channel", int(alsaelementchannel));
            slider->setProperty("_k_capture", alsahascapture);
            slider->setOrientation(Qt::Vertical);
            slider->setRange(int(alsavolumemin), int(alsavolumemax));
            slider->setValue(int(alsavolume));
            slider->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Expanding);
            slider->setMinimumSize(s_minimumslidersize);
            slider->setToolTip(alsaelementchannelname);
            connect(
                slider, SIGNAL(sliderMoved(int)),
                this, SLOT(slotSliderMovedOrChanged(int))
            );
            connect(
                slider, SIGNAL(valueChanged(int)),
                this, SLOT(slotSliderMovedOrChanged(int))
            );
            m_sliders.append(slider);
            framelayout->addItem(slider, 0, columncounter, 1, 1);
            columncounter++;
        }
        Plasma::IconWidget* iconwidget = new Plasma::IconWidget(frame);
        iconwidget->setIcon(kIconForChannel(alsaelementname));
        iconwidget->setToolTip(alsaelementname);
        iconwidget->setAcceptHoverEvents(false);
        iconwidget->setMinimumIconSize(smalliconsizef);
        iconwidget->setMaximumIconSize(smalliconsizef);
        framelayout->addItem(iconwidget, 1, 0, 1, columncounter);
        framelayout->setAlignment(iconwidget, Qt::AlignCenter);
        frame->setLayout(framelayout);

        m_layout->addItem(frame);
        columncounter++;

        if (m_mainelement.isEmpty() && kIsMasterChannel(alsaelementname)) {
            m_mainelement = alsaelementname;
        } else {
            alsaelementnames.append(alsaelementname);
        }
    }
    if (m_mainelement.isEmpty() && alsaelementnames.size() > 0) {
        // pick the first as main if there is no master
        m_mainelement = alsaelementnames.first();
    }
    kDebug() << "Main element is" << m_mainelement;
    m_layout->addStretch();

    adjustSize();
    return hasvalidelement;
}

QIcon MixerTabWidget::mainVolumeIcon()
{
    if (m_mainelement.isEmpty()) {
        return KIcon(s_defaultpopupicon);
    }
    snd_mixer_elem_t *alsaelement = snd_mixer_first_elem(m_alsamixer);
    for (; alsaelement; alsaelement = snd_mixer_elem_next(alsaelement)) {
        if (snd_mixer_elem_empty(alsaelement)) {
            continue;
        }
        const QString alsaelementname = QString::fromLocal8Bit(snd_mixer_selem_get_name(alsaelement));
        if (alsaelementname != m_mainelement) {
            continue;
        }
        // the icon represents the highest volume from all element channels
        int alsaelementvolume = 0;
        const bool alsahascapture = snd_mixer_selem_has_capture_volume(alsaelement);
        const QList<snd_mixer_selem_channel_id_t> alsaelementchannels = kALSAChannelTypes(alsaelement, alsahascapture);
        foreach (const snd_mixer_selem_channel_id_t alsaelementchannel, alsaelementchannels) {
            long alsavolumemin = 0;
            long alsavolumemax = 0;
            long alsavolume = 0;
            const bool gotvolumes = kGetChannelVolumes(
                alsaelement, alsaelementchannel, alsahascapture,
                &alsavolumemin, &alsavolumemax, &alsavolume
            );
            if (!gotvolumes) {
                return KIcon(s_defaultpopupicon);
            }
            alsaelementvolume = qMax(alsaelementvolume, kFixedVolume(alsavolume, alsavolumemax));
        }
        return kMixerIcon(this, alsaelementvolume);
    }
    return KIcon(s_defaultpopupicon);
}

void MixerTabWidget::decreaseVolume()
{
    foreach (Plasma::Slider *slider, m_sliders) {
        const QString alsaelementname = slider->property("_k_name").toString();
        if (alsaelementname == m_mainelement) {
            slider->setValue(kStepVolume(slider->value(), slider->maximum(), -kVolumeStep()));
        }
    }
}
void MixerTabWidget::increaseVolume()
{
    foreach (Plasma::Slider *slider, m_sliders) {
        const QString alsaelementname = slider->property("_k_name").toString();
        if (alsaelementname == m_mainelement) {
            slider->setValue(kStepVolume(slider->value(), slider->maximum(), kVolumeStep()));
        }
    }
}

void MixerTabWidget::slotSliderMovedOrChanged(const int value)
{
    Q_ASSERT(m_alsamixer != nullptr);
    Plasma::Slider* slider = qobject_cast<Plasma::Slider*>(sender());
    Q_ASSERT(slider != nullptr);
    const uint alsaelementindex = slider->property("_k_index").toUInt();
    const int alsaelementchannel = slider->property("_k_channel").toInt();
    const bool alsahascapture = slider->property("_k_capture").toBool();
    snd_mixer_elem_t *alsaelement = snd_mixer_first_elem(m_alsamixer);
    for (; alsaelement; alsaelement = snd_mixer_elem_next(alsaelement)) {
        if (snd_mixer_elem_empty(alsaelement)) {
            continue;
        }
        if (snd_mixer_selem_get_index(alsaelement) == alsaelementindex) {
            kDebug() << "Changing" << alsaelementindex << "volume to" << value;
            if (alsahascapture) {
                const int alsaresult = snd_mixer_selem_set_capture_volume(alsaelement, snd_mixer_selem_channel_id_t(alsaelementchannel), long(value));
                if (alsaresult != 0) {
                    kWarning() << "Could not set capture volume" << snd_strerror(alsaresult);
                    return;
                }
            } else {
                const int alsaresult = snd_mixer_selem_set_playback_volume(alsaelement, snd_mixer_selem_channel_id_t(alsaelementchannel), long(value));
                if (alsaresult != 0) {
                    kWarning() << "Could not set playback volume" << snd_strerror(alsaresult);
                    return;
                }
            }
            const QString alsaelementname = QString::fromLocal8Bit(snd_mixer_selem_get_name(alsaelement));
            if (alsaelementname == m_mainelement) {
                emit mainVolumeChanged();
            }
            return;
        }
    }
    kWarning() << "Could not find the element" << alsaelementindex;
}


class MixerWidget : public Plasma::TabBar
{
    Q_OBJECT
public:
    MixerWidget(MixerApplet *mixer);

    void decreaseVolume();
    void increaseVolume();

public Q_SLOTS:
    void slotUnhack();

protected:
    // QGraphicsWidget reimplementation
    QSizeF sizeHint(Qt::SizeHint which, const QSizeF &constraint = QSizeF()) const final;

private Q_SLOTS:
    void slotMainVolumeChanged();
    void slotCurrentChanged(const int index);

private:
    MixerApplet* m_mixer;
    bool m_hintshack;
    QList<MixerTabWidget*> m_tabwidgets;
};

MixerWidget::MixerWidget(MixerApplet* mixer)
    : Plasma::TabBar(mixer),
    m_mixer(mixer),
    m_hintshack(true)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setMinimumSize(s_minimumsize);

    int alsacard = -1;
    while (true) {
        int alsaresult = snd_card_next(&alsacard);
        if (alsaresult != 0) {
            kWarning() << "Could not get card" << snd_strerror(alsaresult);
            break;
        }

        const QByteArray alsacardname = (alsacard == -1 ? "default" : "hw:" + QByteArray::number(alsacard));
        snd_ctl_t *alsactl = nullptr;
        alsaresult = snd_ctl_open(&alsactl, alsacardname.constData(), SND_CTL_NONBLOCK);
        if (alsaresult != 0) {
            kWarning() << "Could not open card" << snd_strerror(alsaresult);
            break;
        }

        snd_ctl_card_info_t *alsacardinfo = nullptr;
        snd_ctl_card_info_alloca(&alsacardinfo);
        alsaresult = snd_ctl_card_info(alsactl, alsacardinfo);
        if (alsaresult != 0) {
            kWarning() << "Could not open card" << snd_strerror(alsaresult);
            snd_ctl_close(alsactl);
            break;
        }

        const QString alsamixername = QString::fromLocal8Bit(snd_ctl_card_info_get_mixername(alsacardinfo));
        snd_ctl_close(alsactl);

        MixerTabWidget* mixertabwidget = new MixerTabWidget(this);
        if (mixertabwidget->setup(alsacard, alsacardname)) {
            if (alsacard == -1) {
                // default sound card goes to the front
                insertTab(0, alsamixername, mixertabwidget);
                m_tabwidgets.prepend(mixertabwidget);
            } else {
                addTab(alsamixername, mixertabwidget);
                m_tabwidgets.append(mixertabwidget);
            }
            connect(
                mixertabwidget, SIGNAL(mainVolumeChanged()),
                this, SLOT(slotMainVolumeChanged())
            );
        } else {
            delete mixertabwidget;
        }

        if (alsacard == -1) {
            break;
        }
        alsacard++;
    }

    if (m_tabwidgets.size() > 0) {
        m_mixer->setStatus(Plasma::ItemStatus::ActiveStatus);
    } else {
        m_mixer->setFailedToLaunch(true, i18n("No sound cards found"));
        m_mixer->setStatus(Plasma::ItemStatus::PassiveStatus);
    }
    setTabBarShown(m_tabwidgets.size() > 1);
    setCurrentIndex(0);
    if (m_tabwidgets.size() > 0) {
        m_mixer->setPopupIcon(m_tabwidgets.first()->mainVolumeIcon());
    }
    connect(
        this, SIGNAL(currentChanged(int)),
        this, SLOT(slotCurrentChanged(int))
    );
}

void MixerWidget::decreaseVolume()
{
    if (m_tabwidgets.size() < 1) {
        return;
    }
    MixerTabWidget* mixertabwidget = m_tabwidgets.at(currentIndex());
    mixertabwidget->decreaseVolume();
}

void MixerWidget::increaseVolume()
{
    if (m_tabwidgets.size() < 1) {
        return;
    }
    MixerTabWidget* mixertabwidget = m_tabwidgets.at(currentIndex());
    mixertabwidget->increaseVolume();
}

void MixerWidget::slotUnhack()
{
    m_hintshack = false;
}

QSizeF MixerWidget::sizeHint(Qt::SizeHint which, const QSizeF &constraint) const
{
    // HACK: because Plasma::TabBar minimum size is bogus during initialization when there is nothing
    // in it a hardcoded minimum size is returned, after that it is up to Plasma::TabBar and its
    // child items to return something sensible (hint: that may not happen)
    if (m_hintshack && which == Qt::MinimumSize) {
        return s_minimumsize * 1.3;
    }
    return Plasma::TabBar::sizeHint(which, constraint);
}

void MixerWidget::slotMainVolumeChanged()
{
    MixerTabWidget* mixertabwidget = qobject_cast<MixerTabWidget*>(sender());
    if (m_tabwidgets.indexOf(mixertabwidget) == currentIndex()) {
        m_mixer->setPopupIcon(mixertabwidget->mainVolumeIcon());
    }
}

void MixerWidget::slotCurrentChanged(const int index)
{
    Q_ASSERT(index < m_tabwidgets.size());
    MixerTabWidget* mixertabwidget = m_tabwidgets.at(index);
    Q_ASSERT(mixertabwidget != nullptr);
    m_mixer->setPopupIcon(mixertabwidget->mainVolumeIcon());
}


MixerApplet::MixerApplet(QObject *parent, const QVariantList &args)
    : Plasma::PopupApplet(parent, args),
    m_mixerwidget(nullptr)
{
    KGlobal::locale()->insertCatalog("plasma_applet_mixer");
    setAspectRatioMode(Plasma::AspectRatioMode::IgnoreAspectRatio);
    m_mixerwidget = new MixerWidget(this);
    setPopupIcon(s_defaultpopupicon);
}

MixerApplet::~MixerApplet()
{
    delete m_mixerwidget;
}

QGraphicsWidget* MixerApplet::graphicsWidget()
{
    return m_mixerwidget;
}

void MixerApplet::wheelEvent(QGraphicsSceneWheelEvent *event)
{
    if (event->delta() < 0) {
        m_mixerwidget->decreaseVolume();
    } else {
        m_mixerwidget->increaseVolume();
    }
}

void MixerApplet::constraintsEvent(Plasma::Constraints constraints)
{
    if (constraints & Plasma::FormFactorConstraint) {
        switch (formFactor()) {
            case Plasma::FormFactor::Horizontal:
            case Plasma::FormFactor::Vertical: {
                // HACK: limit the widget size to 2-times less than that of the desktop because
                // Plasma::TabBar sets its maximum size to QWIDGETSIZE_MAX which is more than what
                // can fit on panel and for some reason hints do not have effect on the widget size
                // when it is in a panel, see:
                // kdelibs/plasma/widgets/tabbar.cpp
                const QSize desktopsize = qApp->desktop()->size();
                m_mixerwidget->setMaximumSize(desktopsize / 2);
                break;
            }
            default: {
                // back to the Plasma::TabBar maximum on form factor switch
                m_mixerwidget->setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
                break;
            }
        }
    }
    if (constraints & Plasma::StartupCompletedConstraint) {
        QTimer::singleShot(500, m_mixerwidget, SLOT(slotUnhack()));
    }
}

K_EXPORT_PLASMA_APPLET(mixer, MixerApplet)

#include "moc_mixer.cpp"
#include "mixer.moc"
