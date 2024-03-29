/*
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

#ifndef THEME_PROXY_P
#define THEME_PROXY_P

#include <QObject>

#include <KUrl>
#include <QFont>
#include <QColor>

#include <Plasma/Theme>

#include <QDeclarativePropertyMap>

class FontProxy : public QObject
{
    Q_OBJECT

    /**
     * true if the font is bold
     */
    Q_PROPERTY(bool bold READ bold NOTIFY boldChanged)

    /**
     * name of the font family
     */
    Q_PROPERTY(QString family READ family NOTIFY familyChanged )

    /**
     * true if the font is italic
     */
    Q_PROPERTY(bool italic READ italic NOTIFY italicChanged )

    /**
     * Size of the font in pixels: settings this is strongly discouraged.
     * @see pointSize
     */
    Q_PROPERTY(int pixelSize READ pixelSize NOTIFY pixelSizeChanged )

    /**
     * Size of the font in points
     */
    Q_PROPERTY(qreal pointSize READ pointSize NOTIFY pointSizeChanged )

    /**
     * True if the text is striked out with an horizontal line
     */
    Q_PROPERTY(bool strikeout READ strikeout NOTIFY strikeoutChanged )

    /**
     * True if all the text will be underlined
     */
    Q_PROPERTY(bool underline READ underline NOTIFY underlineChanged )

    /**
     * One of:
     * Light
     * Normal
     * DemiBold
     * Bold
     * Black
     */
    Q_PROPERTY(Weight weight READ weight NOTIFY weightChanged )

    /**
     * Size in pixels of an uppercase "M" letter
     */
    Q_PROPERTY(QSize mSize READ mSize NOTIFY mSizeChanged )

    Q_ENUMS(Weight)

public:
    enum Weight {
        Light = 25,
        Normal = 50,
        DemiBold = 63,
        Bold = 75,
        Black = 87
    };

    FontProxy(Plasma::Theme::FontRole role, QObject *parent = 0);
    ~FontProxy();
    static FontProxy *defaultFont();
    static FontProxy *desktopFont();
    static FontProxy *smallestFont();

    bool bold() const;
    QString family() const;
    bool italic() const;
    int pixelSize() const;
    qreal pointSize() const;
    bool strikeout() const;
    bool underline() const;
    Weight weight() const;

    /**
     * @return The size of an uppercase M in this font
     */
    QSize mSize() const;

Q_SIGNALS:
    void boldChanged();
    void familyChanged();
    void italicChanged();
    void pixelSizeChanged();
    void pointSizeChanged();
    void strikeoutChanged();
    void underlineChanged();
    void weightChanged();
    void mSizeChanged();

private:
    Plasma::Theme::FontRole m_fontRole;
};

/**
 * QML wrapper for kdelibs Plasma::Theme
 *
 * Exposed as `Theme` in QML.
 */
