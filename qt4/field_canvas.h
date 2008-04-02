// -*-c++-*-

/*!
	\file field_canvas.h
	\brief field canvas class Header File.
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
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this code; see the file COPYING.	If not, write to
 the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.

 *EndCopyright:
 */

/////////////////////////////////////////////////////////////////////

#ifndef RCSSLOGPLAYER_FIELD_CANVAS_H
#define RCSSLOGPLAYER_FIELD_CANVAS_H

#include <QWidget>
#include <QPoint>
#include <QPointF>
#include <QFont>

#include "mouse_state.h"

#include <boost/shared_ptr.hpp>

#include <vector>
#include <cmath>

class QContextMenuEvent;
class QMenu;
class QMouseEvent;
class QPaintEvent;

class MainData;
class PainterInterface;
class FieldPainter;

//! main soccer field canvas class
class FieldCanvas
		: public QWidget {

    Q_OBJECT

private:

    MainData & M_main_data;

    QMenu * M_normal_menu;
    QMenu * M_system_menu;
    QMenu * M_monitor_menu;

    boost::shared_ptr< FieldPainter > M_field_painter;
    std::vector< boost::shared_ptr< PainterInterface > > M_painters;

    //! 0: left, 1: middle, 2: right
    MouseState M_mouse_state[3];
    QPen M_measure_pen;


    double M_field_scale; //!< scaling factor
    bool M_zoomed; //!< zoom flag
    QPoint M_field_center; //!< field center point on the screen
    QPointF M_focus_point; //!< the current focus point

    // not used
    FieldCanvas( const FieldCanvas & );
    const FieldCanvas & operator=( const FieldCanvas & );

public:

    explicit
    FieldCanvas( MainData & main_data );

    ~FieldCanvas();

    void createPainters();

    void setNormalMenu( QMenu * menu );
    void setSystemMenu( QMenu * menu );
    void setMonitorMenu( QMenu * menu );

    void draw( QPainter & painter );

    const
    QPoint & fieldCenter() const
      {
          return M_field_center;
      }

    const
    QPointF & focusPoint() const
      {
          return M_focus_point;
      }

    int scale( const double & len ) const
      {
          return static_cast< int >( ::rint( len * M_field_scale ) );
      }

    int screenX( const double & x ) const
      {
          return M_field_center.x() + scale( x );
      }

    int screenY( const double & y ) const
      {
          return M_field_center.y() + scale( y );
      }

    double fieldX( const int x ) const
      {
          return ( x - M_field_center.x() ) / M_field_scale;
      }

    double fieldY( const int y ) const
      {
          return ( y - M_field_center.y() ) / M_field_scale;
      }

private:

    void drawMouseMeasure( QPainter & painter );

    void updateScale();

protected:

    void mouseDoubleClickEvent( QMouseEvent * event );
    void mousePressEvent( QMouseEvent * event );
    void mouseReleaseEvent( QMouseEvent * event );
    void mouseMoveEvent( QMouseEvent * event );

    void paintEvent( QPaintEvent * );

public slots:

    void setRedrawAllFlag();
    void redrawAll();

    void dropBall();
    void freeKickLeft();
    void freeKickRight();

signals:

    void dropBall( const QPoint & pos );
    void freeKickLeft( const QPoint & pos );
    void freeKickRight( const QPoint & pos );

    void playerMoved( const QPoint & point );

    void focusChanged( const QPoint & point );
    void mouseMoved( const QPoint & point );

};

#endif
