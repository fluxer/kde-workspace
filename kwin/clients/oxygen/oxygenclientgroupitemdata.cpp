
//////////////////////////////////////////////////////////////////////////////
// oxygenclientgroupitemdata.cpp
// handles tabs' geometry and animations
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

#include "oxygenclientgroupitemdata.h"
#include "moc_oxygenclientgroupitemdata.cpp"
#include "oxygenclient.h"

namespace Oxygen
{

    //____________________________________________________________________________
    ClientGroupItemDataList::ClientGroupItemDataList( Client* parent ):
        QObject( parent ),
        QList<ClientGroupItemData>(),
        _client( *parent ),
        _dirty( false ),
        targetItem_( NoItem )
    {

    }

    //________________________________________________________________
    int ClientGroupItemDataList::itemAt( const QPoint& point, bool between ) const
    {

        for( int i=0; i < count(); i++ )
        {
            QRect rect = at(i)._activeRect;
            if( between ) rect.translate( -rect.width() / 2, 0 );
            if( rect.adjusted(0,0,0,2).contains( point ) )
            { return i; }
        }

        return NoItem;

    }

    //____________________________________________________________________________
    void ClientGroupItemDataList::updateButtonActivity( long visibleItem ) const
    {

        for( int index = 0; index < count(); index++ )
        {

            const ClientGroupItemData& item( at(index) );
            if( item._closeButton )
            { item._closeButton.data()->setForceInactive( _client.tabId(index) != visibleItem ); }

        }

    }

    //____________________________________________________________________________
    void ClientGroupItemDataList::updateButtons( bool alsoUpdate ) const
    {

        // move close buttons
        if( alsoUpdate ) _client.widget()->setUpdatesEnabled( false );
        for( int index = 0; index < count(); index++ )
        {

            const ClientGroupItemData& item( at(index) );
            if( !item._closeButton ) continue;

            if( (!item._boundingRect.isValid()) || count()<=2 )
            {

                item._closeButton.data()->hide();

            } else {

                QPoint position(
                    item._boundingRect.right() - _client.buttonSize() - _client.layoutMetric(KCommonDecoration::LM_TitleEdgeRight),
                    item._boundingRect.top() + _client.layoutMetric( KCommonDecoration::LM_TitleEdgeTop ) );

                if( item._closeButton.data()->isHidden() ) item._closeButton.data()->show();
                item._closeButton.data()->move( position );

            }

        }

        if( alsoUpdate )
        {
            _client.widget()->setUpdatesEnabled( true );
            _client.updateTitleRect();
        }

    }

    //____________________________________________________________________________
    void ClientGroupItemDataList::updateBoundingRects( bool alsoUpdate )
    {

        for( iterator iter = begin(); iter != end(); ++iter )
        {

            // left
            if( iter->_endBoundingRect.left() == iter->_startBoundingRect.left() )
            {

                iter->_boundingRect.setLeft( iter->_startBoundingRect.left() );

            } else {

                iter->_boundingRect.setLeft( (1.0)*iter->_startBoundingRect.left() *iter->_endBoundingRect.left() );

            }

            // right
            if( iter->_endBoundingRect.right() == iter->_startBoundingRect.right() )
            {

                iter->_boundingRect.setRight( iter->_startBoundingRect.right() );

            } else {

                iter->_boundingRect.setRight( (1.0)*iter->_startBoundingRect.right() *iter->_endBoundingRect.right() );

            }

        }

        // update button position
        updateButtons( alsoUpdate );

    }
}
