// -*-c++-*-

/*!
  \file score_board_painter.h
  \brief score board painter class Header File.
*/

/*
 *Copyright:

 Copyright (C) The RoboCup Soccer Server Maintenance Group.
 Hidehisa Akiyama

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

#ifndef RCSSLOGPLAYER_SCORE_BOARD_PAINTER_RCSS_H
#define RCSSLOGPLAYER_SCORE_BOARD_PAINTER_RCSS_H

#include "painter_interface.h"

#include <QPen>
#include <QBrush>
#include <QFont>

class FieldCanvas;
class MainData;

class ScoreBoardPainter
    : public PainterInterface {
private:
    const FieldCanvas & M_canvas;
    const MainData & M_main_data;


    QPen M_brush;
    QPen M_font_pen;
    QFont M_font;

    // not used
    ScoreBoardPainter();
    ScoreBoardPainter( const ScoreBoardPainter & );
    const ScoreBoardPainter & operator=( const ScoreBoardPainter & );
public:

    ScoreBoardPainter( const FieldCanvas & canvas,
                       const MainData & main_data );
    ~ScoreBoardPainter();

    void draw( QPainter & painter );

private:

    void readSettings();
    void writeSettings();


};

#endif
