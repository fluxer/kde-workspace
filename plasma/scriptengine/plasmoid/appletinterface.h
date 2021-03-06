/*
 *   Copyright 2008 Chani Armitage <chani@kde.org>
 *   Copyright 2008, 2009 Aaron Seigo <aseigo@kde.org>
 *   Copyright 2010 Marco Martin <mart@kde.org>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as
 *   published by the Free Software Foundation; either version 2, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef APPLETINTERFACE_H
#define APPLETINTERFACE_H

#include <QAbstractAnimation>
#include <QObject>
#include <QSizePolicy>
#include <QScriptValue>

#include <Plasma/Applet>
#include <Plasma/Containment>
#include <Plasma/PopupApplet>
#include <Plasma/DataEngine>
#include <Plasma/Theme>
#include <Plasma/ToolTipContent>

#include "abstractjsappletscript.h"
#include "../declarative/toolboxproxy.h"

#include <QAction>
class QmlAppletScript;
#include <QSignalMapper>
#include <QSizeF>


namespace Plasma
{
    class ConfigLoader;
    class Extender;
} // namespace Plasa

class AppletInterface : public QObject
{
    Q_OBJECT
    Q_ENUMS(FormFactor)
    Q_ENUMS(Location)
    Q_ENUMS(AspectRatioMode)
    Q_ENUMS(BackgroundHints)
    Q_ENUMS(QtOrientation)
    Q_ENUMS(QtModifiers)
    Q_ENUMS(QtAnchorPoint)
    Q_ENUMS(QtCorner)
    Q_ENUMS(QtSizePolicy)
    Q_ENUMS(QtAlignment)
    Q_ENUMS(QtMouseButton)
    Q_ENUMS(AnimationDirection)
    Q_ENUMS(IntervalAlignment)
    Q_ENUMS(ThemeColors)
    Q_ENUMS(ItemStatus)
    Q_PROPERTY(AspectRatioMode aspectRatioMode READ aspectRatioMode WRITE setAspectRatioMode)
    Q_PROPERTY(FormFactor formFactor READ formFactor NOTIFY formFactorChanged)
    Q_PROPERTY(Location location READ location NOTIFY locationChanged)
    Q_PROPERTY(bool shouldConserveResources READ shouldConserveResources)
    Q_PROPERTY(QString activeConfig WRITE setActiveConfig READ activeConfig)
    Q_PROPERTY(bool busy WRITE setBusy READ isBusy)
    Q_PROPERTY(BackgroundHints backgroundHints WRITE setBackgroundHints READ backgroundHints)
    Q_PROPERTY(bool immutable READ immutable NOTIFY immutableChanged)
    Q_PROPERTY(bool userConfiguring READ userConfiguring) // @since 4.5
    Q_PROPERTY(ItemStatus status READ status WRITE setStatus NOTIFY statusChanged)
    Q_PROPERTY(QRectF rect READ rect)
    Q_PROPERTY(QSizeF size READ size)
    Q_PROPERTY(QString associatedApplication WRITE setAssociatedApplication READ associatedApplication)
    Q_PROPERTY(QtSizePolicy horizontalSizePolicy READ horizontalSizePolicy WRITE setHorizontalSizePolicy)
    Q_PROPERTY(QtSizePolicy verticalSizePolicy READ verticalSizePolicy WRITE setVerticalSizePolicy)

public:
    AppletInterface(AbstractJsAppletScript *parent);
    ~AppletInterface();

//------------------------------------------------------------------
//enums copy&pasted from plasma.h because qtscript is evil

enum FormFactor {
    Planar = 0,  /**< The applet lives in a plane and has two
                    degrees of freedom to grow. Optimize for
                    desktop, laptop or tablet usage: a high
                    resolution screen 1-3 feet distant from the
                    viewer. */
    MediaCenter, /**< As with Planar, the applet lives in a plane
                    but the interface should be optimized for
                    medium-to-high resolution screens that are
                    5-15 feet distant from the viewer. Sometimes
                    referred to as a "ten foot interface".*/
    Horizontal,  /**< The applet is constrained vertically, but
                    can expand horizontally. */
    Vertical,     /**< The applet is constrained horizontally, but
                    can expand vertically. */
    Application /**< The Applet lives in a plane and should be optimized to look as a full application,
                     for the desktop or the particular device. */
};

