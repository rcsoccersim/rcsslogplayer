// -*-c++-*-

/*!
  \file field_canvas.cpp
  \brief main field canvas class Source File.
*/

/*
 *Copyright:

 Copyright (C) The RoboCup Soccer Server Maintenance Group.
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

#include "field_canvas.h"

//#include "draw_config.h"
#include "field_painter.h"
#include "score_board_painter.h"
#include "ball_painter.h"
#include "player_painter.h"
//#include "team_graphic_painter.h"

// model
#include "main_data.h"
#include "app_config.h"

#include <rcsc/common/server_param.h>

#include <iostream>
#include <cmath>
#include <cassert>

namespace {
int cursor_timeout = 5000;
}

/*-------------------------------------------------------------------*/
/*!

*/
FieldCanvas::FieldCanvas( MainData & main_data )
    : M_main_data( main_data )
    , M_field_scale( 1.0 )
    , M_zoomed( false )
    , M_field_center( 0, 0 )
    , M_focus_point( 0.0, 0.0 )
//     , M_redraw_all( true )
//     , M_normal_menu( static_cast< QMenu * >( 0 ) )
//     , M_system_menu( static_cast< QMenu * >( 0 ) )
//     , M_monitor_menu( static_cast< QMenu * >( 0 ) )
{
    //this->setPalette( M_background_color );
    //this->setAutoFillBackground( true );

    // need for the MouseMoveEvent
    this->setMouseTracking( true );

    this->setFocusPolicy( Qt::WheelFocus );

    // paint directory
    //this->setAttribute( Qt::WA_PaintOnScreen );

    createPainters();
}

/*-------------------------------------------------------------------*/
/*!

*/
FieldCanvas::~FieldCanvas()
{

}

/*-------------------------------------------------------------------*/
/*!

*/
void
FieldCanvas::createPainters()
{
    M_painters.clear();

    M_field_painter
        = boost::shared_ptr< FieldPainter >( new FieldPainter( *this ) );

    //M_painters.push_back( boost::shared_ptr< PainterInterface >
    //                      ( new TeamGraphicPainter( *this, M_main_data ) ) );
    M_painters.push_back( boost::shared_ptr< PainterInterface >
                          ( new PlayerPainter( *this, M_main_data ) ) );
    M_painters.push_back( boost::shared_ptr< PainterInterface >
                          ( new BallPainter( *this, M_main_data ) ) );
    M_painters.push_back( boost::shared_ptr< PainterInterface >
                          ( new ScoreBoardPainter( *this, M_main_data ) ) );
}

/*-------------------------------------------------------------------*/
/*!

*/
void
FieldCanvas::setNormalMenu( QMenu * menu )
{
    if ( M_normal_menu )
    {
        delete M_normal_menu;
        M_normal_menu = static_cast< QMenu * >( 0 );
    }

    M_normal_menu = menu;
}

/*-------------------------------------------------------------------*/
/*!

*/
void
FieldCanvas::setSystemMenu( QMenu * menu )
{
    if ( M_system_menu )
    {
        delete M_system_menu;
        M_system_menu = static_cast< QMenu * >( 0 );
    }

    M_system_menu = menu;
}

/*-------------------------------------------------------------------*/
/*!

*/
void
FieldCanvas::setMonitorMenu( QMenu * menu )
{
    if ( M_monitor_menu )
    {
        delete M_monitor_menu;
        M_monitor_menu = static_cast< QMenu * >( 0 );
    }

    M_monitor_menu = menu;
}

/*-------------------------------------------------------------------*/
/*!

*/
void
FieldCanvas::setRedrawAllFlag()
{
    M_redraw_all = true;
}

/*-------------------------------------------------------------------*/
/*!

*/
void
FieldCanvas::redrawAll()
{
    M_redraw_all = true;
    this->update();
}

/*-------------------------------------------------------------------*/
/*!

*/
void
FieldCanvas::dropBall()
{
    emit dropBall( M_mouse_state[0].pressedPoint() );
}

/*-------------------------------------------------------------------*/
/*!

*/
void
FieldCanvas::freeKickLeft()
{
    emit freeKickLeft( M_mouse_state[0].pressedPoint() );
}

/*-------------------------------------------------------------------*/
/*!

*/
void
FieldCanvas::freeKickRight()
{
    emit freeKickRight( M_mouse_state[0].pressedPoint() );
}

/*-------------------------------------------------------------------*/
/*!

*/
void
FieldCanvas::paintEvent( QPaintEvent * )
{
    QPainter painter( this );
    if ( M_main_data.viewConfig().antiAliasing() )
    {
        painter.setRenderHint( QPainter::Antialiasing );
        // for QGLWidget
        //painter.setRenderHint( QPainter::HighQualityAntialiasing );
    }

    M_main_data.update( this->width(),
                        this->height() );

    draw( painter );

    // draw mouse measure

    if ( M_mouse_state[2].isDragged() )
    {
        drawMouseMeasure( painter );
    }

    if ( ! M_player_dragged_point.isNull() )
    {
        drawDraggedPlayer( painter );
    }
}

