#ifndef oxygen_shadowCacheh
#define oxygen_shadowCacheh

//////////////////////////////////////////////////////////////////////////////
// oxygenshadowcache.h
// handles caching of TileSet objects to draw shadows
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

#include "oxygenhelper.h"
#include "oxygen_export.h"

#include <QtCore/QCache>
#include <QtGui/QRadialGradient>
#include <cmath>

namespace Oxygen
{

    class OXYGEN_EXPORT ShadowCache
    {
        public:

        //! constructor
        explicit ShadowCache( Helper& );

        //! destructor
        virtual ~ShadowCache( void )
        {}

        //! read configuration
        void readConfig( void );

        //! cache size
        void setEnabled( bool enabled )
        {
            _enabled = enabled;
            if( enabled )
            {

                _shadowCache.setMaxCost( 1<<6 );

            } else {

                _shadowCache.setMaxCost( 1 );

            }
        }

        //! invalidate caches
        void invalidateCaches( void )
        {
            _shadowCache.clear();
        }

        //! true if shadow is enabled for a given group
        bool isEnabled( QPalette::ColorGroup ) const;

        //! set shadow size manually
        void setShadowSize( QPalette::ColorGroup, int );

        //! shadow size
        int shadowSize( void ) const;

        //! Key class to be used into QCache
        /*! class is entirely inline for optimization */
        class Key
        {

            public:

            //! explicit constructor
            explicit Key( void ):
                index(0),
                active(false),
                isShade(false),
                hasBorder( true )
            {}

            //! constructor from int
            explicit Key( int hash ):
                index( hash >> 3 ),
                active( ( hash >> 2 )&1 ),
                isShade( ( hash >> 1)&1 ),
                hasBorder( (hash)&1 )
            {}

            //! hash function
            int hash( void ) const
            {

                return
                    ( index << 3 ) |
                    ( active << 2 ) |
                    ( isShade<< 1 ) |
                    ( hasBorder );

            }

            int index;
            bool active;
            bool isShade;
            bool hasBorder;

        };

        //! get shadow matching client
        TileSet* tileSet( const Key& );

        //! simple pixmap
        QPixmap pixmap( const Key& key ) const
        { return pixmap( key, key.active ); }

        //! simple pixmap
        QPixmap pixmap( const Key&, bool active ) const;

        protected:

        Helper& helper( void ) const
        { return _helper; }

        //! square utility function
        static qreal square( qreal x )
        { return x*x; }

        //! functions used to draw shadows
        class Parabolic
        {
            public:

            //! constructor
            Parabolic( qreal amplitude, qreal width ):
                amplitude_( amplitude ),
                width_( width )
            {}

            //! destructor
            virtual ~Parabolic( void )
            {}

            //! value
            virtual qreal operator() ( qreal x ) const
            { return qMax( 0.0, amplitude_*(1.0 - square(x/width_) ) ); }

            private:

            qreal amplitude_;
            qreal width_;

        };

        //! functions used to draw shadows
        class Gaussian
        {
            public:

            //! constructor
            Gaussian( qreal amplitude, qreal width ):
                amplitude_( amplitude ),
                width_( width )
            {}

            //! destructor
            virtual ~Gaussian( void )
            {}

            //! value
            virtual qreal operator() ( qreal x ) const
            { return qMax( 0.0, amplitude_*(std::exp( -square(x/width_) -0.05 ) ) ); }

            private:

            qreal amplitude_;
            qreal width_;

        };

        //! draw gradient into rect
        /*! a separate method is used in order to properly account for corners */
        void renderGradient( QPainter&, const QRectF&, const QRadialGradient&, bool hasBorder = true ) const;

        private:

        //! helper
        Helper& _helper;

        //! defines overlap between shadows and body
        enum { overlap = 4 };

        //! caching enable state
        bool _enabled;

        //! cache
        typedef QCache<int, TileSet> TileSetCache;

        //! shadow cache
        TileSetCache _shadowCache;

    };

}

#endif
