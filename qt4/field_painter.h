// -*-c++-*-

/*!
  \file field_painter.h
  \brief field painter class Header File.
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

#ifndef RCSSLOGPLAYER_FIELD_PAINTER_H
#define RCSSLOGPLAYER_FIELD_PAINTER_H

#include "painter_interface.h"

#include <QPen>
#include <QBrush>

class FieldCanvas;
class MainData;

class FieldPainter
    : public PainterInterface {
public:
    static const double PITCH_LENGTH;
    static const double PITCH_WIDTH;
    static const double PITCH_MARGIN;
    static const double CENTER_CIRCLE_R;
    static const double PENALTY_AREA_LENGTH;
    static const double PENALTY_AREA_WIDTH;
    static const double PENALTY_CIRCLE_R;
    static const double PENALTY_SPOT_DIST;
    static const double GOAL_AREA_LENGTH;
    static const double GOAL_AREA_WIDTH;
    static const double GOAL_DEPTH;
    static const double CORNER_ARC_R;
    static const double GOAL_POST_RADIUS;

private:

    const FieldCanvas & M_canvas;
    const MainData & M_main_data;

    QBrush M_field_brush;
    QPen M_line_pen;

    // not used
    FieldPainter();
    FieldPainter( const FieldPainter & );
    const FieldPainter & operator=( const FieldPainter & );
public:


    FieldPainter( const FieldCanvas & canvas,
                  const MainData & main_data );
    ~FieldPainter();

    void draw( QPainter & painter );

private:

    void readSettings();
    void writeSettings();

    void drawBackGround( QPainter & painter ) const;
    void drawLines( QPainter & painter ) const;
    void drawPenaltyAreaLines( QPainter & painter ) const;
    void drawGoalAreaLines( QPainter & painter ) const;
    void drawGoals( QPainter & painter ) const;
    void drawFlags( QPainter & painter ) const;
    void drawGrid( QPainter & painter ) const;
};

#endif