enum Location {
    Floating = 0, /**< Free floating. Neither geometry or z-ordering
                     is described precisely by this value. */
    Desktop,      /**< On the planar desktop layer, extending across
                     the full screen from edge to edge */
    FullScreen,   /**< Full screen */
    TopEdge,      /**< Along the top of the screen*/
    BottomEdge,   /**< Along the bottom of the screen*/
    LeftEdge,     /**< Along the left side of the screen */
    RightEdge     /**< Along the right side of the screen */
};

enum AspectRatioMode {
    InvalidAspectRatioMode = -1, /**< Unsetted mode used for dev convenience
                                    when there is a need to store the
                                    aspectRatioMode somewhere */
    IgnoreAspectRatio = 0,       /**< The applet can be freely resized */
    KeepAspectRatio = 1,         /**< The applet keeps a fixed aspect ratio */
    Square = 2,                  /**< The applet is always a square */
    ConstrainedSquare = 3,       /**< The applet is no wider (in horizontal
                                    formfactors) or no higher (in vertical
                                    ones) than a square */
    FixedSize = 4                /** The applet cannot be resized */
};

enum ItemStatus {
    UnknownStatus = 0, /**< The status is unknown **/
    PassiveStatus = 1, /**< The Item is passive **/
    ActiveStatus = 2, /**< The Item is active **/
    NeedsAttentionStatus = 3, /**< The Item needs attention **/
    AcceptingInputStatus = 4 /**< The Item is accepting input **/
};

//From Qt namespace
enum QtModifiers {
    QtNoModifier = Qt::NoModifier,
    QtShiftModifier = Qt::ShiftModifier,
    QtControlModifier = Qt::ControlModifier,
    QtAltModifier = Qt::AltModifier,
    QtMetaModifier = Qt::MetaModifier
};

enum QtOrientation {
    QtHorizontal= Qt::Horizontal,
    QtVertical = Qt::Vertical
};

enum QtAnchorPoint {
    QtAnchorLeft = Qt::AnchorLeft,
    QtAnchorRight = Qt::AnchorRight,
    QtAnchorBottom = Qt::AnchorBottom,
    QtAnchorTop = Qt::AnchorTop,
    QtAnchorHorizontalCenter = Qt::AnchorHorizontalCenter,
    QtAnchorVerticalCenter = Qt::AnchorVerticalCenter
};

enum QtCorner {
    QtTopLeftCorner = Qt::TopLeftCorner,
    QtTopRightCorner = Qt::TopRightCorner,
    QtBottomLeftCorner = Qt::BottomLeftCorner,
    QtBottomRightCorner = Qt::BottomRightCorner
};

enum QtSizePolicy {
    QSizePolicyFixed = QSizePolicy::Fixed,
    QSizePolicyMinimum = QSizePolicy::Minimum,
    QSizePolicyMaximum = QSizePolicy::Maximum,
    QSizePolicyPreferred = QSizePolicy::Preferred,
    QSizePolicyExpanding = QSizePolicy::Expanding,
    QSizePolicyMinimumExpanding = QSizePolicy::MinimumExpanding,
    QSizePolicyIgnored = QSizePolicy::Ignored
};

enum BackgroundHints {
    NoBackground = Plasma::Applet::NoBackground,
    StandardBackground = Plasma::Applet::StandardBackground,
    TranslucentBackground = Plasma::Applet::TranslucentBackground,
    DefaultBackground = Plasma::Applet::DefaultBackground
};

enum ThemeColors {
    TextColor = Plasma::Theme::TextColor,
    HighlightColor = Plasma::Theme::HighlightColor,
    BackgroundColor = Plasma::Theme::BackgroundColor,
    ButtonTextColor = Plasma::Theme::ButtonTextColor,
    ButtonBackgroundColor = Plasma::Theme::ButtonBackgroundColor,
    LinkColor = Plasma::Theme::LinkColor,
    VisitedLinkColor = Plasma::Theme::VisitedLinkColor
};

