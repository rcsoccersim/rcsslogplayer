// -*-c++-*-

/*!
  \file field_painter.cpp
  \brief field painter class Source File.
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

#include "field_painter.h"

#include "field_canvas.h"
#include "main_data.h"

#include <iostream>

const double FieldPainter::PITCH_LENGTH = 105.0;
const double FieldPainter::PITCH_WIDTH = 68.0;
const double FieldPainter::PITCH_MARGIN = 5.0;
const double FieldPainter::CENTER_CIRCLE_R = 9.15;
const double FieldPainter::PENALTY_AREA_LENGTH = 16.5;
const double FieldPainter::PENALTY_AREA_WIDTH = 40.32;
const double FieldPainter::PENALTY_CIRCLE_R = 9.15;
const double FieldPainter::PENALTY_SPOT_DIST = 11.0;
const double FieldPainter::GOAL_AREA_LENGTH = 5.5;
const double FieldPainter::GOAL_AREA_WIDTH = 18.32;
const double FieldPainter::GOAL_DEPTH = 2.44;
const double FieldPainter::CORNER_ARC_R = 1.0;
const double FieldPainter::GOAL_POST_RADIUS = 0.06;

/*-------------------------------------------------------------------*/
/*!

 */
FieldPainter::FieldPainter( const FieldCanfas & canvas,
                            const MainData & main_data )
    : M_canvas( canvas )
    , M_main_data( main_data )
    , M_field_brush( QColor( 31, 160, 31 ), Qt::SolidPattern )
    , M_line_pen( QColor( 255, 255, 255 ),1, Qt::SolidLine )
{
    readSettings();
}

/*-------------------------------------------------------------------*/
/*!

 */
FieldPainter::~FieldPainter()
{
    saveSettings();
}

/*-------------------------------------------------------------------*/
/*!

 */
void
FieldPainter::readSettings()
{
    QSettings settings( QDir::homePath() + "/.rcsslogplayer",
                        QSettings::IniFormat );

    settings.beginGroup( "FieldPainter" );

    QVariant val;

    val = settings.value( "field_brush" );
    if ( val.isValid() ) M_field_brush.setColor( val.toString() );

    val = settings.value( "line_pen" );
    if ( val.isValid() ) M_line_pen.setColor( val.toString() );

    settings.endGroup();
}

/*-------------------------------------------------------------------*/
/*!

 */
void
FieldPainter::writeSettings()
{
    QSettings settings( QDir::homePath() + "/.rcsslogplayer",
                        QSettings::IniFormat );

    settings.beginGroup( "FieldPainter" );

    settings.setValue( "field_brush", M_field_brush.color().name() );
    settings.setValue( "line_pen", M_line_pen.color().name() );

    settings.endGroup();
}

/*-------------------------------------------------------------------*/
/*!

 */
void
FieldPainter::draw( QPainter & painter )
{
    if ( Options::instance().antiAliasing() )
    {
        painter.setRenderHint( QPainter::Antialiasing, false );
    }

    drawBackGround( painter );
    drawLines( painter );
    drawPenaltyAreaLines( painter );
    drawGoalAreaLines( painter );
    drawGoals( painter );

    if ( Options::instance().antiAliasing() )
    {
        painter.setRenderHint( QPainter::Antialiasing );
    }
}

/*-------------------------------------------------------------------*/
/*!

 */
void
FieldPainter::drawBackGround( QPainter & painter ) const
{
    // fill the whole region
    painter.fillRect( painter.window(),
                      M_field_brush );
}

/*-------------------------------------------------------------------*/
/*!

 */