/*-------------------------------------------------------------------*/
/*!

*/
void
FieldCanvas::mouseDoubleClickEvent( QMouseEvent * event )
{
    this->unsetCursor();

    if ( event->button() == Qt::LeftButton )
    {
        emit focusChanged( event->pos() );
    }

    if ( M_main_data.viewConfig().cursorHide() )
    {
        M_cursor_timer->start( cursor_timeout );
    }
}

/*-------------------------------------------------------------------*/
/*!

*/
void
FieldCanvas::mousePressEvent( QMouseEvent * event )
{
    if ( event->button() == Qt::LeftButton )
    {
        M_mouse_state[0].pressed( event->pos() );
        if ( M_mouse_state[1].isMenuFailed() )
        {
            M_mouse_state[1].setMenuFailed( false );
        }

        if ( event->modifiers() == Qt::ControlModifier )
        {
            emit focusChanged( event->pos() );
        }
    }
    else if ( event->button() == Qt::MidButton )
    {
        M_mouse_state[1].pressed( event->pos() );
        if ( M_mouse_state[1].isMenuFailed() )
        {
            M_mouse_state[1].setMenuFailed( false );
        }
    }
    else if ( event->button() == Qt::RightButton )
    {
        M_mouse_state[2].pressed( event->pos() );
        if ( M_mouse_state[2].isMenuFailed() )
        {
            M_mouse_state[2].setMenuFailed( false );
        }
    }
}

/*-------------------------------------------------------------------*/
/*!

*/
void
FieldCanvas::mouseReleaseEvent( QMouseEvent * event )
{
    this->unsetCursor();

    if ( event->button() == Qt::LeftButton )
    {
        M_mouse_state[0].released();

        if ( M_mouse_state[0].isMenuFailed() )
        {
            M_mouse_state[0].setMenuFailed( false );
        }
        else if ( M_mouse_state[0].pressedPoint() == event->pos() )
        {
//             if ( AppConfig::instance().monitorClientMode() )
//             {
//                 if ( M_monitor_menu
//                      && ! M_monitor_menu->exec( event->globalPos() ) )
//                 {
//                     M_mouse_state[0].setMenuFailed( true );
//                 }
//             }
        }
        else
        {

        }
    }
    else if ( event->button() == Qt::MidButton )
    {
        M_mouse_state[1].released();
    }
    else if ( event->button() == Qt::RightButton )
    {
        M_mouse_state[2].released();

        if ( M_mouse_state[2].isMenuFailed() )
        {
            M_mouse_state[2].setMenuFailed( false );
        }
        else if ( M_mouse_state[2].pressedPoint() == event->pos() )
        {
//             if ( AppConfig::instance().monitorClientMode() )
//             {
//                 if ( M_system_menu
//                      && ! M_system_menu->exec( event->globalPos() ) )
//                 {
//                     M_mouse_state[2].setMenuFailed( true );
//                 }
//             }
//             else
            {
                if ( M_normal_menu
                     && ! M_normal_menu->exec( event->globalPos() ) )
                {
                    M_mouse_state[2].setMenuFailed( true );
                }
            }
        }
    }
}

/*-------------------------------------------------------------------*/
/*!

*/
void
FieldCanvas::mouseMoveEvent( QMouseEvent * event )
{
    if ( this->cursor().shape() == Qt::BlankCursor )
    {
        this->unsetCursor();
    }

    if ( M_mouse_state[0].isDragged() )
    {
        if ( M_main_data.trainerData().isPlayerDragged() )
        {
            dragPlayer( event->pos() );
        }
        else
        {
            if ( this->cursor().shape() != Qt::ClosedHandCursor )
            {
                this->setCursor( QCursor( Qt::ClosedHandCursor ) );
            }

            int new_x = M_main_data.viewConfig().absScreenX( M_main_data.viewConfig().focusPoint().x );
            int new_y = M_main_data.viewConfig().absScreenY( M_main_data.viewConfig().focusPoint().y );
            new_x -= ( event->pos().x() - M_mouse_state[0].draggedPoint().x() );
            new_y -= ( event->pos().y() - M_mouse_state[0].draggedPoint().y() );
            emit focusChanged( QPoint( new_x, new_y ) );
        }
    }

    for ( int i = 0; i < 3; ++i )
    {
        M_mouse_state[i].moved( event->pos() );
    }

    if ( M_mouse_state[2].isDragged() )
    {
        static QRect s_last_rect;
        QRect new_rect
            = QRect( M_mouse_state[2].pressedPoint(),
                     M_mouse_state[2].draggedPoint() ).normalized();
        new_rect.adjust( -32, -32, 32, 32 );
        if ( new_rect.right() < M_mouse_state[2].draggedPoint().x() + 256 )
        {
            new_rect.setRight( M_mouse_state[2].draggedPoint().x() + 256 );
        }
        // draw mouse measure
        this->update( s_last_rect.unite( new_rect ) );
        s_last_rect = new_rect;
    }

    emit mouseMoved( event->pos() );
}

