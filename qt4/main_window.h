// -*-c++-*-

/*!
  \file main_window.h
  \brief main application window class Header File.
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

#ifndef RCSSLOGPLAYER_MAIN_WINDOW_H
#define RCSSLOGPLAYER_MAIN_WINDOW_H

#include <QMainWindow>
#include <QString>

#include "main_data.h"

#include <boost/shared_ptr.hpp>

class QAction;
class QActionGroup;
class QCloseEvent;
class QDragEnterEvent;
class QDropEvent;
class QLabel;
class QPoint;
class QResizeEvent;

class ConfigDialog;
class DetailDialog;
class FieldCanvas;
class LogPlayer;
class LogPlayerToolBar;
class MonitorServer;
//class MonitorClient;
class PlayerTypeDialog;

class MainWindow
    : public QMainWindow {

    Q_OBJECT

private:

    MainData M_main_data;

    QString M_window_style;
    QString M_game_log_path; //!< file path to the last opened game log

    FieldCanvas * M_field_canvas;
    //LogPlayer * M_log_player;
    //LogPlayerToolBar * M_log_player_tool_bar;
    //ConfigDialog * M_config_dialog;

    QLabel * M_position_label;

    //boost::shared_ptr< MonitorServer > M_monitor_server;
    //boost::shared_ptr< MonitorClient > M_monitor_client;

    // file actions
    QAction * M_open_act;
    //QAction * M_save_rcg_act;
    QAction * M_exit_act;

    // monitor actions
    QAction * M_kick_off_act;
    QAction * M_set_live_mode_act;
    QAction * M_connect_monitor_act;
    QAction * M_connect_monitor_to_act;
    QAction * M_disconnect_monitor_act;
    QAction * M_toggle_drag_move_mode_act;

    // view actions
    QAction * M_toggle_menu_bar_act;
    QAction * M_toggle_tool_bar_act;
    QAction * M_toggle_status_bar_act;
    QAction * M_full_screen_act;
    QAction * M_show_player_type_dialog_act;
    QAction * M_show_detail_dialog_act;
    QActionGroup * M_style_act_group;
    QAction * M_show_config_dialog_act;

    // help actions
    QAction * M_about_act;

    // not used
    MainWindow( const MainWindow & );
    const MainWindow & operator=( const MainWindow & );

public:

    MainWindow();
    ~MainWindow();

    //! initialize application settings using command line options.
    void init();

private:

    void readSettings();
    void writeSettings();

    void createActions();
    void createActionsFile();
    void createActionsMonitor();
    void createActionsView();
    void createActionsHelp();

    void createMenus();
    void createMenuFile();
    void createMenuMonitor();
    void createMenuView();
    void createMenuHelp();

    void createToolBars();
    void createStatusBar();

    void createFieldCanvas();
    void createConfigDialog();

protected:

    // overrided method (virtual in super)
    void closeEvent( QCloseEvent * event );
    void resizeEvent( QResizeEvent * event );
    void wheelEvent( QWheelEvent * event );

    void dragEnterEvent( QDragEnterEvent * event );
    void dropEvent( QDropEvent * event );

private:

    void openRCG( const QString & file_path );
    void connectMonitorTo( const char * hostname );

private slots:

    // file menu actions slots
    void openRCG();
    // QWidget::close() can be used as the slot for a quit action.

    // monitor menu actions slots
    void kickOff();
    void setLiveMode();
    void connectMonitor();
    void connectMonitorTo();
    void disconnectMonitor();

    // view menu actions slots
    //void toggleMenuBar();
    void toggleToolBar();
    void toggleStatusBar();
    void toggleFullScreen();
    void showPlayerTypeDialog();
    void showDetailDialog();
    void changeStyle( bool checked );
    void showConfigDialog();

    // tool menu actions slots
    void showDebugMessageWindow();
    void toggleDebugServer( bool on );
    void startDebugServer();
    void stopDebugServer();
    void showImageSaveDialog();

    // help menu actions slots
    void about();

    void resizeCanvas( const QSize & size );

    void saveImageAndQuit();

public slots:

    void receiveMonitorPacket();

    void updatePositionLabel( const QPoint & point );

    void dropBallThere();
    void dropBall( const QPoint & pos );
    void freeKickLeft( const QPoint & pos );
    void freeKickRight( const QPoint & pos );

    void movePlayer( const QPoint & point );
    void moveObjects();

signals:

    void viewUpdated();

};

#endif
