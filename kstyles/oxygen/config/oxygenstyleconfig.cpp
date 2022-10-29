/*
Copyright 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
Copyright 2009 Matthew Woehlke <mw.triad@users.sourceforge.net>
Copyright 2009 Long Huynh Huu <long.upcase@googlemail.com>
Copyright 2003 Sandro Giessl <ceebx@users.sourceforge.net>

originally based on the Keramik configuration dialog:
Copyright 2003 Maksim Orlovich <maksim.orlovich@kdemail.net>

Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.

*/

#include "oxygenstyleconfig.h"
#include "moc_oxygenstyleconfig.cpp"
#include "oxygenstyleconfigdata.h"

#include <QtCore/QTextStream>
#include <QtDBus/QDBusMessage>
#include <QtDBus/QDBusConnection>

#include <KGlobal>
#include <KLocale>
#include <KSharedConfig>
#include <KConfigGroup>
#include <kdemacros.h>
#include <KDialog>

#define SCROLLBAR_DEFAULT_WIDTH 15
#define SCROLLBAR_MINIMUM_WIDTH 10
#define SCROLLBAR_MAXIMUM_WIDTH 30

extern "C"
{
    KDE_EXPORT QWidget* allocate_kstyle_config(QWidget* parent)
    {
        KGlobal::locale()->insertCatalog("kstyle_config");
        return new Oxygen::StyleConfig(parent);
    }
}

namespace Oxygen
{
    //__________________________________________________________________
    StyleConfig::StyleConfig(QWidget* parent):
        QWidget(parent)
    {
        KGlobal::locale()->insertCatalog("kstyle_config");

        setupUi(this);

        // connections
        connect( _windowDragMode, SIGNAL(currentIndexChanged(int)), SLOT(windowDragModeChanged(int)) );

        // updateMinimumSize();

        // load setup from configData
        load();

        connect( _toolBarDrawItemSeparator, SIGNAL(toggled(bool)), SLOT(updateChanged()) );
        connect( _checkDrawX, SIGNAL(toggled(bool)), SLOT(updateChanged()) );
        connect( _mnemonicsMode, SIGNAL(currentIndexChanged(int)), SLOT(updateChanged()) );
        connect( _cacheEnabled, SIGNAL(toggled(bool)), SLOT(updateChanged()) );
        connect( _viewDrawTriangularExpander, SIGNAL(toggled(bool)), SLOT(updateChanged()) );
        connect( _viewTriangularExpanderSize, SIGNAL(currentIndexChanged(int)), SLOT(updateChanged()) );
        connect( _viewDrawFocusIndicator, SIGNAL(toggled(bool)), SLOT(updateChanged()) );
        connect( _viewDrawTreeBranchLines, SIGNAL(toggled(bool)), SLOT(updateChanged()) );
        connect( _scrollBarWidth, SIGNAL(valueChanged(int)), SLOT(updateChanged()) );
        connect( _scrollBarAddLineButtons, SIGNAL(currentIndexChanged(int)), SLOT(updateChanged()) );
        connect( _scrollBarSubLineButtons, SIGNAL(currentIndexChanged(int)), SLOT(updateChanged()) );
        connect( _menuHighlightDark, SIGNAL(toggled(bool)), SLOT(updateChanged()) );
        connect( _menuHighlightStrong, SIGNAL(toggled(bool)), SLOT(updateChanged()) );
        connect( _menuHighlightSubtle, SIGNAL(toggled(bool)), SLOT(updateChanged()) );
        connect( _tabStylePlain, SIGNAL(toggled(bool)), SLOT(updateChanged()) );
        connect( _tabStyleSingle, SIGNAL(toggled(bool)), SLOT(updateChanged()) );
        connect( _windowDragMode, SIGNAL(currentIndexChanged(int)), SLOT(updateChanged()) );
        connect( _useWMMoveResize, SIGNAL(toggled(bool)), SLOT(updateChanged()) );

    }

