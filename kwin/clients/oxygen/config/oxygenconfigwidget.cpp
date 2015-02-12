//////////////////////////////////////////////////////////////////////////////
// oxygenconfigurationui.cpp
// -------------------
//
// Copyright (c) 2009 Hugo Pereira Da Costa <hugo.pereira@free.fr>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
//////////////////////////////////////////////////////////////////////////////

#include "oxygenconfigwidget.h"

#include <kdeversion.h>

#include <QLabel>
#include <QLayout>
#include <QGroupBox>
#include <KLocalizedString>
#include <KTabWidget>

namespace Oxygen
{

    //_________________________________________________________
    ConfigWidget::ConfigWidget( QWidget* parent ):
        QWidget( parent ),
        _changed( false )
    {

        ui.setupUi( this );

        // shadow configuration
        ui.activeShadowConfiguration->setGroup( QPalette::Active );
        ui.inactiveShadowConfiguration->setGroup( QPalette::Inactive );

        shadowConfigurations.append( ui.activeShadowConfiguration );
        shadowConfigurations.append( ui.inactiveShadowConfiguration );

        // track ui changes
        connect( ui.titleAlignment, SIGNAL(currentIndexChanged(int)), SLOT(updateChanged()) );
        connect( ui.buttonSize, SIGNAL(currentIndexChanged(int)), SLOT(updateChanged()) );
        connect( ui.frameBorder, SIGNAL(currentIndexChanged(int)), SLOT(updateChanged()) );

        connect( ui.titleOutline, SIGNAL(clicked()), SLOT(updateChanged()) );
        connect( ui.drawSizeGrip, SIGNAL(clicked()), SLOT(updateChanged()) );
        connect( ui.narrowButtonSpacing, SIGNAL(clicked()), SLOT(updateChanged()) );
        connect( ui.closeFromMenuButton, SIGNAL(clicked()), SLOT(updateChanged()) );
        connect( ui.separatorMode, SIGNAL(currentIndexChanged(int)), SLOT(updateChanged()) );

        // track exception changes
        connect( ui.exceptions, SIGNAL(changed(bool)), SLOT(updateChanged()) );

        // track shadow configuration changes
        connect( shadowConfigurations[0], SIGNAL(changed(bool)), SLOT(updateChanged()) );
        connect( shadowConfigurations[1], SIGNAL(changed(bool)), SLOT(updateChanged()) );

    }

    //_________________________________________________________
    void ConfigWidget::setConfiguration( ConfigurationPtr configuration )
    {
        _configuration = configuration;
    }

    //_________________________________________________________
    void ConfigWidget::load( void )
    {
        if( !_configuration ) return;
        ui.titleAlignment->setCurrentIndex( _configuration->titleAlignment() );
        ui.buttonSize->setCurrentIndex( _configuration->buttonSize() );
        ui.frameBorder->setCurrentIndex( _configuration->frameBorder() );
        ui.separatorMode->setCurrentIndex( _configuration->separatorMode() );
        ui.drawSizeGrip->setChecked( _configuration->drawSizeGrip() );
        ui.titleOutline->setChecked( _configuration->drawTitleOutline() );
        ui.narrowButtonSpacing->setChecked( _configuration->useNarrowButtonSpacing() );
        ui.closeFromMenuButton->setChecked( _configuration->closeWindowFromMenuButton() );
        setChanged( false );
    }

    //_________________________________________________________
    void ConfigWidget::save( void )
    {

        if( !_configuration ) return;

        // apply modifications from ui
        _configuration->setTitleAlignment( ui.titleAlignment->currentIndex() );
        _configuration->setButtonSize( ui.buttonSize->currentIndex() );
        _configuration->setFrameBorder( ui.frameBorder->currentIndex() );
        _configuration->setSeparatorMode( ui.separatorMode->currentIndex() );
        _configuration->setDrawSizeGrip( ui.drawSizeGrip->isChecked() );
        _configuration->setDrawTitleOutline( ui.titleOutline->isChecked() );
        _configuration->setUseNarrowButtonSpacing( ui.narrowButtonSpacing->isChecked() );
        _configuration->setCloseWindowFromMenuButton( ui.closeFromMenuButton->isChecked() );

        setChanged( false );

    }

    //__________________________________________________________________
    bool ConfigWidget::eventFilter( QObject* object, QEvent* event )
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
    void ConfigWidget::updateLayout( void )
    {

        window()->setMinimumSize( QSize( window()->minimumSizeHint().width(), window()->size().height() ) );

    }


    //_______________________________________________
    void ConfigWidget::updateChanged( void )
    {

        // check configuration
        if( !_configuration ) return;

        // track modifications
        bool modified( false );

        if( ui.titleAlignment->currentIndex() != _configuration->titleAlignment() ) modified = true;
        else if( ui.buttonSize->currentIndex() != _configuration->buttonSize() ) modified = true;
        else if( ui.frameBorder->currentIndex() != _configuration->frameBorder() ) modified = true;
        else if( ui.separatorMode->currentIndex() != _configuration->separatorMode() ) modified = true;
        else if( ui.drawSizeGrip->isChecked() != _configuration->drawSizeGrip() ) modified = true;
        else if( ui.titleOutline->isChecked() !=  _configuration->drawTitleOutline() ) modified = true;
        else if( ui.narrowButtonSpacing->isChecked() !=  _configuration->useNarrowButtonSpacing() ) modified = true;
        else if( ui.closeFromMenuButton->isChecked() != _configuration->closeWindowFromMenuButton() ) modified = true;

        // exceptions
        else if( ui.exceptions->isChanged() ) modified = true;

        // shadow configurations
        else if( shadowConfigurations[0]->isChanged() ) modified = true;
        else if( shadowConfigurations[1]->isChanged() ) modified = true;

        setChanged( modified );

    }
}