void
FieldPainter::drawLines( QPainter & painter ) const
{
    // set paint styles
    painter.setPen( M_line_pen );
    painter.setBrush( Qt::NoBrush );

    // set screen coordinates of field
    int left_x   = M_canvas.screenX( - PITCH_LENGTH*0.5 );
    int right_x  = M_canvas.screenX( + PITCH_LENGTH*0.5 );
    int top_y    = M_canvas.screenY( - PITCH_WIDTH*0.5 );
    int bottom_y = M_canvas.screenY( + PITCH_WIDTH*0.5 );

    // side lines & goal lines
    painter.drawLine( left_x, top_y, right_x, top_y );
    painter.drawLine( right_x, top_y, right_x, bottom_y );
    painter.drawLine( right_x, bottom_y, left_x, bottom_y );
    painter.drawLine( left_x, bottom_y, left_x, top_y );

    if ( M_main_data.serverParam().keepaway_mode_ )
    {
        // keepaway area
        int ka_left = M_canvas.screenX( - M_main_data.serverParam().keepaway_length_ * 0.5 );
        int ka_top = M_canvas.screenY( - M_main_data.serverParam().keepaway_width_ * 0.5 );
        int ka_width = M_canvas.scale( M_main_data.serverParam().keepaway_width_  );
        int ka_length = M_canvas.scale( M_main_data.serverParam().keepaway_length_ );

        painter.drawRect( ka_left, ka_top, ka_length, ka_width );
    }
    else
    {
        // center line
        painter.drawLine( M_canvas.fieldCenter().x(), top_y,
                          M_canvas.fieldCenter().x(), bottom_y );

        // center circle
        int center_radius = M_canvas.scale( CENTER_CIRCLE_R );
        painter.drawEllipse( M_canvas.fieldCenter().x() - center_radius,
                             M_canvas.fieldCenter().y() - center_radius,
                             center_radius * 2,
                             center_radius * 2 );
    }

    // corner arc
    {
        int r = M_canvas.scale( CORNER_ARC_R );

        painter.drawArc( left_x - r, top_y - r, r * 2, r * 2,
                         -90*16, 90*16 );
        painter.drawArc( left_x - r, bottom_y - r, r * 2, r * 2,
                         0*16, 90*16 );
        painter.drawArc( right_x - r, bottom_y - r, r * 2, r * 2,
                         90*16, 90*16 );
        painter.drawArc( right_x - r, top_y - r, r * 2, r * 2,
                         180*16, 90*16 );
    }
}

/*-------------------------------------------------------------------*/
/*!

 */
void
FieldPainter::drawPenaltyAreaLines( QPainter & painter ) const
{
    // set gdi objects
    painter.setPen( M_line_pen );
    painter.setBrush( Qt::NoBrush );

    // set screen coordinates of field
    int left_x   = M_canvas.screenX( - PITCH_LENGTH*0.5 );
    int right_x  = M_canvas.screenX( + PITCH_LENGTH*0.5 );

    // set penalty area params
    int pen_top_y    = M_canvas.screenY( - PITCH_WIDTH*0.5 );
    int pen_bottom_y = M_canvas.screenY( + PITCH_WIDTH*0.5 );
    double pen_circle_y_degree_abs
        = std::acos( ( PENALTY_AREA_LENGTH*0.5 - PENALTY_SPOT_DIST )
                     / PENALTY_CIRCLE_R )
        * ( 180.0 / M_PI );
    int span_angle = qRound( pen_circle_y_degree_abs * 2.0 * 16 );
    int pen_circle_r = M_canvas.scale( PENALTY_CIRCLE_R );
    int pen_circle_size = M_canvas.scale( PENALTY_CIRCLE_R * 2.0 );

    // left penalty area X
    int pen_x = M_canvas.screenX( -( PITCH_LENGTH*0.5 - PENALTY_AREA_LENGTH ) );
    // left arc
    int pen_spot_x = M_canvas.screenX( -( PITCH_LENGTH*0.5 - PENALTY_SPOT_DIST ) );
    painter.drawArc( pen_spot_x - pen_circle_r + 1,
                     M_canvas.fieldCenter().y - pen_circle_r,
                     pen_circle_size,
                     pen_circle_size,
                     qRound( -pen_circle_y_degree_abs * 16 ),
                     span_angle );
    // left rectangle
    painter.drawLine( left_x, pen_top_y, pen_x, pen_top_y );
    painter.drawLine( pen_x, pen_top_y, pen_x, pen_bottom_y );
    painter.drawLine( pen_x, pen_bottom_y, left_x, pen_bottom_y );
    // left spot
    painter.drawPoint( pen_spot_x, M_canvas.fieldCenter().y );

    // right penalty area X
    pen_x = M_canvas.screenX( +( PITCH_LENGTH*0.5 - PENALTY_AREA_LENGTH ) );
    // right arc
    pen_spot_x = M_canvas.screenX( +( PITCH_LENGTH*0.5 - PENALTY_SPOT_DIST ) );
    painter.drawArc( pen_spot_x - pen_circle_r,
                     M_canvas.fieldCenter().y - pen_circle_r,
                     pen_circle_size, pen_circle_size,
                     qRound( ( 180.0 - pen_circle_y_degree_abs + 0.5 ) * 16 ),
                     span_angle );
    // right rectangle
    painter.drawLine( right_x, pen_top_y, pen_x, pen_top_y );
    painter.drawLine( pen_x, pen_top_y, pen_x, pen_bottom_y );
    painter.drawLine( pen_x, pen_bottom_y, right_x, pen_bottom_y );
    // right spot
    painter.drawPoint( pen_spot_x, M_canvas.fieldCenter().y );
}