    //__________________________________________________________________
    void StyleConfig::save( void )
    {
        StyleConfigData::setToolBarDrawItemSeparator( _toolBarDrawItemSeparator->isChecked() );
        StyleConfigData::setCheckBoxStyle( ( _checkDrawX->isChecked() ? StyleConfigData::CS_X : StyleConfigData::CS_CHECK ) );
        StyleConfigData::setMnemonicsMode( _mnemonicsMode->currentIndex() );
        StyleConfigData::setCacheEnabled( _cacheEnabled->isChecked() );
        StyleConfigData::setViewDrawTriangularExpander( _viewDrawTriangularExpander->isChecked() );
        StyleConfigData::setViewTriangularExpanderSize( triangularExpanderSize() );
        StyleConfigData::setViewDrawFocusIndicator( _viewDrawFocusIndicator->isChecked() );
        StyleConfigData::setViewDrawTreeBranchLines( _viewDrawTreeBranchLines->isChecked() );
        StyleConfigData::setScrollBarWidth( _scrollBarWidth->value() );
        StyleConfigData::setScrollBarAddLineButtons( _scrollBarAddLineButtons->currentIndex() );
        StyleConfigData::setScrollBarSubLineButtons( _scrollBarSubLineButtons->currentIndex() );
        StyleConfigData::setMenuHighlightMode( menuMode() );
        StyleConfigData::setTabStyle( tabStyle() );
        StyleConfigData::setViewTriangularExpanderSize( triangularExpanderSize() );

        StyleConfigData::setUseWMMoveResize( _useWMMoveResize->isChecked() );
        if( _windowDragMode->currentIndex() == 0 )
        {

            StyleConfigData::setWindowDragEnabled( false  );

        } else {

            StyleConfigData::setWindowDragEnabled( true  );
            StyleConfigData::setWindowDragMode( windowDragMode()  );

        }

        StyleConfigData::self()->writeConfig();

        // emit dbus signal
        QDBusMessage message( QDBusMessage::createSignal("/OxygenStyle",  "org.kde.Oxygen.Style", "reparseConfiguration") );
        QDBusConnection::sessionBus().send(message);

    }

    //__________________________________________________________________
    void StyleConfig::defaults( void )
    {
        StyleConfigData::self()->setDefaults();
        load();
    }

    //__________________________________________________________________
    void StyleConfig::reset( void )
    {
        StyleConfigData::self()->readConfig();
        load();
    }

    //__________________________________________________________________
    bool StyleConfig::eventFilter( QObject* object, QEvent* event )
    {

        switch( event->type() )
        {

            case QEvent::ShowToParent:
                object->event( event );
                updateLayout();
                return true;

            default:
                return false;
        }
    }

    //__________________________________________________________________
    bool StyleConfig::event( QEvent* event )
    {
        const bool result( QWidget::event( event ) );
        switch( event->type() )
        {
            case QEvent::Show:
            case QEvent::ShowToParent:
                updateMinimumSize();
                break;

            default:
                break;
        }

        return result;

    }

    //__________________________________________________________________
    void StyleConfig::updateMinimumSize( void )
    {
        setMinimumSize( minimumSizeHint() );
    }

    //__________________________________________________________________
    void StyleConfig::updateLayout( void )
    {
    }

    //__________________________________________________________________
    void StyleConfig::updateChanged()
    {

        bool modified( false );

        // check if any value was modified
        if ( _toolBarDrawItemSeparator->isChecked() != StyleConfigData::toolBarDrawItemSeparator() ) modified = true;
        else if( _mnemonicsMode->currentIndex() != StyleConfigData::mnemonicsMode() ) modified = true;
        else if( _viewDrawTriangularExpander->isChecked() != StyleConfigData::viewDrawTriangularExpander() ) modified = true;
        else if( _viewDrawFocusIndicator->isChecked() != StyleConfigData::viewDrawFocusIndicator() ) modified = true;
        else if( _viewDrawTreeBranchLines->isChecked() != StyleConfigData::viewDrawTreeBranchLines() ) modified = true;
        else if( _scrollBarWidth->value() != StyleConfigData::scrollBarWidth() ) modified = true;
        else if( _scrollBarAddLineButtons->currentIndex() != StyleConfigData::scrollBarAddLineButtons() ) modified = true;
        else if( _scrollBarSubLineButtons->currentIndex() != StyleConfigData::scrollBarSubLineButtons() ) modified = true;
        else if( (_checkDrawX->isChecked() ? StyleConfigData::CS_X : StyleConfigData::CS_CHECK) != StyleConfigData::checkBoxStyle() ) modified = true;
        else if( menuMode() != StyleConfigData::menuHighlightMode() ) modified = true;
        else if( tabStyle() != StyleConfigData::tabStyle() ) modified = true;
        else if( _cacheEnabled->isChecked() != StyleConfigData::cacheEnabled() ) modified = true;
        else if( _useWMMoveResize->isChecked() != StyleConfigData::useWMMoveResize() ) modified = true;
        else if( triangularExpanderSize() != StyleConfigData::viewTriangularExpanderSize() ) modified = true;

        if( !modified )
        {
            switch( _windowDragMode->currentIndex() )
            {
                case 0:
                {
                    if( StyleConfigData::windowDragEnabled() ) modified = true;
                    break;
                }

                case 1:
                case 2:
                default:
                {
                    if( !StyleConfigData::windowDragEnabled() || windowDragMode() != StyleConfigData::windowDragMode() )
                    { modified = true; }
                    break;
                }

            }
        }

        emit changed(modified);

    }

