// -*-c++-*-

/*!
  \file log_player_tool_bar.h
  \brief log player control tool bar class Header File.
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

#ifndef RCSSLOGPLAYER_LOG_PLAYER_TOOL_BAR_H
#define RCSSLOGPLAYER_LOG_PLAYER_TOOL_BAR_H

#include <QToolBar>

class QAction;
class QBoxLayout;
class QMainWindow;
class QSlider;
class QLineEdit;
class QMoveEvent;

class LogPlayer;
class MainData;

class LogPlayerToolBar
    : public QToolBar {

    Q_OBJECT

private:

    const MainData & M_main_data;
    LogPlayer * M_log_player;

    QAction * M_toggle_record_act;

public:

    LogPlayerToolBar( LogPlayer * log_player,
                      const MainData & main_data,
                      QMainWindow * main_win );
    ~LogPlayerToolBar();

private:

    void createControls( LogPlayer * log_player,
                         QMainWindow * main_win );

protected:

//     void moveEvent( QMoveEvent * event );

public slots:

    void checkRecord( bool checked );
    void enableRecord( bool checked );

private slots:


    void stop();
    void playForward();
    void playBack();

signals:

    void cycleChanged( int cycle );
    void recordToggled( bool checked );

};

#endif