enum QtAlignment {
    QtAlignLeft = 0x0001,
    QtAlignRight = 0x0002,
    QtAlignHCenter = 0x0004,
    QtAlignJustify = 0x0005,
    QtAlignTop = 0x0020,
    QtAlignBottom = 0x0020,
    QtAlignVCenter = 0x0080
};

enum QtMouseButton {
    QtNoButton = Qt::NoButton,
    QtLeftButton = Qt::LeftButton,
    QtRightButton = Qt::RightButton,
    QtMidButton = Qt::MiddleButton
};

enum QtScrollBarPolicy {
    QtScrollBarAsNeeded = Qt::ScrollBarAsNeeded,
    QtScrollBarAlwaysOff = Qt::ScrollBarAlwaysOff,
    QtScrollBarAlwaysOn = Qt::ScrollBarAlwaysOn
};

enum AnimationDirection {
    AnimationForward = QAbstractAnimation::Forward,
    AnimationBackward = QAbstractAnimation::Backward
};

enum IntervalAlignment {
    NoAlignment = 0,
    AlignToMinute,
    AlignToHour
};

//-------------------------------------------------------------------

    Q_INVOKABLE void gc();
    Q_INVOKABLE FormFactor formFactor() const;

    Location location() const;
    bool shouldConserveResources() const;

    Q_INVOKABLE AspectRatioMode aspectRatioMode() const;
    Q_INVOKABLE void setAspectRatioMode(AspectRatioMode mode);

    Q_INVOKABLE void setFailedToLaunch(bool failed, const QString &reason = QString());

    Q_INVOKABLE bool isBusy() const;
    Q_INVOKABLE void setBusy(bool busy);

    Q_INVOKABLE BackgroundHints backgroundHints() const;
    Q_INVOKABLE void setBackgroundHints(BackgroundHints hint);

    Q_INVOKABLE void setConfigurationRequired(bool needsConfiguring, const QString &reason = QString());

    Q_INVOKABLE QSizeF size() const;
    Q_INVOKABLE QRectF rect() const;

    Q_INVOKABLE void setActionSeparator(const QString &name);
    Q_INVOKABLE void setAction(const QString &name, const QString &text,
                               const QString &icon = QString(), const QString &shortcut = QString());

    Q_INVOKABLE void removeAction(const QString &name);

    Q_INVOKABLE QAction *action(QString name) const;

    Q_INVOKABLE void resize(qreal w, qreal h);

    Q_INVOKABLE void setMinimumSize(qreal w, qreal h);

    Q_INVOKABLE void setPreferredSize(qreal w, qreal h);

    Q_INVOKABLE QString activeConfig() const;

    Q_INVOKABLE void setActiveConfig(const QString &name);

    Q_INVOKABLE QScriptValue readConfig(const QString &entry) const;

    Q_INVOKABLE void writeConfig(const QString &entry, const QVariant &value);

    Q_INVOKABLE QString file(const QString &fileType);
    Q_INVOKABLE QString file(const QString &fileType, const QString &filePath);

    Q_INVOKABLE bool include(const QString &script);

    Q_INVOKABLE void debug(const QString &msg);
    Q_INVOKABLE QObject *findChild(const QString &name) const;

    Q_INVOKABLE Plasma::Extender *extender() const;

    Plasma::DataEngine *dataEngine(const QString &name);

    QList<QAction*> contextualActions() const;
    bool immutable() const;
    bool userConfiguring() const;

    static AppletInterface *extract(QScriptEngine *engine);
    inline Plasma::Applet *applet() const { return m_appletScriptEngine->applet(); }

    void setAssociatedApplication(const QString &string);
    QString associatedApplication() const;

    void setStatus(const ItemStatus &status);
    ItemStatus status() const;

    void setHorizontalSizePolicy(QtSizePolicy policy);
    QtSizePolicy horizontalSizePolicy() const;

    void setVerticalSizePolicy(QtSizePolicy policy);
    QtSizePolicy verticalSizePolicy() const;

//    Q_INVOKABLE QString downloadPath(const QString &file);
    Q_INVOKABLE QStringList downloadedFiles() const;

Q_SIGNALS:
    void releaseVisualFocus();
    void configNeedsSaving();

    void formFactorChanged();
    void locationChanged();
    void immutableChanged();
    void statusChanged();

protected:
    AbstractJsAppletScript *m_appletScriptEngine;

