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

#include "field_painter.h"
#include "score_board_painter.h"
#include "ball_painter.h"
#include "player_painter.h"
#include "team_graphic_painter.h"
#include "draw_info_painter.h"

// model
#include "main_data.h"
#include "options.h"

#include <iostream>
#include <cmath>
#include <cassert>

/*-------------------------------------------------------------------*/
/*!

*/
FieldCanvas::FieldCanvas( MainData & main_data )
    :
#ifdef USE_GLWIDGET
    QGLWidget( QGLFormat( QGL::SampleBuffers ) ),
#endif
    M_main_data( main_data ),
    M_normal_menu( static_cast< QMenu * >( 0 ) ),
    M_system_menu( static_cast< QMenu * >( 0 ) ),
    M_monitor_menu( static_cast< QMenu * >( 0 ) ),
    M_measure_line_pen( QColor( 0, 255, 255 ), 0, Qt::SolidLine ),
    M_measure_mark_pen( QColor( 255, 0, 0 ), 0, Qt::SolidLine ),
    M_measure_font_pen( QColor( 255, 191, 191 ), 0, Qt::SolidLine ),
    M_measure_font_pen2( QColor( 224, 224, 192 ), 0, Qt::SolidLine ),
    M_measure_font( "6x13bold", 9 )
{
    this->setMouseTracking( true ); // need for the MouseMoveEvent
    this->setFocusPolicy( Qt::WheelFocus );

    readSettings();

    createPainters();
}

/*-------------------------------------------------------------------*/
/*!

*/
FieldCanvas::~FieldCanvas()
{
    writeSettings();
}

/*-------------------------------------------------------------------*/
/*!

*/
void
FieldCanvas::readSettings()
{
#ifndef Q_WS_WIN
    QSettings settings( QDir::homePath() + "/.rcsslogplayer",
                        QSettings::IniFormat );
#else
    QSettings settings( QDir::currentPath() + "/rcsslogplayer.ini",
                        QSettings::IniFormat );
#endif

    settings.beginGroup( "FieldCanvas" );

    QVariant val;

    val = settings.value( "measure_line_pen" );
    if ( val.isValid() ) M_measure_line_pen.setColor( val.toString() );

    val = settings.value( "measure_mark_pen" );
    if ( val.isValid() ) M_measure_mark_pen.setColor( val.toString() );

    val = settings.value( "measure_font_pen" );
    if ( val.isValid() ) M_measure_font_pen.setColor( val.toString() );

    val = settings.value( "measure_font_pen2" );
    if ( val.isValid() ) M_measure_font_pen2.setColor( val.toString() );

    val = settings.value( "measure_font" );
    if ( val.isValid() ) M_measure_font.fromString( val.toString() );

    settings.endGroup();
}

/*-------------------------------------------------------------------*/
/*!

*/
void
FieldCanvas::writeSettings()
{
#ifndef Q_WS_WIN
    QSettings settings( QDir::homePath() + "/.rcsslogplayer",
                        QSettings::IniFormat );
#else
    QSettings settings( QDir::currentPath() + "/rcsslogplayer.ini",
                        QSettings::IniFormat );
#endif

    settings.beginGroup( "FieldCanvas" );

    settings.setValue( "measure_line_pen", M_measure_line_pen.color().name() );
    settings.setValue( "measure_mark_pen", M_measure_mark_pen.color().name() );
    settings.setValue( "measure_font_pen", M_measure_font_pen.color().name() );
    settings.setValue( "measure_font_pen2", M_measure_font_pen2.color().name() );
    settings.setValue( "measure_font", M_measure_font.toString() );

    settings.endGroup();
}

/*-------------------------------------------------------------------*/
/*!

*/
void
FieldCanvas::createPainters()
{
    M_painters.clear();

    M_field_painter = boost::shared_ptr< FieldPainter >( new FieldPainter( M_main_data ) );

    M_painters.push_back( boost::shared_ptr< PainterInterface >
                          ( new PlayerPainter( M_main_data ) ) );
    M_painters.push_back( boost::shared_ptr< PainterInterface >
                          ( new BallPainter( M_main_data ) ) );
    M_painters.push_back( boost::shared_ptr< PainterInterface >
                          ( new DrawInfoPainter( M_main_data ) ) );
    M_painters.push_back( boost::shared_ptr< PainterInterface >
                          ( new TeamGraphicPainter( M_main_data ) ) );
    M_painters.push_back( boost::shared_ptr< PainterInterface >
                          ( new ScoreBoardPainter( M_main_data ) ) );
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

    if ( Options::instance().antiAliasing() )
    {
        painter.setRenderHint( QPainter::Antialiasing );
        // for QGLWidget
        //painter.setRenderHint( QPainter::HighQualityAntialiasing );
    }

    draw( painter );

    // draw mouse measure

    if ( M_mouse_state[2].isDragged() )
    {
        drawMouseMeasure( painter );
    }
}