/*-------------------------------------------------------------------*/
/*!

*/
void
FieldCanvas::draw( QPainter & painter )
{
    M_field_painter->setRedraw( M_redraw_all );
    M_field_painter->draw( painter );

    if ( ! M_main_data.getViewData( M_main_data.viewIndex() ) )
    {
        return;
    }

    for ( std::vector< boost::shared_ptr< PainterInterface > >::iterator
              it = M_painters.begin();
          it != M_painters.end();
          ++it )
    {
        (*it)->draw( painter );
    }

    M_redraw_all = false;
}

/*-------------------------------------------------------------------*/
/*!

*/
void
FieldCanvas::drawMouseMeasure( QPainter & painter )
{
    const ViewConfig & vconf = M_main_data.viewConfig();

    QPoint start_point = M_mouse_state[2].pressedPoint();
    QPoint end_point = M_mouse_state[2].draggedPoint();

    // draw straight line
    painter.setPen( DrawConfig::instance().measurePen() );
    painter.setBrush( DrawConfig::instance().transparentBrush() );
    painter.drawLine( start_point, end_point );

    QPainterPath mark_path;
    mark_path.addEllipse( start_point.x() - 2,
                          start_point.y() - 2,
                          4,
                          4 );
    mark_path.addEllipse( end_point.x() - 2,
                          end_point.y() - 2,
                          4,
                          4 );

    rcsc::Vector2D start_real( vconf.fieldX( start_point.x() ),
                               vconf.fieldY( start_point.y() ) );
    rcsc::Vector2D end_real( vconf.fieldX( end_point.x() ),
                             vconf.fieldY( end_point.y() ) );

    // ball travel marks
    {
        double ball_speed = rcsc::ServerParam::i().ballSpeedMax();

        if ( ! M_measure_ball_speed_point.isNull()
             && M_measure_ball_speed_point != start_point )
        {
            rcsc::Vector2D vel( std::abs( M_measure_ball_speed_point.x()
                                          - start_point.x() )
                                / vconf.fieldScale(),
                                std::abs( M_measure_ball_speed_point.y()
                                          - start_point.y() )
                                / vconf.fieldScale() );
            ball_speed = vel.r();
            if ( ball_speed > rcsc::ServerParam::i().ballSpeedMax() )
            {
                ball_speed = rcsc::ServerParam::i().ballSpeedMax();
            }
        }

        rcsc::Vector2D ball_pos = start_real;
        rcsc::Vector2D ball_vel = end_real - start_real;
        ball_vel.setLength( ball_speed );

        const double max_length = start_real.dist( end_real );
        double total_travel = 0.0;

        ball_pos += ball_vel;
        ball_vel *= rcsc::ServerParam::i().ballDecay();
        total_travel += ball_vel.r();
        QPoint last_pt( start_point.x() - 2,
                        start_point.y() - 2 );
        while ( total_travel < max_length )
        {
            QPoint pt( vconf.absScreenX( ball_pos.x ) - 2,
                       vconf.absScreenY( ball_pos.y ) - 2 );
            if ( std::abs( pt.x() - last_pt.x() ) < 1
                 && std::abs( pt.y() - last_pt.y() ) < 1 )
            {
                break;
            }
            last_pt = pt;
            mark_path.addEllipse( pt.x(),
                                  pt.y(),
                                  4,
                                  4 );
            ball_pos += ball_vel;
            ball_vel *= rcsc::ServerParam::i().ballDecay();
            ball_speed *= rcsc::ServerParam::i().ballDecay();
            total_travel += ball_speed;
        }
    }

    painter.setPen( Qt::red );
    painter.drawPath( mark_path );

    // draw distance & angle text
    painter.setFont( DrawConfig::instance().measureFont() );
    painter.setPen( DrawConfig::instance().measureFontPen() );
    //painter.setBackgroundMode( Qt::TransparentMode );

    char buf[64];
    std::snprintf( buf, 64,
                   "(%.2f,%.2f)",
                   start_real.x,
                   start_real.y );

    painter.drawText( start_point,
                      QString::fromAscii( buf ) );

    if ( std::abs( start_point.x() - end_point.x() ) < 1
         && std::abs( start_point.y() - end_point.y() ) < 1 )
    {
        return;
    }

    std::snprintf( buf, 64,
                   "(%.2f,%.2f)",
                   end_real.x,
                   end_real.y );

    painter.drawText( end_point.x(),
                      end_point.y(),
                      QString::fromAscii( buf ) );

    painter.setPen( QColor( 224, 224, 192 ) );
    rcsc::Vector2D rel( end_real - start_real );
    std::snprintf( buf, 64,
                   "rel(%.2f,%.2f) r%.2f th%.1f",
                   rel.x, rel.y, rel.r(), rel.th().degree() );

    int dist_add_y = ( end_point.y() > start_point.y()
                       ? + painter.fontMetrics().height()
                       : - painter.fontMetrics().height() );
    painter.drawText( end_point.x(),
                      end_point.y() + dist_add_y,
                      QString::fromAscii( buf ) );
}