    //__________________________________________________________________
    void StyleConfig::load( void )
    {

        _toolBarDrawItemSeparator->setChecked( StyleConfigData::toolBarDrawItemSeparator() );
        _mnemonicsMode->setCurrentIndex( StyleConfigData::mnemonicsMode() );
        _checkDrawX->setChecked( StyleConfigData::checkBoxStyle() == StyleConfigData::CS_X );
        _viewDrawTriangularExpander->setChecked( StyleConfigData::viewDrawTriangularExpander() );
        _viewDrawFocusIndicator->setChecked( StyleConfigData::viewDrawFocusIndicator() );
        _viewDrawTreeBranchLines->setChecked(StyleConfigData::viewDrawTreeBranchLines() );

        _scrollBarWidth->setValue(
            qMin(SCROLLBAR_MAXIMUM_WIDTH, qMax(SCROLLBAR_MINIMUM_WIDTH,
            StyleConfigData::scrollBarWidth())) );

        _scrollBarAddLineButtons->setCurrentIndex( StyleConfigData::scrollBarAddLineButtons() );
        _scrollBarSubLineButtons->setCurrentIndex( StyleConfigData::scrollBarSubLineButtons() );

        // menu highlight
        _menuHighlightDark->setChecked( StyleConfigData::menuHighlightMode() == StyleConfigData::MM_DARK );
        _menuHighlightStrong->setChecked( StyleConfigData::menuHighlightMode() == StyleConfigData::MM_STRONG );
        _menuHighlightSubtle->setChecked( StyleConfigData::menuHighlightMode() == StyleConfigData::MM_SUBTLE );

        // tab style
        _tabStyleSingle->setChecked( StyleConfigData::tabStyle() == StyleConfigData::TS_SINGLE );
        _tabStylePlain->setChecked( StyleConfigData::tabStyle() == StyleConfigData::TS_PLAIN );

        _cacheEnabled->setChecked( StyleConfigData::cacheEnabled() );

        if( !StyleConfigData::windowDragEnabled() ) _windowDragMode->setCurrentIndex(0);
        else if( StyleConfigData::windowDragMode() == StyleConfigData::WD_MINIMAL ) _windowDragMode->setCurrentIndex(1);
        else _windowDragMode->setCurrentIndex(2);

        switch( StyleConfigData::viewTriangularExpanderSize() )
        {
            case StyleConfigData::TE_TINY: _viewTriangularExpanderSize->setCurrentIndex(0); break;
            case StyleConfigData::TE_SMALL: default: _viewTriangularExpanderSize->setCurrentIndex(1); break;
            case StyleConfigData::TE_NORMAL: _viewTriangularExpanderSize->setCurrentIndex(2); break;
        }

        _useWMMoveResize->setChecked( StyleConfigData::useWMMoveResize() );

    }

    //__________________________________________________________________
    void StyleConfig::windowDragModeChanged( int value )
    { _useWMMoveResize->setEnabled( value != 0 ); }

    //____________________________________________________________
    int StyleConfig::menuMode( void ) const
    {
        if (_menuHighlightDark->isChecked()) return StyleConfigData::MM_DARK;
        else if (_menuHighlightSubtle->isChecked()) return StyleConfigData::MM_SUBTLE;
        else return StyleConfigData::MM_STRONG;
    }

    //____________________________________________________________
    int StyleConfig::tabStyle( void ) const
    {
        if( _tabStylePlain->isChecked() ) return StyleConfigData::TS_PLAIN;
        else return StyleConfigData::TS_SINGLE;
    }

    //____________________________________________________________
    int StyleConfig::windowDragMode( void ) const
    {
        switch( _windowDragMode->currentIndex() )
        {
            case 1: return StyleConfigData::WD_MINIMAL;
            case 2: default: return StyleConfigData::WD_FULL;
        }
    }

    //____________________________________________________________
    int StyleConfig::triangularExpanderSize( void ) const
    {
        switch( _viewTriangularExpanderSize->currentIndex() )
        {
            case 0: return StyleConfigData::TE_TINY;
            case 1: default: return StyleConfigData::TE_SMALL;
            case 2: return StyleConfigData::TE_NORMAL;
        }

    }

}