/*-------------------------------------------------------------------*/
/*!

*/
void
FieldCanvas::updateFocus()
{
    DispConstPtr disp = M_main_data.getDispInfo( M_main_data.index() );

    // if auto select mode, update ball nearest player
    if ( disp
         && Options::instance().playerAutoSelect() )
    {
        const rcss::rcg::ShowInfoT & show = disp->show_;

        Options::PlayerSelectType old_type = Options::instance().playerSelectType();

        float min_dist2 = 40000.0f;

        rcss::rcg::Side side = rcss::rcg::NEUTRAL;
        int unum = 0;

        const int first = ( old_type == Options::SELECT_AUTO_RIGHT
                            ? 11
                            : 0 );
        const int last = ( old_type == Options::SELECT_AUTO_LEFT
                           ? 11
                           : 22 );
        for ( int i = first; i < last; ++i )
        {
            if ( show.player_[i].state_ != 0 )
            {
                float d2
                    = std::pow( show.ball_.x_ - show.player_[i].x_, 2 )
                    + std::pow( show.ball_.y_ - show.player_[i].y_, 2 );

                if ( d2 < min_dist2 )
                {
                    min_dist2 = d2;
                    side = show.player_[i].side();
                    unum = show.player_[i].unum_;
                }
            }
        }

        if ( unum != 0 )
        {
            Options::instance().setSelectedNumber( side, unum );
        }
    }

    // update focus point
    if ( disp )
    {
        if ( Options::instance().focusType() == Options::FOCUS_BALL )
        {
            Options::instance().setFocusPointReal( disp->show_.ball_.x_,
                                                   disp->show_.ball_.y_ );
        }
        else if ( Options::instance().focusType() == Options::FOCUS_PLAYER
                  && Options::instance().selectedNumber() != 0 )
        {
            int id = Options::instance().selectedNumber();
            if ( id < 0 )
            {
                id = -1*id + 11;
            }
            id -= 1;

            if ( disp->show_.player_[id].state_ != 0 )
            {
                Options::instance().setFocusPointReal( disp->show_.player_[id].x_,
                                                       disp->show_.player_[id].y_ );
            }
        }
        else
        {
            // already set
        }
    }

}