/*-------------------------------------------------------------------*/
/*!

 */
void
FieldPainter::drawGoalAreaLines( QPainter & painter ) const
{
    // set gdi objects
    painter.setPen( M_line_pen );
    painter.setBrush( Qt::NoBrush );

    // set screen coordinates of field
    int left_x   = M_canvas.screenX( - PITCH_LENGTH*0.5 );
    int right_x  = M_canvas.screenX( + PITCH_LENGTH*0.5 );

    // set coordinates M_canvass
    int goal_area_y_abs = M_canvas.scale( GOAL_AREA_WIDTH*0.5 );
    int goal_area_top_y = M_canvas.fieldCenter().y - goal_area_y_abs;
    int goal_area_bottom_y = M_canvas.fieldCenter().y + goal_area_y_abs;

    // left goal area
    int goal_area_x = M_canvas.screenX( - PITCH_LENGTH*0.5 + GOAL_AREA_LENGTH );
    painter.drawLine( left_x, goal_area_top_y, goal_area_x, goal_area_top_y );
    painter.drawLine( goal_area_x, goal_area_top_y, goal_area_x, goal_area_bottom_y );
    painter.drawLine( goal_area_x, goal_area_bottom_y, left_x, goal_area_bottom_y );

    // right goal area
    goal_area_x = M_canvas.screenX( PITCH_LENGTH*0.5 - GOAL_AREA_LENGTH );
    painter.drawLine( right_x, goal_area_top_y, goal_area_x, goal_area_top_y );
    painter.drawLine( goal_area_x, goal_area_top_y, goal_area_x, goal_area_bottom_y );
    painter.drawLine( goal_area_x, goal_area_bottom_y, right_x, goal_area_bottom_y );
}

/*-------------------------------------------------------------------*/
/*!

 */
void
FieldPainter::drawGoals( QPainter & painter ) const
{
    // set gdi objects
    painter.setPen( Qt::black );
    painter.setBrush( Qt::black );

    // set coordinates param
    int goal_top_y = M_canvas.screenY( - GOAL_WIDTH*0.5 );
    int goal_size_x = M_canvas.scale( GOAL_DEPTH );
    int goal_size_y = M_canvas.scale( GOAL_WIDTH );

    int post_top_y = M_canvas.screenY( - GOAL_WIDTH*0.5 - GOAL_POST_RADIUS*2.0 );
    int post_bottom_y = M_canvas.screenY( + GOAL_WIDTH*0.5 );
    int post_diameter = M_canvas.scale( GOAL_POST_RADIUS*2.0 );

    // left goal
    painter.drawRect( M_canvas.screenX( - PITCH_LENGTH*0.5 - GOAL_DEPTH ) - 1,
                      goal_top_y,
                      goal_size_x,
                      goal_size_y );
    if ( post_diameter >= 1 )
    {
        int post_x = M_canvas.screenX( - PITCH_LENGTH*0.5 );
        painter.drawEllipse( post_x,
                             post_top_y,
                             post_diameter,
                             post_diameter );
        painter.drawEllipse( post_x,
                             post_bottom_y,
                             post_diameter,
                             post_diameter );
    }
    // right goal
    painter.drawRect( M_canvas.screenX( PITCH_LENGTH*0.5 ) + 1,
                      goal_top_y,
                      goal_size_x,
                      goal_size_y );
    if ( post_diameter >= 1 )
    {
        int post_x = M_canvas.screenX( PITCH_LENGTH*0.5 - GOAL_POST_RADIUS*2.0 );
        painter.drawEllipse( post_x,
                             post_top_y,
                             post_diameter,
                             post_diameter );
        painter.drawEllipse( post_x,
                             post_bottom_y,
                             post_diameter,
                             post_diameter );
    }
}

