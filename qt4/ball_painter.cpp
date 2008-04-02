// -*-c++-*-

/*!
  \file ball_painter.cpp
  \brief ball painter class Source File.
*/

/*
 *Copyright:

 Copyright (C)  The RoboCup Soccer Server Maintenance Group.
 Hidehisa AKIYAMA

 This code is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2, or (at your option)
 any later version.

 This code is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this code; see the file COPYING.  If not, write to
 the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.

 *EndCopyright:
 */

/////////////////////////////////////////////////////////////////////

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <QtGui>

#include "ball_painter.h"

#include "options.h"
#include "main_data.h"
#include "monitor_view_data.h"

#include <rcsc/common/server_param.h>
#include <rcsc/geom/vector_2d.h>

#include <vector>

/*-------------------------------------------------------------------*/
/*!

*/
BallPainter( const FieldCanvas & canvas;
             const MainData & main_data )
    : M_ball_pen( QColor( 255, 255, 255 ), 0, Qt::SolidLine )
    , M_ball_vel_pen( QColor( 255, 0, 0 ), 0, Qt::SolidLine )
    , M_ball_brush( QColor( 255, 255, 255 ), Qt::SolidPattern )
{
    readSettings();
}

/*-------------------------------------------------------------------*/
/*!

*/
BallPainter::~BallPainter()
{
    writeSettings();
}

/*-------------------------------------------------------------------*/
/*!

*/
void
BallPainter::readSettings()
{
    QSettings settings( QDir::homePath() + "/.rcsslogplayer",
                        QSettings::IniFormat );

    settings.beginGroup( "BallPainter" );

    QVariant val;

    val = settings.value( "ball_pen" );
    if ( val.isValid() ) M_ball_pen.setColor( val.toString() );

    val = settings.value( "ball_vel_pen" );
    if ( val.isValid() ) M_ball_vel_pen.setColor( val.toString() );

    val = settings.value( "ball_brush" );
    if ( val.isValid() ) M_ball_brush.setColor( val.toString() );

    settings.endGroup();
}

/*-------------------------------------------------------------------*/
/*!

*/
void
BallPainter::writeSettings()
{
    QSettings settings( QDir::homePath() + "/.rcsslogplayer",
                        QSettings::IniFormat );

    settings.beginGroup( "BallPainter" );

    settings.setValue( "ball_pen", M_ball_pen.color().name() );
    settings.setValue( "ball_vel_pen", M_ball_vel_pen.color().name() );
    settings.setValue( "ball_brush", M_ball_brush.color().name() );

    settings.endGroup();
}

/*-------------------------------------------------------------------*/
/*!

*/
void
BallPainter::draw( QPainter & painter )
{
    if ( ! Options::instance().showBall() )
    {
        return;
    }

    MonitorViewConstPtr view = M_main_data.getViewData( M_main_data.viewIndex() );

    if ( ! view )
    {
        return;
    }

    // decide radius
    const int ball_radius = std::max( 1, M_canvas.scale( M_main_data.serverParam().ball_size_ ) );
    const int kickable_radius
        = std::max( 1, M_canvas.scale( M_main_data.serverParam().player_size_
                                       + M_main_data.serverParam().kickable_margin_
                                       + M_main_data.serverParam().ball_size_ ) );
    const int ix = M_canvas.screenX( view->ball().x() );
    const int iy = M_canvas.screenY( view->ball().y() );

    // draw ball body
    painter.setPen( Qt::NoPen );
    painter.setBrush( M_ball_brush );
    painter.drawEllipse( ix - ball_radius,
                         iy - ball_radius,
                         ball_radius * 2,
                         ball_radius * 2 );

    // draw kickable margin
    painter.setPen( M_ball_pen );
    painter.setBrush( Qt::NoBrush );
    painter.drawEllipse( ix - kickable_radius,
                         iy - kickable_radius,
                         kickable_radius * 2,
                         kickable_radius * 2 );

    // draw future status
    if ( Options::instance().ballFutureCycle() > 0
         && view->ball().hasDelta() )
    {
        drawFutureState( painter );
    }
}

/*-------------------------------------------------------------------*/
/*!

*/
void
BallPainter::drawFutureState( QPainter & painter ) const
{
    if ( Options::instance().antiAliasing() )
    {
        painter.setRenderHint( QPainter::Antialiasing, false );
    }

    MonitorViewConstPtr view = M_main_data.getViewData( M_main_data.viewIndex() );

    const double bdecay = M_main_data.serverParam().ball_decay_;

    QPointF bpos( view->ball().x(),
                  view->ball().y() );
    QPointF bvel( view->ball().deltaX(),
                  view->ball().deltaY() );

    QPoint first_point( M_canvas.screenX( bpos.x ),
                        M_canvas.screenY( bpos.y ) );
    QPoint last_point = first_point;

    QPainterPath path;
    for ( int i = 0; i < 100; ++i )
    {
        bpos += bvel;
        bvel *= bdecay;

        QPoint pt( M_canvas.screenX( bpos.x ),
                   M_canvas.screenY( bpos.y ) );
        if ( std::abs( last_point.x() - pt.x() ) < 1
             && std::abs( last_point.y() - pt.y() ) < 1 )
        {
            break;
        }
        last_point = pt;
        path.addEllipse( pt.x() - 1,
                         pt.y() - 1,
                         2,
                         2 );
    }

    path.moveTo( first_point );
    path.lineTo( last_point );

    painter.setPen( M_ball_vel_pen );
    painter.setBrush( Qt::NoBrush );

    painter.drawPath( path );

    if ( Options::instance().antiAliasing() )
    {
        painter.setRenderHint( QPainter::Antialiasing );
    }
}