/*-------------------------------------------------------------------*/
/*!

*/
void
FieldCanvas::selectPlayer( const QPoint & point )
{
    DispConstPtr disp = M_main_data.getDispInfo( M_main_data.index() );

    if ( ! disp
         || ( Options::instance().playerSelectType() != Options::SELECT_FIX
              && Options::instance().playerSelectType() != Options::SELECT_UNSELECT )
         )
    {
        return;
    }

    double pos_x = Options::instance().fieldX( point.x() );
    double pos_y = Options::instance().fieldY( point.y() );

    // if auto select mode, toggle mode
    const rcss::rcg::ShowInfoT & show = disp->show_;

    double min_dist2 = 1.0 * 1.0;
    rcss::rcg::Side side = rcss::rcg::NEUTRAL;
    int unum = 0;

    for ( int i = 0; i < rcss::rcg::MAX_PLAYER*2; ++i )
    {
        if ( show.player_[i].state_ != 0 )
        {
            double d2
                = std::pow( pos_x - show.player_[i].x_, 2 )
                + std::pow( pos_y - show.player_[i].y_, 2 );

            if ( d2 < min_dist2 )
            {
                min_dist2 = d2;
                side = show.player_[i].side();
                unum = show.player_[i].unum_;
            }
        }
    }

    if ( unum != 0 )
    {
        if ( side == rcss::rcg::RIGHT ) unum += rcss::rcg::MAX_PLAYER;
        emit playerSelected( unum );
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
        else
        {
            selectPlayer( event->pos() );
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
            if ( Options::instance().monitorClientMode() )
            {
                if ( M_monitor_menu
                     && ! M_monitor_menu->exec( event->globalPos() ) )
                {
                    M_mouse_state[0].setMenuFailed( true );
                }
            }
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
            if ( Options::instance().monitorClientMode() )
            {
                if ( M_system_menu
                     && ! M_system_menu->exec( event->globalPos() ) )
                {
                    M_mouse_state[2].setMenuFailed( true );
                }
            }
            else
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
#if QT_VERSION >= 0x040200
        if ( this->cursor().shape() != Qt::ClosedHandCursor )
        {
            this->setCursor( QCursor( Qt::ClosedHandCursor ) );
        }
#else
        if ( this->cursor().shape() != Qt::SizeAllCursor )
        {
            this->setCursor( QCursor( Qt::SizeAllCursor ) );
        }
#endif

        int new_x = Options::instance().screenX( Options::instance().focusPoint().x() );
        int new_y = Options::instance().screenY( Options::instance().focusPoint().y() );
        new_x -= ( event->pos().x() - M_mouse_state[0].draggedPoint().x() );
        new_y -= ( event->pos().y() - M_mouse_state[0].draggedPoint().y() );
        emit focusChanged( QPoint( new_x, new_y ) );
    }

    for ( int i = 0; i < 3; ++i )
    {
        M_mouse_state[i].moved( event->pos() );
    }

    if ( M_mouse_state[2].isDragged() )
    {
        static QRect s_last_rect;

        if ( this->cursor().shape() != Qt::CrossCursor )
        {
            this->setCursor( QCursor( Qt::CrossCursor ) );
        }

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
    updateFocus();
    // update field scale and related things
    Options::instance().updateFieldSize( this->width(), this->height() );


    M_field_painter->draw( painter );

    if ( ! M_main_data.getDispInfo( M_main_data.index() ) )
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
}

/*-------------------------------------------------------------------*/
/*!

*/
void
FieldCanvas::drawMouseMeasure( QPainter & painter )
{
    const Options & opt = Options::instance();

    QPoint start_point = M_mouse_state[2].pressedPoint();
    QPoint end_point = M_mouse_state[2].draggedPoint();

    // draw straight line
    painter.setPen( M_measure_line_pen );
    painter.setBrush( Qt::NoBrush );
    painter.drawLine( start_point, end_point );

    // draw mark
    painter.setPen( M_measure_mark_pen );
    painter.drawEllipse( start_point.x() - 2,
                         start_point.y() - 2,
                         4,
                         4 );
    painter.drawEllipse( end_point.x() - 2,
                         end_point.y() - 2,
                         4,
                         4 );

    // draw distance & angle text
    painter.setFont( M_measure_font );
    painter.setPen( M_measure_font_pen );

    char buf[64];

    // draw start point value
    QPointF start_real( opt.fieldX( start_point.x() ),
                        opt.fieldY( start_point.y() ) );
    snprintf( buf, 64,
              "(%.2f,%.2f)",
              start_real.x(),
              start_real.y() );
    painter.drawText( start_point,
                      QString::fromAscii( buf ) );

    if ( std::abs( start_point.x() - end_point.x() ) < 1
         && std::abs( start_point.y() - end_point.y() ) < 1 )
    {
        return;
    }

    // draw end point value
    QPointF end_real( opt.fieldX( end_point.x() ),
                      opt.fieldY( end_point.y() ) );
    snprintf( buf, 64,
              "(%.2f,%.2f)",
              end_real.x(),
              end_real.y() );
    painter.drawText( end_point.x(),
                      end_point.y(),
                      QString::fromAscii( buf ) );

    // draw relative coordinate value
    painter.setPen( M_measure_font_pen2 );

    QPointF rel( end_real - start_real );
    double r = std::sqrt( std::pow( rel.x(), 2.0 ) + std::pow( rel.y(), 2.0 ) );
    double th = ( rel.x() == 0.0 && rel.y() == 0.0
                   ? 0.0
                   : std::atan2( rel.y(), rel.x() ) * 180.0 / M_PI );

    snprintf( buf, 64,
              "rel(%.2f,%.2f) r%.2f th%.1f",
              rel.x(), rel.y(), r, th );

    int dist_add_y = ( end_point.y() > start_point.y()
                       ? + painter.fontMetrics().height()
                       : - painter.fontMetrics().height() );
    painter.drawText( end_point.x(),
                      end_point.y() + dist_add_y,
                      QString::fromAscii( buf ) );
}