/*-------------------------------------------------------------------*/
/*!

 */
void
FieldPainter::drawFlags( QPainter & painter ) const
{
    // set gdi objects
    painter.setPen( M_line_pen );
    painter.setBrush( Qt::NoBrush );

    // set size or coordinates params
    int flag_radius = M_canvas.scale( 0.5 );
    if ( flag_radius < 2 ) flag_radius = 2;
    if ( flag_radius > 5 ) flag_radius = 5;
    int flag_diameter = flag_radius * 2;

    int x, y;
    int pitch_half_length = M_canvas.scale( PITCH_LENGTH*0.5 );
    int pitch_half_width = M_canvas.scale( PITCH_WIDTH*0.5 );
    int pitch_margin_x = M_canvas.scale( PITCH_LENGTH*0.5 + PITCH_MARGIN );
    int pitch_margin_y = M_canvas.scale( PITCH_WIDTH*0.5 + PITCH_MARGIN );
    int penalty_x = M_canvas.scale( PITCH_LENGTH*0.5 - PENALTY_AREA_LENGTH );
    int penalty_y = M_canvas.scale( PENALTY_AREA_WIDTH*0.5 );
    int goal_y = M_canvas.scale( GOAL_WIDTH*0.5 );
    int scale10 = M_canvas.scale( 10.0 );
    int scale20 = M_canvas.scale( 20.0 );
    int scale30 = M_canvas.scale( 30.0 );
    int scale40 = M_canvas.scale( 40.0 );
    int scale50 = M_canvas.scale( 50.0 );

    QPainterPath path;

    // goal left
    x = M_canvas.fieldCenter().x - pitch_half_length - flag_radius;
    y = M_canvas.fieldCenter().y - flag_radius;
    path.addEllipse( x, y, flag_diameter, flag_diameter );
    // goal right
    x = M_canvas.fieldCenter().x + pitch_half_length - flag_radius;
    y = M_canvas.fieldCenter().y - flag_radius;
    path.addEllipse( x, y, flag_diameter, flag_diameter );
    // flag c
    x  = M_canvas.fieldCenter().x - flag_radius;
    y = M_canvas.fieldCenter().y - flag_radius;
    path.addEllipse( x, y, flag_diameter, flag_diameter );
    // flag c t
    x = M_canvas.fieldCenter().x - flag_radius;
    y = M_canvas.fieldCenter().y - pitch_half_width - flag_radius;
    path.addEllipse( x, y, flag_diameter, flag_diameter );

    // flag c b
    x = M_canvas.fieldCenter().x - flag_radius;
    y = M_canvas.fieldCenter().y + pitch_half_width - flag_radius;
    path.addEllipse( x, y, flag_diameter, flag_diameter );
    // flag l t
    x = M_canvas.fieldCenter().x - pitch_half_length - flag_radius;
    y = M_canvas.fieldCenter().y - pitch_half_width - flag_radius;
    path.addEllipse( x, y, flag_diameter, flag_diameter );
    // flag l b
    x = M_canvas.fieldCenter().x - pitch_half_length - flag_radius;
    y = M_canvas.fieldCenter().y + pitch_half_width - flag_radius;
    path.addEllipse( x, y, flag_diameter, flag_diameter );
    // flag r t
    x = M_canvas.fieldCenter().x + pitch_half_length - flag_radius;
    y = M_canvas.fieldCenter().y - pitch_half_width - flag_radius;
    path.addEllipse( x, y, flag_diameter, flag_diameter );
    // flag r b
    x = M_canvas.fieldCenter().x + pitch_half_length - flag_radius;
    y = M_canvas.fieldCenter().y + pitch_half_width - flag_radius;
    path.addEllipse( x, y, flag_diameter, flag_diameter );
    // flag p l t
    x = M_canvas.fieldCenter().x - penalty_x - flag_radius;
    y = M_canvas.fieldCenter().y - penalty_y - flag_radius;
    path.addEllipse( x, y, flag_diameter, flag_diameter );
    // flag p l c
    x = M_canvas.fieldCenter().x - penalty_x - flag_radius;
    y = M_canvas.fieldCenter().y - flag_radius;
    path.addEllipse( x, y, flag_diameter, flag_diameter );
    // flag p l b
    x = M_canvas.fieldCenter().x - penalty_x - flag_radius;
    y = M_canvas.fieldCenter().y + penalty_y - flag_radius;
    path.addEllipse( x, y, flag_diameter, flag_diameter );
    // flag p r t
    x = M_canvas.fieldCenter().x + penalty_x - flag_radius;
    y = M_canvas.fieldCenter().y - penalty_y - flag_radius;
    path.addEllipse( x, y, flag_diameter, flag_diameter );
    // flag p r c
    x = M_canvas.fieldCenter().x + penalty_x - flag_radius;
    y = M_canvas.fieldCenter().y - flag_radius;
    path.addEllipse( x, y, flag_diameter, flag_diameter );
    // flag p r b
    x = M_canvas.fieldCenter().x + penalty_x - flag_radius;
    y = M_canvas.fieldCenter().y + penalty_y - flag_radius;
    path.addEllipse( x, y, flag_diameter, flag_diameter );

    // flag g l t
    x = M_canvas.fieldCenter().x - pitch_half_length - flag_radius;
    y = M_canvas.fieldCenter().y - goal_y - flag_radius;
    path.addEllipse( x, y, flag_diameter, flag_diameter );
    // flag g l b
    x = M_canvas.fieldCenter().x - pitch_half_length - flag_radius;
    y = M_canvas.fieldCenter().y + goal_y - flag_radius;
    path.addEllipse( x, y, flag_diameter, flag_diameter );
    // flag g r t
    x = M_canvas.fieldCenter().x + pitch_half_length - flag_radius;
    y = M_canvas.fieldCenter().y - goal_y - flag_radius;
    path.addEllipse( x, y, flag_diameter, flag_diameter );
    // flag g r b
    x = M_canvas.fieldCenter().x + pitch_half_length - flag_radius;
    y = M_canvas.fieldCenter().y + goal_y - flag_radius;
    path.addEllipse( x, y, flag_diameter, flag_diameter );

    // flag t ...

    y = M_canvas.fieldCenter().y - pitch_margin_y - flag_radius;
    // flag t l 50
    x = M_canvas.fieldCenter().x - scale50 - flag_radius;
    path.addEllipse( x, y, flag_diameter, flag_diameter );
    // flag t l 40
    x = M_canvas.fieldCenter().x - scale40 - flag_radius;
    path.addEllipse( x, y, flag_diameter, flag_diameter );
    // flag t l 30
    x = M_canvas.fieldCenter().x - scale30 - flag_radius;
    path.addEllipse( x, y, flag_diameter, flag_diameter );
    // flag t l 20
    x = M_canvas.fieldCenter().x - scale20 - flag_radius;
    path.addEllipse( x, y, flag_diameter, flag_diameter );
    // flag t l 10
    x = M_canvas.fieldCenter().x - scale10 - flag_radius;
    path.addEllipse( x, y, flag_diameter, flag_diameter );
    // flag t 0
    x = M_canvas.fieldCenter().x - flag_radius;
    path.addEllipse( x, y, flag_diameter, flag_diameter );
    // flag t r 10
    x = M_canvas.fieldCenter().x + scale10 - flag_radius;
    path.addEllipse( x, y, flag_diameter, flag_diameter );
    // flag t r 20
    x = M_canvas.fieldCenter().x + scale20 - flag_radius;
    path.addEllipse( x, y, flag_diameter, flag_diameter );
    // flag t r 30
    x = M_canvas.fieldCenter().x + scale30 - flag_radius;
    path.addEllipse( x, y, flag_diameter, flag_diameter );
    // flag t r 40
    x = M_canvas.fieldCenter().x + scale40 - flag_radius;
    path.addEllipse( x, y, flag_diameter, flag_diameter );
    // flag t r 50
    x = M_canvas.fieldCenter().x + scale50 - flag_radius;
    path.addEllipse( x, y, flag_diameter, flag_diameter );

    // flag b ...

    y = M_canvas.fieldCenter().y + pitch_margin_y - flag_radius;
    // flag b l 50
    x = M_canvas.fieldCenter().x - scale50 - flag_radius;
    path.addEllipse( x, y, flag_diameter, flag_diameter );
    // flag b l 40
    x = M_canvas.fieldCenter().x - scale40 - flag_radius;
    path.addEllipse( x, y, flag_diameter, flag_diameter );
    // flag b l 30
    x = M_canvas.fieldCenter().x - scale30 - flag_radius;
    path.addEllipse( x, y, flag_diameter, flag_diameter );
    // flag b l 20
    x = M_canvas.fieldCenter().x - scale20 - flag_radius;
    path.addEllipse( x, y, flag_diameter, flag_diameter );
    // flag b l 10
    x = M_canvas.fieldCenter().x - scale10 - flag_radius;
    path.addEllipse( x, y, flag_diameter, flag_diameter );
    // flag b 0
    x = M_canvas.fieldCenter().x - flag_radius;
    path.addEllipse( x, y, flag_diameter, flag_diameter );
    // flag b r 10
    x = M_canvas.fieldCenter().x + scale10 - flag_radius;
    path.addEllipse( x, y, flag_diameter, flag_diameter );
    // flag b r 20
    x = M_canvas.fieldCenter().x + scale20 - flag_radius;
    path.addEllipse( x, y, flag_diameter, flag_diameter );
    // flag b r 30
    x = M_canvas.fieldCenter().x + scale30 - flag_radius;
    path.addEllipse( x, y, flag_diameter, flag_diameter );
    // flag b r 40
    x = M_canvas.fieldCenter().x + scale40 - flag_radius;
    path.addEllipse( x, y, flag_diameter, flag_diameter );
    // flag b r 50
    x = M_canvas.fieldCenter().x + scale50 - flag_radius;
    path.addEllipse( x, y, flag_diameter, flag_diameter );

    // flag l ...

    x = M_canvas.fieldCenter().x - pitch_margin_x - flag_radius;
    // flag l t 30
    y = M_canvas.fieldCenter().y - scale30 - flag_radius;
    path.addEllipse( x, y, flag_diameter, flag_diameter );
    // flag l t 20
    y = M_canvas.fieldCenter().y - scale20 - flag_radius;
    path.addEllipse( x, y, flag_diameter, flag_diameter );
    // flag l t 10
    y = M_canvas.fieldCenter().y - scale10 - flag_radius;
    path.addEllipse( x, y, flag_diameter, flag_diameter );
    // flag l 0
    y = M_canvas.fieldCenter().y - flag_radius;
    path.addEllipse( x, y, flag_diameter, flag_diameter );
    // flag l b 10
    y = M_canvas.fieldCenter().y + scale10 - flag_radius;
    path.addEllipse( x, y, flag_diameter, flag_diameter );
    // flag l b 20
    y = M_canvas.fieldCenter().y + scale20 - flag_radius;
    path.addEllipse( x, y, flag_diameter, flag_diameter );
    // flag l b 30
    y = M_canvas.fieldCenter().y + scale30 - flag_radius;
    path.addEllipse( x, y, flag_diameter, flag_diameter );

    // flag r ...

    x = M_canvas.fieldCenter().x + pitch_margin_x - flag_radius;
    // flag r t 30
    y = M_canvas.fieldCenter().y - scale30 - flag_radius;
    path.addEllipse( x, y, flag_diameter, flag_diameter );
    // flag r t 20
    y = M_canvas.fieldCenter().y - scale20 - flag_radius;
    path.addEllipse( x, y, flag_diameter, flag_diameter );
    // flag r t 10
    y = M_canvas.fieldCenter().y - scale10 - flag_radius;
    path.addEllipse( x, y, flag_diameter, flag_diameter );
    // flag r 0
    y = M_canvas.fieldCenter().y - flag_radius;
    path.addEllipse( x, y, flag_diameter, flag_diameter );
    // flag r b 10
    y = M_canvas.fieldCenter().y + scale10 - flag_radius;
    path.addEllipse( x, y, flag_diameter, flag_diameter );
    // flag r b 20
    y = M_canvas.fieldCenter().y + scale20 - flag_radius;
    path.addEllipse( x, y, flag_diameter, flag_diameter );
    // flag r b 30
    y = M_canvas.fieldCenter().y + scale30 - flag_radius;
    path.addEllipse( x, y, flag_diameter, flag_diameter );

    painter.drawPath( path );
}

