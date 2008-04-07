// -*-c++-*-

/*!
  \file draw_info_painter.h
  \brief draw info painter class Header File.
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

#ifndef RCSSLOGPLAYER_DRAW_INFO_PAINTER_H
#define RCSSLOGPLAYER_DRAW_INFO_PAINTER_H

#include "painter_interface.h"

#include <QPen>
#include <QBrush>
#include <QFont>

class MainData;

class DrawInfoPainter
    : public PainterInterface {
private:

    const MainData & M_main_data;

    QPen M_pen;

    // not used
    DrawInfoPainter();
    DrawInfoPainter( const DrawInfoPainter & );
    const DrawInfoPainter & operator=( const DrawInfoPainter & );
public:

    DrawInfoPainter( const MainData & main_data );
    ~DrawInfoPainter();

    void draw( QPainter & painter );

private:

    void readSettings();
    void writeSettings();


};

#endif