class ThemeProxy : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString themeName READ themeName NOTIFY themeChanged)
    Q_PROPERTY(bool windowTranslucentEnabled READ windowTranslucencyEnabled NOTIFY themeChanged)
    Q_PROPERTY(KUrl homepage READ homepage NOTIFY themeChanged)
    Q_PROPERTY(bool useGlobalSettings READ useGlobalSettings NOTIFY themeChanged)
    Q_PROPERTY(QString wallpaperPath READ wallpaperPath NOTIFY themeChanged)

    //fonts
    Q_PROPERTY(QObject *defaultFont READ defaultFont CONSTANT)
    Q_PROPERTY(QObject *desktopFont READ desktopFont CONSTANT)
    Q_PROPERTY(QObject *smallestFont READ smallestFont CONSTANT)

    // colors
    Q_PROPERTY(QColor textColor READ textColor NOTIFY themeChanged)
    Q_PROPERTY(QColor highlightColor READ highlightColor NOTIFY themeChanged)
    Q_PROPERTY(QColor backgroundColor READ backgroundColor NOTIFY themeChanged)
    Q_PROPERTY(QColor buttonTextColor READ buttonTextColor NOTIFY themeChanged)
    Q_PROPERTY(QColor buttonBackgroundColor READ buttonBackgroundColor NOTIFY themeChanged)
    Q_PROPERTY(QColor linkColor READ linkColor NOTIFY themeChanged)
    Q_PROPERTY(QColor visitedLinkColor READ visitedLinkColor NOTIFY themeChanged)
    Q_PROPERTY(QColor visitedLinkColor READ visitedLinkColor NOTIFY themeChanged)
    Q_PROPERTY(QColor buttonHoverColor READ buttonHoverColor NOTIFY themeChanged)
    Q_PROPERTY(QColor buttonFocusColor READ buttonFocusColor NOTIFY themeChanged)
    Q_PROPERTY(QColor viewTextColor READ viewTextColor NOTIFY themeChanged)
    Q_PROPERTY(QColor viewBackgroundColor READ viewBackgroundColor NOTIFY themeChanged)
    Q_PROPERTY(QColor viewHoverColor READ viewHoverColor NOTIFY themeChanged)
    Q_PROPERTY(QColor viewFocusColor READ viewFocusColor NOTIFY themeChanged)
    Q_PROPERTY(QString styleSheet READ styleSheet NOTIFY themeChanged)

    // icon sizes
    Q_PROPERTY(int smallIconSize READ smallIconSize CONSTANT)
    Q_PROPERTY(int smallMediumIconSize READ smallMediumIconSize CONSTANT)
    Q_PROPERTY(int mediumIconSize READ mediumIconSize CONSTANT)
    Q_PROPERTY(int largeIconSize READ largeIconSize CONSTANT)
    Q_PROPERTY(int hugeIconSize READ hugeIconSize CONSTANT)
    Q_PROPERTY(int enormousIconSize READ enormousIconSize CONSTANT)
    Q_PROPERTY(int defaultIconSize READ defaultIconSize NOTIFY defaultIconSizeChanged)

    /**
     * icon sizes depending from the context: use those if possible
     * Access with theme.iconSizes.desktop theme.iconSizes.small etc.
     * available keys are:
     * * desktop
     * * toolbar
     * * small
     * * dialog
     */
    Q_PROPERTY(QDeclarativePropertyMap *iconSizes READ iconSizes NOTIFY iconSizesChanged)

public:
    ThemeProxy(QObject *parent = 0);
    ~ThemeProxy();

    QString themeName() const;
    QObject *defaultFont() const;
    QObject *desktopFont() const;
    QObject *smallestFont() const;
    bool windowTranslucencyEnabled() const;
    KUrl homepage() const;
    bool useGlobalSettings() const;
    QString wallpaperPath() const;
    Q_INVOKABLE QString wallpaperPathForSize(int width=-1, int height=-1) const;

    QColor textColor() const;
    QColor highlightColor() const;
    QColor backgroundColor() const;
    QColor buttonTextColor() const;
    QColor buttonBackgroundColor() const;
    QColor linkColor() const;
    QColor visitedLinkColor() const;
    QColor buttonHoverColor() const;
    QColor buttonFocusColor() const;
    QColor viewTextColor() const;
    QColor viewBackgroundColor() const;
    QColor viewHoverColor() const;
    QColor viewFocusColor() const;
    QString styleSheet() const;

    int smallIconSize() const;
    int smallMediumIconSize() const;
    int mediumIconSize() const;
    int largeIconSize() const;
    int hugeIconSize() const;
    int enormousIconSize() const;
    int defaultIconSize() const;
    QDeclarativePropertyMap *iconSizes() const;

private Q_SLOTS:
    void iconLoaderSettingsChanged();

Q_SIGNALS:
    void themeChanged();
    void defaultIconSizeChanged();
    void iconSizesChanged();

private:
    int m_defaultIconSize;
    QDeclarativePropertyMap *m_iconSizes;
};

#endif
