// -*-c++-*-

/*!
  \file log_slider_tool_bar.h
  \brief log player slider tool bar class Header File.
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

#ifndef RCSSLOGPLAYER_LOG_SLIDER_TOOL_BAR_H
#define RCSSLOGPLAYER_LOG_SLIDER_TOOL_BAR_H

#include <QToolBar>

class QAction;
class QBoxLayout;
class QMainWindow;
class QSlider;
class QLineEdit;
class QMoveEvent;

class LogPlayer;
class MainData;

class LogSliderToolBar
    : public QToolBar {

    Q_OBJECT

private:

    const MainData & M_main_data;
    LogPlayer * M_log_player;

    QSlider * M_cycle_slider;
    QLineEdit * M_cycle_edit;

public:

    LogSliderToolBar( LogPlayer * log_player,
                      const MainData & main_data,
                      QMainWindow * main_win );
    ~LogSliderToolBar();

private:

    void createControls( LogPlayer * log_player,
                         QMainWindow * main_win );

protected:

//     void moveEvent( QMoveEvent * event );

private slots:

    void editCycle();

public slots:

    void changeOrientation( Qt::Orientation );

    void updateSlider();

signals:

    void cycleChanged( int cycle );

};

#endif