private:
    QStringList m_actions;
    QSignalMapper *m_actionSignals;
    QString m_currentConfig;
    QMap<QString, Plasma::ConfigLoader*> m_configs;
};

class PopupAppletInterface : public AppletInterface
{
    Q_OBJECT
    Q_PROPERTY(QIcon popupIcon READ popupIcon WRITE setPopupIcon)
    Q_PROPERTY(bool passivePopup READ isPassivePopup WRITE setPassivePopup)
    Q_PROPERTY(QGraphicsWidget *popupWidget READ popupWidget WRITE setPopupWidget)
    Q_PROPERTY(QVariantHash popupIconToolTip READ popupIconToolTip WRITE setPopupIconToolTip NOTIFY popupIconToolTipChanged)
    Q_PROPERTY(bool popupShowing READ isPopupShowing WRITE setPopupShowing NOTIFY popupEvent)

public:
    PopupAppletInterface(AbstractJsAppletScript *parent);

    void setPopupIcon(const QIcon &icon);
    QIcon popupIcon();

    void setPopupIconToolTip(const QVariantHash &data);
    QVariantHash popupIconToolTip() const;

    inline Plasma::PopupApplet *popupApplet() const { return static_cast<Plasma::PopupApplet *>(m_appletScriptEngine->applet()); }

    void setPassivePopup(bool passive);
    bool isPassivePopup() const;

    bool isPopupShowing() const;
    void setPopupShowing(bool show);

    void setPopupWidget(QGraphicsWidget *widget);
    QGraphicsWidget *popupWidget();

Q_SIGNALS:
    void popupEvent(bool popupShowing);
    void popupIconToolTipChanged();

public Q_SLOTS:
    void setPopupIconByName(const QString &name);
    void togglePopup();
    void hidePopup();
    void showPopup();
    void showPopup(int timeout);

private Q_SLOTS:
    void sourceAppletPopupEvent(bool show);

private:
    QVariantHash m_rawToolTipData;
    Plasma::ToolTipContent m_toolTipData;
};


class ContainmentInterface : public AppletInterface
{
    Q_OBJECT
    Q_PROPERTY(QScriptValue applets READ applets)
    Q_PROPERTY(bool drawWallpaper READ drawWallpaper WRITE setDrawWallpaper)
    Q_PROPERTY(Type containmentType READ containmentType WRITE setContainmentType)
    Q_PROPERTY(int screen READ screen NOTIFY screenChanged)
    Q_PROPERTY(bool movableApplets READ hasMovableApplets WRITE setMovableApplets)
    Q_PROPERTY(ToolBoxProxy* toolBox READ toolBox CONSTANT)
    Q_ENUMS(Type)

public:
    enum Type {
        NoContainmentType = -1,  /**< @internal */
        DesktopContainment = 0,  /**< A desktop containment */
        PanelContainment,        /**< A desktop panel */
        CustomContainment = 127, /**< A containment that is neither a desktop nor a panel
                                    but something application specific */
        CustomPanelContainment = 128 /**< A customized desktop panel */
    };
    ContainmentInterface(AbstractJsAppletScript *parent);

    inline Plasma::Containment *containment() const { return static_cast<Plasma::Containment *>(m_appletScriptEngine->applet()); }

    QScriptValue applets();

    void setDrawWallpaper(bool drawWallpaper);
    bool drawWallpaper();
    Type containmentType() const;
    void setContainmentType(Type type);
    int screen() const;

    void setMovableApplets(bool movable);
    bool hasMovableApplets() const;

    ToolBoxProxy* toolBox();

    Q_INVOKABLE QScriptValue screenGeometry(int id) const;
    Q_INVOKABLE QScriptValue availableScreenRegion(int id) const;

Q_SIGNALS:
    void appletAdded(QGraphicsWidget *applet, const QPointF &pos);
    void appletRemoved(QGraphicsWidget *applet);
    void screenChanged();
    void availableScreenRegionChanged();

protected Q_SLOTS:
    void appletAddedForward(Plasma::Applet *applet, const QPointF &pos);
    void appletRemovedForward(Plasma::Applet *applet);

private:
    bool m_movableApplets;
    ToolBoxProxy* m_toolBox;
};

#endif