/*-------------------------------------------------------------------*/
/*!

 */
void
FieldPainter::drawGrid( QPainter & painter ) const
{
    const double grid_step = 0.1 * M_canvas.gridStep();
    const int istep = M_canvas.scale( grid_step );
    const int text_step = ( M_canvas.isShownGridCoord()
                            ? 10
                            : 100000 );

    if ( istep < 2 )
    {
        return;
    }

    const QRect win = painter.window();

    const int max_ix = win.right();
    const int min_ix = win.left();
    const int max_iy = win.bottom();
    const int min_iy = win.top();
    const double max_x = M_canvas.fieldX( max_ix );
    const double min_x = M_canvas.fieldX( min_ix );
    const double max_y = M_canvas.fieldY( max_iy );
    const double min_y = M_canvas.fieldY( min_iy );

    //     std::cerr << "drawGrid  min_x = " << min_x
    //               << "  max_x = " << max_x
    //               << "  min_y = " << min_y
    //               << "  max_y = " << max_y
    //               << std::endl;

    painter.setPen( M_line_pen );
    painter.setBrush( Qt::NoBrush );

    QString text;

    double x = 0.0;
    while ( x < max_x )
    {
        int ix = M_canvas.screenX( x );
        if ( istep > text_step )
        {
            text.sprintf( "%.1f", x );
            painter.drawText( ix, max_iy, text );
        }
        painter.drawLine( ix, max_iy, ix, min_iy );
        x += grid_step;
    }

    x = -grid_step;
    while ( min_x < x )
    {
        int ix = M_canvas.screenX( x );
        if ( istep > text_step )
        {
            text.sprintf( "%.1f", x );
            painter.drawText( ix, max_iy, text );
        }
        painter.drawLine( ix, max_iy, ix, min_iy );
        x -= grid_step;
    }


    double y = 0.0;
    while ( y < max_y )
    {
        int iy = M_canvas.screenY( y );
        if ( istep > text_step )
        {
            text.sprintf( "%.1f", y );
            painter.drawText( min_ix, iy, text );
        }
        painter.drawLine( max_ix, iy, min_ix, iy );
        y += grid_step;
    }

    y = -grid_step;
    while ( min_y < y )
    {
        int iy = M_canvas.screenY( y );
        if ( istep > text_step )
        {
            text.sprintf( "%.1f", y );
            painter.drawText( min_ix, iy, text );
        }
        painter.drawLine( max_ix, iy, min_ix, iy );
        y -= grid_step;
    }
}
