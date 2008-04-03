// -*-c++-*-

/*!
  \file main_window.cpp
  \brief main application window class Source File.
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

#include "main_window.h"

//#include "color_setting_dialog.h"
//#include "image_save_dialog.h"
//#include "detail_dialog.h"
//#include "font_setting_dialog.h"
//#include "monitor_move_dialog.h"
//#include "player_type_dialog.h"
//#include "view_config_dialog.h"
#include "field_canvas.h"
//#include "monitor_client.h"
//#include "log_player.h"
//#include "log_player_tool_bar.h"
#include "options.h"

#include <string>
#include <iostream>
#include <cstring>

#include "icons/rcss.xpm"

#include "icons/open.xpm"

#ifndef PACKAGE_NAME
#define PACKAGE_NAME "rcsslogplayer"
#endif

/*-------------------------------------------------------------------*/
/*!

 */
MainWindow::MainWindow()
//:M_log_player( new LogPlayer( M_main_data, this ) )
//, M_detail_dialog( static_cast< DetailDialog * >( 0 ) )
//, M_player_type_dialog( static_cast< PlayerTypeDialog * >( 0 ) )
//, M_monitor_client( static_cast< MonitorClient * >( 0 ) )
{
    readSettings();

    createActions();
    createMenus();
    createToolBars();
    createStatusBar();
    // central widget
    createFieldCanvas();
    // control dialogs
    //    createViewConfigDialog();

    //     connect( M_log_player, SIGNAL( updated() ),
    //              this, SIGNAL( viewUpdated() ) );

    this->setWindowIcon( QIcon( QPixmap( rcss_xpm ) ) );
    this->setWindowTitle( tr( PACKAGE_NAME ) );

    this->setMinimumSize( 280, 220 );
    this->resize( Options::instance().windowWidth() > 0
                  ? Options::instance().windowWidth()
                  : 640,
                  Options::instance().windowHeight() > 0
                  ? Options::instance().windowHeight()
                  : 480 );
    this->move( Options::instance().windowX() >= 0
                ? Options::instance().windowX()
                : this->x(),
                Options::instance().windowY() >= 0
                ? Options::instance().windowY()
                : this->y() );

    // this->setWindowOpacity( 0.5 ); // window transparency

    this->setAcceptDrops( true );

    if ( Options::instance().hideToolBar() )
    {
        //M_log_player_tool_bar->hide();
        //M_monitor_tool_bar->hide();
    }

    if ( Options::instance().hideStatusBar() )
    {
        this->statusBar()->hide();
    }

    if ( Options::instance().hideMenuBar() )
    {
        this->menuBar()->hide();
    }
}

/*-------------------------------------------------------------------*/
/*!

 */
MainWindow::~MainWindow()
{
    //std::cerr << "delete MainWindow" << std::endl;

    saveSettings();
}

/*-------------------------------------------------------------------*/
/*!

 */
void
MainWindow::init()
{
    if ( ! Options::instance().gameLogFile().empty() )
    {
        openRCG( QString::fromStdString( Options::instance().gameLogFile() ) );
    }
    else if ( Options::instance().connect() )
    {
        std::string host = Options::instance().host();
        if ( host.empty() )
        {
            host = "127.0.0.1";
        }

        connectMonitorTo( host.c_str() );
    }

    if ( Options::instance().canvasWidth() > 0
         && Options::instance().canvasHeight() > 0 )
    {
        resizeCanvas( QSize( Options::instance().canvasWidth(),
                             Options::instance().canvasHeight() ) );
    }
    else if ( Options::instance().fullScreen() )
    {
        this->showFullScreen();
    }
    else if ( Options::instance().maximize() )
    {
        this->showMaximized();
    }

    if ( QApplication::setStyle( "plastique" ) )
    {
        Q_FOREACH( QAction * action, M_style_act_group->actions() )
        {
            if ( action->data().toString().toLower()
                 == QApplication::style()->objectName().toLower() )
            {
                action->setChecked( true );
                break;
            }
        }
    }

    if ( Options::instance().autoImageSave() )
    {
        QTimer::singleShot( 500,
                            this, SLOT( saveImageAndQuit() ) );
        this->setEnabled( false );
    }
}

/*-------------------------------------------------------------------*/
/*!

 */
void
MainWindow::readSettings()
{
    QSettings settings( QDir::homePath() + "/.soccerwindow2-qt4",
                        QSettings::IniFormat );
    //QSettings settings( QSettings::IniFormat,
    //                    QSettings::UserScope,
    //                    PACKAGE_NAME );
    //QSettings settings( QSettings::NativeFormat,
    //                    QSettings::UserScope,
    //                    "rctools",
    //                    PACKAGE_NAME );


    settings.beginGroup( "Options" );

    if ( Options::instance().gameLogDir().empty() )
    {
        Options::instance()
            .setGameLogDir( settings.value( "gameLogDir", "" )
                            .toString()
                            .toStdString() );
    }

    if ( Options::instance().debugLogDir().empty() )
    {
        Options::instance()
            .setDebugLogDir( settings.value( "debugLogDir", "" )
                             .toString()
                             .toStdString() );
    }

    settings.endGroup();
}

/*-------------------------------------------------------------------*/
/*!

 */
void
MainWindow::saveSettings()
{
    QSettings settings( QDir::homePath() + "/.soccerwindow2-qt4",
                        QSettings::IniFormat );
    //QSettings settings( QSettings::IniFormat,
    //                    QSettings::UserScope,
    //                    PACKAGE_NAME );
    //QSettings settings( QSettings::NativeFormat,
    //                    QSettings::UserScope,
    //                    "rctools",
    //                    PACKAGE_NAME );

    settings.beginGroup( "Options" );

    settings.setValue( "gameLogDir", Options::instance().gameLogDir().c_str() );
    settings.setValue( "debugLogDir", Options::instance().debugLogDir().c_str() );

    settings.endGroup();
}

/*-------------------------------------------------------------------*/
/*!

 */
void
MainWindow::createActions()
{
    createActionsFile();
    createActionsMonitor();
    createActionsView();
    createActionsTool();
    createActionsHelp();

}

/*-------------------------------------------------------------------*/
/*!

 */
void
MainWindow::createActionsFile()
{
    M_open_act = new QAction( QIcon( QPixmap( open_rcg_xpm ) ),
                              tr( "&Open rcg file..." ), this );
#ifdef Q_WS_MAC
    M_open_act->setShortcut( tr( "Meta+O" ) );
#else
    M_open_act->setShortcut( tr( "Ctrl+O" ) );
#endif
    M_open_act->setStatusTip( tr( "Open RoboCup Game Log file" ) );
    connect( M_open_act, SIGNAL( triggered() ), this, SLOT( openRCG() ) );
    this->addAction( M_open_act );
    //
    M_save_rcg_act = new QAction( QIcon( QPixmap( save_xpm ) ),
                                  tr( "Save rcg file as..." ), this );
    M_save_rcg_act->setStatusTip( tr( "Save RoboCup Game Log file" ) );
    connect( M_save_rcg_act, SIGNAL( triggered() ), this, SLOT( saveRCG() ) );
    this->addAction( M_save_rcg_act );
    //
    M_open_debug_view_act = new QAction( QIcon( QPixmap( open_xpm ) )
                                         ,tr( "Open debug view" ), this );
    M_open_debug_view_act
        ->setStatusTip( tr( "Open the directory where debug view logs exist" ) );
    connect( M_open_debug_view_act, SIGNAL( triggered() ),
             this, SLOT( openDebugView() ) );
    this->addAction( M_open_debug_view_act );
    //
    M_save_debug_view_act = new QAction( QIcon( QPixmap( save_xpm ) ),
                                         tr( "Save debug view" ), this );
    M_save_debug_view_act
        ->setStatusTip( tr( "Save debug view logs to the directory..." ) );
    connect( M_save_debug_view_act, SIGNAL( triggered() ),
             this, SLOT( saveDebugView() ) );
    this->addAction( M_save_debug_view_act );
    //
    M_exit_act = new QAction( tr( "&Quit" ), this );
#ifdef Q_WS_MAC
    M_exit_act->setShortcut( tr( "Meta+Q" ) );
#else
    M_exit_act->setShortcut( tr( "Ctrl+Q" ) );
#endif
    M_exit_act->setStatusTip( tr( "Exit the application." ) );
    connect( M_exit_act, SIGNAL( triggered() ), this, SLOT( close() ) );
    this->addAction( M_exit_act );
}

/*-------------------------------------------------------------------*/
/*!

 */
void
MainWindow::createActionsMonitor()
{
    M_kick_off_act = new QAction( tr( "&KickOff" ), this );
#ifdef Q_WS_MAC
    M_kick_off_act->setShortcut( tr( "Meta+K" ) );
#else
    M_kick_off_act->setShortcut( tr( "Ctrl+K" ) );
#endif
    M_kick_off_act->setStatusTip( tr( "Start the game" ) );
    M_kick_off_act->setEnabled( false );
    connect( M_kick_off_act, SIGNAL( triggered() ), this, SLOT( kickOff() ) );
    this->addAction( M_kick_off_act );
    //
    M_set_live_mode_act = new QAction( QIcon( QPixmap( logplayer_live_mode_xpm ) ),
                                       tr( "&Live Mode" ), this );
#ifdef Q_WS_MAC
    M_set_live_mode_act->setShortcut( tr( "Meta+L" ) );
#else
    M_set_live_mode_act->setShortcut( tr( "Ctrl+L" ) );
#endif
    M_set_live_mode_act->setStatusTip( tr( "set monitor to live mode" ) );
    M_set_live_mode_act->setEnabled( false );
    connect( M_set_live_mode_act, SIGNAL( triggered() ),
             this, SLOT( setLiveMode() ) );
    this->addAction( M_set_live_mode_act );
    //
    M_connect_monitor_act = new QAction( tr( "&Connect" ), this );
#ifdef Q_WS_MAC
    M_connect_monitor_act->setShortcut( tr( "Meta+C" ) );
#else
    M_connect_monitor_act->setShortcut( tr( "Ctrl+C" ) );
#endif
    M_connect_monitor_act
        ->setStatusTip( "Connect to the rcssserver on localhost" );
    M_connect_monitor_act->setEnabled( true );
    connect( M_connect_monitor_act, SIGNAL( triggered() ),
             this, SLOT( connectMonitor() ) );
    this->addAction( M_connect_monitor_act );
    //
    M_connect_monitor_to_act = new QAction( tr( "Connect &to ..." ), this );
    M_connect_monitor_to_act
        ->setStatusTip( tr( "Connect to the rcssserver on other host" ) );
    M_connect_monitor_to_act->setEnabled( true );
    connect( M_connect_monitor_to_act, SIGNAL( triggered() ),
             this, SLOT( connectMonitorTo() ) );
    this->addAction( M_connect_monitor_to_act );
    //
    M_disconnect_monitor_act = new QAction( tr( "&Disconnect" ), this );
    M_disconnect_monitor_act->setStatusTip( tr( "Disonnect from rcssserver" ) );
    M_disconnect_monitor_act->setEnabled( false );
    connect( M_disconnect_monitor_act, SIGNAL( triggered() ),
             this, SLOT( disconnectMonitor() ) );
    this->addAction( M_disconnect_monitor_act );
    //
#ifndef Q_WS_WIN
    M_kill_server_act = new QAction( tr( "&Kill server" ), this );
    M_kill_server_act->setStatusTip( tr( "Kill the rcssserver process" ) );
    M_kill_server_act->setEnabled( false );
    connect( M_kill_server_act, SIGNAL( triggered() ),
             this, SLOT( killServer() ) );
    this->addAction( M_kill_server_act );
    //
    M_restart_server_act = new QAction( tr( "(Re)&start server" ), this );
    M_restart_server_act->setStatusTip( tr( "(Re)start rcssserver" ) );
    connect( M_restart_server_act, SIGNAL( triggered() ),
             this, SLOT( restartServer() ) );
    this->addAction( M_restart_server_act );
#endif
    //
    M_toggle_drag_move_mode_act = new QAction( QIcon( QPixmap( monitor_move_player_xpm ) ),
                                               tr( "Drag Move Mode." ),
                                               this );
    M_toggle_drag_move_mode_act->setStatusTip( tr( "Toggle drag move mode." ) );
    M_toggle_drag_move_mode_act->setEnabled( false );
    M_toggle_drag_move_mode_act->setCheckable( true );
    M_toggle_drag_move_mode_act->setChecked( false );
    connect( M_toggle_drag_move_mode_act, SIGNAL( toggled( bool ) ),
             this, SLOT( toggleDragMoveMode( bool ) ) );
    this->addAction( M_toggle_drag_move_mode_act );
    //
    M_show_monitor_move_dialog_act = new QAction( tr( "Move Dialog" ), this );
    M_show_monitor_move_dialog_act->setStatusTip( tr( "Show player move dialog" ) );
    //M_show_monitor_move_dialog_act->setEnabled( false );
    connect( M_show_monitor_move_dialog_act, SIGNAL( triggered() ),
             this, SLOT( showMonitorMoveDialog() ) );
    this->addAction( M_show_monitor_move_dialog_act );
    //
#ifndef Q_WS_WIN
    M_show_launcher_dialog_act = new QAction( tr( "Launcher Dialog" ), this );
#ifdef Q_WS_MAC
    M_show_launcher_dialog_act->setShortcut( tr( "Meta+X" ) );
#else
    M_show_launcher_dialog_act->setShortcut( tr( "Ctrl+X" ) );
#endif
    M_show_launcher_dialog_act->setStatusTip( tr( "Show launcher dialog" ) );
    connect( M_show_launcher_dialog_act, SIGNAL( triggered() ),
             this, SLOT( showLauncherDialog() ) );
    this->addAction( M_show_launcher_dialog_act );
#endif
}

/*-------------------------------------------------------------------*/
/*!

 */
void
MainWindow::createActionsView()
{
    M_toggle_menu_bar_act = new QAction( tr( "&Menu Bar" ), this );
#ifdef Q_WS_MAC
    M_toggle_menu_bar_act->setShortcut( tr( "Meta+M" ) );
#else
    M_toggle_menu_bar_act->setShortcut( tr( "Ctrl+M" ) );
#endif
    M_toggle_menu_bar_act->setStatusTip( tr( "Show/Hide Menu Bar" ) );
    connect( M_toggle_menu_bar_act, SIGNAL( triggered() ),
             this, SLOT( toggleMenuBar() ) );
    this->addAction( M_toggle_menu_bar_act );
    //
    M_toggle_tool_bar_act = new QAction( tr( "&Tool Bar" ), this );
    M_toggle_tool_bar_act->setStatusTip( tr( "Show/Hide Tool Bar" ) );
    connect( M_toggle_tool_bar_act, SIGNAL( triggered() ),
             this, SLOT( toggleToolBar() ) );
    this->addAction( M_toggle_tool_bar_act );
    //
    M_toggle_status_bar_act = new QAction( tr( "&Status Bar" ), this );
    M_toggle_status_bar_act->setStatusTip( tr( "Show/Hide Status Bar" ) );
    connect( M_toggle_status_bar_act, SIGNAL( triggered() ),
             this, SLOT( toggleStatusBar() ) );
    this->addAction( M_toggle_status_bar_act );
    //
    M_full_screen_act = new QAction( tr( "&Full Screen" ), this );
    M_full_screen_act->setShortcut( tr( "F11" ) );
    //M_full_screen_act->setShortcut( tr( "Alt+Enter" ) );
    //M_full_screen_act->setShortcut( Qt::ALT + Qt::Key_Return );
    //M_full_screen_act->setShortcut( Qt::ALT + Qt::Key_Enter );
    M_full_screen_act->setStatusTip( tr( "Toggle Full Screen" ) );
    connect( M_full_screen_act, SIGNAL( triggered() ),
             this, SLOT( toggleFullScreen() ) );
    this->addAction( M_full_screen_act );
    //(void) new QShortcut( Qt::ALT + Qt::Key_Enter, this, SLOT( toggleFullScreen() ) );
    (void) new QShortcut( Qt::ALT + Qt::Key_Return,
                          this, SLOT( toggleFullScreen() ) );
    (void) new QShortcut( Qt::ALT + Qt::Key_Enter,
                          this, SLOT( toggleFullScreen() ) );
    //
    M_show_player_type_dialog_act = new QAction( tr( "&Player Type List" ), this );
#ifdef Q_WS_MAC
    M_show_player_type_dialog_act->setShortcut( tr( "Meta+T" ) );
#else
    M_show_player_type_dialog_act->setShortcut( tr( "Ctrl+T" ) );
#endif
    M_show_player_type_dialog_act
        ->setStatusTip( tr( "Show player type parameters dialog" ) );
    connect( M_show_player_type_dialog_act, SIGNAL( triggered() ),
             this, SLOT( showPlayerTypeDialog() ) );
    this->addAction( M_show_player_type_dialog_act );
    //
    M_show_detail_dialog_act = new QAction( tr( "&Object Detail" ), this );
#ifdef Q_WS_MAC
    M_show_detail_dialog_act->setShortcut( tr( "Meta+I" ) );
#else
    M_show_detail_dialog_act->setShortcut( tr( "Ctrl+I" ) );
#endif
    M_show_detail_dialog_act
        ->setStatusTip( tr( "Show detail information dialog" ) );
    connect( M_show_detail_dialog_act, SIGNAL( triggered() ),
             this, SLOT( showDetailDialog() ) );
    this->addAction( M_show_detail_dialog_act );

    // qt style menu group
    M_style_act_group = new QActionGroup( this );
    Q_FOREACH ( QString style_name, QStyleFactory::keys() )
    {
        QAction * subaction = new QAction( M_style_act_group );
        subaction->setText( style_name );
        subaction->setData( style_name );
        subaction->setCheckable( true );
        if ( style_name.toLower()
             == QApplication::style()->objectName().toLower() )
        {
            subaction->setChecked( true );
        }
        connect( subaction, SIGNAL( triggered( bool ) ),
                 this, SLOT( changeStyle( bool ) ) );
    }
    //
    M_show_color_setting_dialog_act = new QAction( tr( "&Color Settings" ),
                                                   this );
    M_show_color_setting_dialog_act
        ->setStatusTip( tr( "Show color setting dialog" ) );
    connect( M_show_color_setting_dialog_act, SIGNAL( triggered() ),
             this, SLOT( showColorSettingDialog() ) );
    this->addAction( M_show_color_setting_dialog_act );
    //
    M_show_font_setting_dialog_act = new QAction( tr( "&Font Settings" ), this );
    M_show_font_setting_dialog_act
        ->setStatusTip( tr( "Show font setting dialog" ) );
    connect( M_show_font_setting_dialog_act, SIGNAL( triggered() ),
             this, SLOT( showFontSettingDialog() ) );
    this->addAction( M_show_font_setting_dialog_act );
    //
    M_show_view_config_dialog_act = new QAction( tr( "&View Preference" ), this );
#ifdef Q_WS_MAC
    M_show_view_config_dialog_act->setShortcut( tr( "Meta+V" ) );
#else
    M_show_view_config_dialog_act->setShortcut( tr( "Ctrl+V" ) );
#endif
    M_show_view_config_dialog_act
        ->setStatusTip( tr( "Show view preference dialog" ) );
    connect( M_show_view_config_dialog_act, SIGNAL( triggered() ),
             this, SLOT( showViewConfigDialog() ) );
    this->addAction( M_show_view_config_dialog_act );
}

/*-------------------------------------------------------------------*/
/*!

 */
void
MainWindow::createActionsTool()
{
    M_show_debug_message_window_act = new QAction( tr( "Debug &Message" ), this );
#ifdef Q_WS_MAC
    M_show_debug_message_window_act->setShortcut( tr( "Meta+D" ) );
#else
    M_show_debug_message_window_act->setShortcut( tr( "Ctrl+D" ) );
#endif
    M_show_debug_message_window_act
        ->setStatusTip( tr( "Show debug message window" ) );
    connect( M_show_debug_message_window_act, SIGNAL( triggered() ),
             this, SLOT( showDebugMessageWindow() ) );
    this->addAction( M_show_debug_message_window_act );
    //
    M_toggle_debug_server_act = new QAction( QIcon( QPixmap( debug_server_switch_xpm ) ),
                                             tr( "Start/Stop the debug server." ),
                                             this );
#ifdef Q_WS_MAC
    M_toggle_debug_server_act->setShortcut( tr( "Meta+S" ) );
#else
    M_toggle_debug_server_act->setShortcut( tr( "Ctrl+S" ) );
#endif
    M_toggle_debug_server_act
        ->setStatusTip( tr( "Start/Stop the debug server." ) );
    M_toggle_debug_server_act->setEnabled( false );
    M_toggle_debug_server_act->setCheckable( true );
    M_toggle_debug_server_act->setChecked( false );
    connect( M_toggle_debug_server_act, SIGNAL( toggled( bool ) ),
             this, SLOT( toggleDebugServer( bool ) ) );
    this->addAction( M_toggle_debug_server_act );
    //
    M_show_image_save_dialog_act = new QAction( tr( "Save &Image" ), this );
    M_show_image_save_dialog_act
        ->setStatusTip( tr( "Save game log data as image files" ) );
    connect( M_show_image_save_dialog_act, SIGNAL( triggered() ),
             this, SLOT( showImageSaveDialog() ) );
    this->addAction( M_show_image_save_dialog_act );
}

/*-------------------------------------------------------------------*/
/*!

 */
void
MainWindow::createActionsHelp()
{
    M_about_act = new QAction( QIcon( QPixmap( soccerwindow2_nostr_xpm ) ),
                               tr( "&About" ), this );
    M_about_act->setStatusTip( tr( "Show the about dialog." ) );
    connect( M_about_act, SIGNAL( triggered() ), this, SLOT( about() ) );
    this->addAction( M_about_act );
}

/*-------------------------------------------------------------------*/
/*!

 */
void
MainWindow::createMenus()
{
    createMenuFile();
    createMenuMonitor();
    createMenuView();
    createMenuTool();
    createMenuHelp();
}

/*-------------------------------------------------------------------*/
/*!

 */
void
MainWindow::createMenuFile()
{
    QMenu * menu = menuBar()->addMenu( tr( "&File" ) );

    menu->addAction( M_open_act );
    menu->addAction( M_save_rcg_act );

    menu->addSeparator();
    menu->addAction( M_open_debug_view_act );
    menu->addAction( M_save_debug_view_act );

    menu->addSeparator();
    menu->addAction( M_exit_act );
}

/*-------------------------------------------------------------------*/
/*!

 */
void
MainWindow::createMenuMonitor()
{
    QMenu * menu = menuBar()->addMenu( tr( "&Monitor" ) );

    menu->addAction( M_kick_off_act );
    menu->addAction( M_set_live_mode_act );

    menu->addSeparator();
    menu->addAction( M_connect_monitor_act );
    menu->addAction( M_connect_monitor_to_act );
    menu->addAction( M_disconnect_monitor_act );

#ifndef Q_WS_WIN
    menu->addSeparator();
    menu->addAction( M_kill_server_act );
    menu->addAction( M_restart_server_act );
#endif
    menu->addSeparator();
    menu->addAction( M_toggle_drag_move_mode_act );
    menu->addAction( M_show_monitor_move_dialog_act );

#ifndef Q_WS_WIN
    menu->addSeparator();
    menu->addAction( M_show_launcher_dialog_act );
#endif
}

/*-------------------------------------------------------------------*/
/*!

 */
void
MainWindow::createMenuView()
{
    QMenu * menu = menuBar()->addMenu( tr( "&View" ) );
    menu->addAction( M_toggle_menu_bar_act );
    menu->addAction( M_toggle_tool_bar_act );
    menu->addAction( M_toggle_status_bar_act );

    menu->addSeparator();
    menu->addAction( M_full_screen_act );

    menu->addSeparator();
    menu->addAction( M_show_player_type_dialog_act );
    menu->addAction( M_show_detail_dialog_act );

    menu->addSeparator();
    {
        QMenu * submenu = menu->addMenu( tr( "Qt &Style" ) );
        Q_FOREACH ( QAction * action, M_style_act_group->actions() )
        {
            submenu->addAction( action );
        }
    }
    menu->addAction( M_show_color_setting_dialog_act );
    menu->addAction( M_show_font_setting_dialog_act );
    menu->addAction( M_show_view_config_dialog_act );
}

/*-------------------------------------------------------------------*/
/*!

 */
void
MainWindow::createMenuTool()
{
    QMenu * menu = menuBar()->addMenu( tr( "&Tool" ) );
    menu->addAction( M_show_debug_message_window_act );

    menu->addSeparator();
    menu->addAction( M_toggle_debug_server_act );

    menu->addSeparator();
    menu->addAction( M_show_image_save_dialog_act );

}
/*-------------------------------------------------------------------*/
/*!

 */
void
MainWindow::createMenuHelp()
{
    QMenu * menu = menuBar()->addMenu( tr( "&Help" ) );
    menu->addAction( M_about_act );

    //     QAction * act = new QAction( tr( "About Qt" ), this );
    //     act->setStatusTip( tr( "Show about Qt." ) );
    //     connect( act, SIGNAL( triggered() ), qApp, SLOT( aboutQt() ) );
    //     menu->addAction( act );
    menu->addAction( tr( "About Qt" ), qApp, SLOT( aboutQt() ) );
}

/*-------------------------------------------------------------------*/
/*!

 */
void
MainWindow::createToolBars()
{
    //     M_log_player_tool_bar = new LogPlayerToolBar( M_log_player,
    //                                                   M_main_data,
    //                                                   this );

    //     M_log_player_tool_bar->addSeparator();

    //     M_log_player_tool_bar->addAction( M_set_live_mode_act );
    //     M_log_player_tool_bar->addAction( M_toggle_drag_move_mode_act );
    //     M_log_player_tool_bar->addAction( M_toggle_debug_server_act );
    //     {
    //         QFrame * dummy_frame = new QFrame;
    //         QHBoxLayout * layout = new QHBoxLayout;
    //         //layout->addSpacing( 10 );
    //         layout->addStretch( 1 );
    //         dummy_frame->setLayout( layout );
    //         M_log_player_tool_bar->addWidget( dummy_frame );
    //     }
    //     {
    //         QFrame * dummy_frame = new QFrame;
    //         QVBoxLayout * layout = new QVBoxLayout;
    //         //layout->addSpacing( 10 );
    //         layout->addStretch( 1 );
    //         dummy_frame->setLayout( layout );
    //         M_log_player_tool_bar->addWidget( dummy_frame );
    //     }

    //     connect( this,  SIGNAL( viewUpdated() ),
    //              M_log_player_tool_bar, SLOT( updateSlider() ) );


    //     this->addToolBar( Qt::TopToolBarArea, M_log_player_tool_bar );
}

/*-------------------------------------------------------------------*/
/*!

 */
void
MainWindow::createStatusBar()
{
    this->statusBar()->showMessage( tr( "Ready" ) );

    M_position_label = new QLabel( tr( "(0.0, 0.0)" ) );

    int min_width
        = M_position_label->fontMetrics().width(  tr( "(-60.0, -30.0)" ) )
        + 16;
    M_position_label->setMinimumWidth( min_width );
    M_position_label->setAlignment( Qt::AlignRight );

    this->statusBar()->addPermanentWidget( M_position_label );
}

/*-------------------------------------------------------------------*/
/*!

 */
void
MainWindow::createFieldCanvas()
{
    M_field_canvas = new FieldCanvas( M_main_data );
    //M_field_canvas->setParent( this );
    this->setCentralWidget( M_field_canvas );

    M_field_canvas->setFocus();

    connect( this, SIGNAL( viewUpdated() ),
             M_field_canvas, SLOT( update() ) );
    //M_field_canvas, SLOT( repaint() ) );

    connect( M_field_canvas, SIGNAL( mouseMoved( const QPoint & ) ),
             this, SLOT( updatePositionLabel( const QPoint & ) ) );

    connect( M_field_canvas, SIGNAL( dropBall( const QPoint & ) ),
             this, SLOT( dropBall( const QPoint & ) ) );
    connect( M_field_canvas, SIGNAL( freeKickLeft( const QPoint & ) ),
             this, SLOT( freeKickLeft( const QPoint & ) ) );
    connect( M_field_canvas, SIGNAL( freeKickRight( const QPoint & ) ),
             this, SLOT( freeKickRight( const QPoint & ) ) );

    connect( M_field_canvas, SIGNAL( playerMoved( const QPoint & ) ),
             this, SLOT( movePlayer( const QPoint & ) ) );

    // create & set context menus
    {
        QMenu * menu = new QMenu( M_field_canvas );
        menu->addAction( M_open_act );
        menu->addAction( M_connect_monitor_act );

        M_field_canvas->setNormalMenu( menu );
    }
    //     {
    //         QMenu * menu = new QMenu( M_field_canvas );
    //         menu->addAction( M_open_act );
    //         menu->addAction( M_connect_monitor_act );
    // #ifndef Q_WS_WIN
    //         menu->addSeparator();
    //         menu->addAction( M_kill_server_act );
    //         menu->addAction( M_restart_server_act );
    // #endif

    //         M_field_canvas->setSystemMenu( menu );
    //     }
    //     {
    //         QMenu * menu = new QMenu( M_field_canvas );
    //         menu->addAction( M_kick_off_act );
    //         menu->addSeparator();
    //         menu->addAction( tr( "Drop Ball" ),
    //                          M_field_canvas, SLOT( dropBall() ) );
    //         menu->addAction( tr( "Free Kick Left" ),
    //                          M_field_canvas, SLOT( freeKickLeft() ) );
    //         menu->addAction( tr( "Free Kick Right" ),
    //                          M_field_canvas, SLOT( freeKickRight() ) );
    //         menu->addSeparator();

    //         QAction * drop_ball_there
    //             = new QAction( tr( "Drop Ball There" ), this );
    //         drop_ball_there->setStatusTip( tr( "Drop ball at the current ball position." ) );
    //         connect( drop_ball_there, SIGNAL( triggered() ),
    //                  this, SLOT( dropBallThere() ) );
    //         menu->addAction( drop_ball_there );

    //         M_field_canvas->setMonitorMenu( menu );
    //     }

}

/*-------------------------------------------------------------------*/
/*!

 */
#if 0
void
MainWindow::createViewConfigDialog()
{
    if ( M_view_config_dialog )
    {
        return;
    }

    M_view_config_dialog
        = new ViewConfigDialog( this,
                                M_main_data,
                                M_main_data.getViewConfig() );

    //M_view_config_dialog->setParent( this, Qt::Dialog );
    M_view_config_dialog->hide();

    connect( M_view_config_dialog, SIGNAL( configured() ),
             this, SIGNAL( viewUpdated() ) );

    connect( M_view_config_dialog, SIGNAL( canvasResized( const QSize & ) ),
             this, SLOT( resizeCanvas( const QSize & ) ) );

    connect( M_field_canvas, SIGNAL( focusChanged( const QPoint & ) ),
             M_view_config_dialog, SLOT( setFocusPoint( const QPoint & ) ) );

    connect( M_view_config_dialog, SIGNAL( configured() ),
             M_field_canvas, SLOT( setRedrawAllFlag()  ) );

    // register short cut keys
    {
        // z
        QAction * act = new QAction( tr( "ZoomIn" ), this );
        act->setShortcut( Qt::Key_Z );
        this->addAction( act );
        connect( act, SIGNAL( triggered() ),
                 M_view_config_dialog, SLOT( zoomIn() ) );
    }
    {
        // x
        QAction * act = new QAction( tr( "ZoomOut" ), this );
        act->setShortcut( Qt::Key_X );
        this->addAction( act );
        connect( act, SIGNAL( triggered() ),
                 M_view_config_dialog, SLOT( zoomOut() ) );
    }
    {
        // Ctrl + z
        QAction * act = new QAction( tr( "ZoomOut" ), this );
#ifdef Q_WS_MAC
        act->setShortcut( Qt::META + Qt::Key_Z );
#else
        act->setShortcut( Qt::CTRL + Qt::Key_Z );
#endif
        this->addAction( act );
        connect( act, SIGNAL( triggered() ),
                 M_view_config_dialog, SLOT( zoomOut() ) );
    }
    {
        // i
        QAction * act = new QAction( tr( "Fit" ), this );
        act->setShortcut( Qt::Key_I );
        this->addAction( act );
        connect( act, SIGNAL( triggered() ),
                 M_view_config_dialog, SLOT( unzoom() ) );
    }
    {
        // e
        QAction * act = new QAction( tr( "Toggle Enlarge" ), this );
        act->setShortcut( Qt::Key_E );
        this->addAction( act );
        connect( act, SIGNAL( triggered() ),
                 M_view_config_dialog, SLOT( toggleEnlarge() ) );
    }

    // reverse mode
    {
        // Ctrl + r
        QAction * act = new QAction( tr( "Reverse Side" ), this );
#ifdef Q_WS_MAC
        act->setShortcut( Qt::META + Qt::Key_R );
#else
        act->setShortcut( Qt::CTRL + Qt::Key_R );
#endif
        this->addAction( act );
        connect( act, SIGNAL( triggered() ),
                 M_view_config_dialog, SLOT( toggleReverseSide() ) );
    }
    {
        // Ctrl + Shift + r
        QAction * act = new QAction( tr( "Player Reverse Draw" ), this );
#ifdef Q_WS_MAC
        act->setShortcut( Qt::META + Qt::SHIFT + Qt::Key_R );
#else
        act->setShortcut( Qt::CTRL + Qt::SHIFT + Qt::Key_R );
#endif
        this->addAction( act );
        connect( act, SIGNAL( triggered() ),
                 M_view_config_dialog, SLOT( togglePlayerReverseDraw() ) );
    }

    // field style
    {
        // k
        QAction * act = new QAction( tr( "Toggle Keepaway" ), this );
        act->setShortcut( Qt::Key_K );
        this->addAction( act );
        connect( act, SIGNAL( triggered() ),
                 M_view_config_dialog, SLOT( toggleKeepawayMode() ) );
    }

    // player detail
    {
        // n
        QAction * act = new QAction( tr( "Show Player Number" ), this );
        act->setShortcut( Qt::Key_N );
        this->addAction( act );
        connect( act, SIGNAL( triggered() ),
                 M_view_config_dialog, SLOT( toggleShowPlayerNumber() ) );
    }
    {
        // h
        QAction * act = new QAction( tr( "Show Hetero Number" ), this );
        act->setShortcut( Qt::Key_H );
        this->addAction( act );
        connect( act, SIGNAL( triggered() ),
                 M_view_config_dialog, SLOT( toggleShowHeteroNumber() ) );
    }
    {
        // s
        QAction * act = new QAction( tr( "Show Staminar" ), this );
        act->setShortcut( Qt::Key_S );
        this->addAction( act );
        connect( act, SIGNAL( triggered() ),
                 M_view_config_dialog, SLOT( toggleShowStamina() ) );
    }
    {
        // v
        QAction * act = new QAction( tr( "Show View Cone" ), this );
        act->setShortcut( Qt::Key_V );
        this->addAction( act );
        connect( act, SIGNAL( triggered() ),
                 M_view_config_dialog, SLOT( toggleShowViewCone() ) );
    }
    {
        // d
        QAction * act = new QAction( tr( "Show Body Shadow" ), this );
        act->setShortcut( Qt::Key_D );
        this->addAction( act );
        connect( act, SIGNAL( triggered() ),
                 M_view_config_dialog, SLOT( toggleShowBodyShadow() ) );
    }
    {
        // c
        QAction * act = new QAction( tr( "Show Control Area" ), this );
        act->setShortcut( Qt::Key_C );
        this->addAction( act );
        connect( act, SIGNAL( triggered() ),
                 M_view_config_dialog, SLOT( toggleShowControlArea() ) );
    }

    // show/hide
    {
        // t
        QAction * act = new QAction( tr( "Show Score Board" ), this );
        act->setShortcut( Qt::Key_T );
        this->addAction( act );
        connect( act, SIGNAL( triggered() ),
                 M_view_config_dialog, SLOT( toggleShowScoreBoard() ) );
    }
    {
        // Ctrl + b
        QAction * act = new QAction( tr( "Show Ball" ), this );
#ifdef Q_WS_MAC
        act->setShortcut( Qt::META + Qt::Key_B );
#else
        act->setShortcut( Qt::CTRL + Qt::Key_B );
#endif
        this->addAction( act );
        connect( act, SIGNAL( triggered() ),
                 M_view_config_dialog, SLOT( toggleShowBall() ) );
    }
    {
        // Ctrl + p
        QAction * act = new QAction( tr( "Show Players" ), this );
#ifdef Q_WS_MAC
        act->setShortcut( Qt::META + Qt::Key_P );
#else
        act->setShortcut( Qt::CTRL + Qt::Key_P );
#endif
        this->addAction( act );
        connect( act, SIGNAL( triggered() ),
                 M_view_config_dialog, SLOT( toggleShowPlayers() ) );
    }
    {
        // f
        QAction * act = new QAction( tr( "Show Flags" ), this );
        act->setShortcut( Qt::Key_F );
        this->addAction( act );
        connect( act, SIGNAL( triggered() ),
                 M_view_config_dialog, SLOT( toggleShowFlags() ) );
    }
    {
        // o
        QAction * act = new QAction( tr( "Show Offside Line" ), this );
        act->setShortcut( Qt::Key_O );
        this->addAction( act );
        connect( act, SIGNAL( triggered() ),
                 M_view_config_dialog, SLOT( toggleShowOffsideLine() ) );
    }

    // number 1-10
    for ( int i = 0; i < 10; ++i )
    {
        {
            QAction * act = new QAction( QString( "Selct Left %1" ).arg( i ), this );
            act->setShortcut( Qt::Key_0 + i );
            this->addAction( act );
            connect( act, SIGNAL( triggered() ),
                     M_view_config_dialog, SLOT( selectPlayerWithKey() ) );
        }
        {
            QAction * act = new QAction( QString( "Selct Right %1" ).arg( i ), this );
#ifdef Q_WS_MAC
            act->setShortcut( Qt::META + Qt::Key_0 + i );
#else
            act->setShortcut( Qt::CTRL + Qt::Key_0 + i );
#endif
            this->addAction( act );
            connect( act, SIGNAL( triggered() ),
                     M_view_config_dialog, SLOT( selectPlayerWithKey() ) );
        }
    }
    // number 11
    {
        {
            QAction * act = new QAction( tr( "Selct Left 11" ), this );
            act->setShortcut( Qt::Key_Minus );
            this->addAction( act );
            connect( act, SIGNAL( triggered() ),
                     M_view_config_dialog, SLOT( selectPlayerWithKey() ) );
        }
        {
            QAction * act = new QAction( tr( "Selct Right 11" ), this );
#ifdef Q_WS_MAC
            act->setShortcut( Qt::META + Qt::Key_Minus );
#else
            act->setShortcut( Qt::CTRL + Qt::Key_Minus );
#endif
            this->addAction( act );
            connect( act, SIGNAL( triggered() ),
                     M_view_config_dialog, SLOT( selectPlayerWithKey() ) );
        }
    }
    // b
    {
        QAction * act = new QAction( tr( "Focus Ball" ), this );
        act->setShortcut( Qt::Key_B );
        this->addAction( act );
        connect( act, SIGNAL( triggered() ),
                 M_view_config_dialog, SLOT( toggleFocusBall() ) );
    }
    // p
    {
        QAction * act = new QAction( tr( "Focus Player" ), this );
        act->setShortcut( Qt::Key_P );
        this->addAction( act );
        connect( act, SIGNAL( triggered() ),
                 M_view_config_dialog, SLOT( toggleFocusPlayer() ) );
    }
    // a
    {
        QAction * act = new QAction( tr( "Select auto all" ), this );
        act->setShortcut( Qt::Key_A );
        this->addAction( act );
        connect( act, SIGNAL( triggered() ),
                 M_view_config_dialog, SLOT( toggleSelectAutoAll() ) );
    }
    // l
    {
        QAction * act = new QAction( tr( "Select auto left" ), this );
        act->setShortcut( Qt::Key_L );
        this->addAction( act );
        connect( act, SIGNAL( triggered() ),
                 M_view_config_dialog, SLOT( toggleSelectAutoLeft() ) );
    }
    // r
    {
        QAction * act = new QAction( tr( "Select auto right" ), this );
        act->setShortcut( Qt::Key_R );
        this->addAction( act );
        connect( act, SIGNAL( triggered() ),
                 M_view_config_dialog, SLOT( toggleSelectAutoRight() ) );
    }
    // u
    {
        QAction * act = new QAction( tr( "Unselect" ), this );
        act->setShortcut( Qt::Key_U );
        this->addAction( act );
        connect( act, SIGNAL( triggered() ),
                 M_view_config_dialog, SLOT( setUnselect() ) );
    }
}
#endif

/*-------------------------------------------------------------------*/
/*!

 */
void
MainWindow::closeEvent( QCloseEvent * event )
{
    event->ignore();

    //QCoreApplication::instance()->quit();
    qApp->quit();
}

/*-------------------------------------------------------------------*/
/*!

 */
void
MainWindow::resizeEvent( QResizeEvent * event )
{
    event->accept();

    if ( M_view_config_dialog
         && M_view_config_dialog->isVisible() )
    {
        M_view_config_dialog->updateFieldScale();
    }
}

/*-------------------------------------------------------------------*/
/*!

 */
void
MainWindow::wheelEvent( QWheelEvent * event )
{
    if ( event->delta() < 0 )
    {
        M_log_player->stepForward();
    }
    else
    {
        M_log_player->stepBack();
    }

    event->accept();
}

/*-------------------------------------------------------------------*/
/*!

 */
void
MainWindow::dragEnterEvent( QDragEnterEvent * event )
{
    if ( event->mimeData()->hasFormat( "text/uri-list" ) )
    {
        event->acceptProposedAction();
    }
}

/*-------------------------------------------------------------------*/
/*!

 */
void
MainWindow::dropEvent( QDropEvent * event )
{
    const QMimeData * mimedata = event->mimeData();

    QList< QUrl > urls = mimedata->urls();

    //     std::cerr << "urls size = " << urls.size() << std::endl;

    //     for ( int i = 0; i < urls.size() && i < 32; ++i )
    //     {
    //         std::cerr << "url " << i << ": "
    //                   << urls.at(i).path().toStdString()
    //                   << std::endl;
    //     }

    while ( ! urls.empty()
            && urls.back().isEmpty() )
    {
        urls.pop_back();
    }

    if ( urls.empty() )
    {
        QMessageBox::information( this,
                                  tr( "Information" ),
                                  tr( "Dropped data has no file path." ),
                                  QMessageBox::Ok, QMessageBox::NoButton );
    }
    else if ( urls.size() == 1 )
    {
        openRCG( urls.front().toLocalFile() );
    }
    else if ( urls.size() > 1 )
    {
        QMessageBox::critical( this,
                               tr( "Error" ),
                               tr( "Too many files are dropped." ),
                               QMessageBox::Ok, QMessageBox::NoButton );
    }

    event->acceptProposedAction();
}

/*-------------------------------------------------------------------*/
/*!

 */
void
MainWindow::openRCG()
{
#ifdef HAVE_LIBRCSC_GZ
    QString filter( tr( "Game Log files (*.rcg *.rcg.gz);;"
                        "All files (*)" ) );
#else
    QString filter( tr( "Game Log files (*.rcg);;"
                        "All files (*)" ) );
#endif

    QString default_dir
        = QString::fromStdString( Options::instance().gameLogDir() );

    QString file_path = QFileDialog::getOpenFileName( this,
                                                      tr( "Choose a game log file to open" ),
                                                      default_dir,
                                                      filter );

    if ( file_path.isEmpty() )
    {
        //std::cerr << "MainWindow::opneRCG() empty file path" << std::endl;
        return;
    }

    std::cerr << "file = [" << file_path.toStdString() << ']' << std::endl;

    openRCG( file_path );
}

/*-------------------------------------------------------------------*/
/*!

 */
void
MainWindow::openRCG( const QString & file_path )
{
    if ( ! QFile::exists( file_path ) )
    {
        std::cerr << "File [" << file_path.toStdString()
                  << "] does not exist."
                  << std::endl;
        return;
    }

    disconnectMonitor();
    M_log_player->stop();

    if ( ! M_main_data.openRCG( file_path.toStdString() ) )
    {
        QString err_msg = tr( "Failed to read [" );
        err_msg += file_path;
        err_msg += tr( "]" );
        QMessageBox::critical( this,
                               tr( "Error" ),
                               err_msg,
                               QMessageBox::Ok, QMessageBox::NoButton );
        this->setWindowTitle( tr( PACKAGE_NAME ) );
        return;
    }

    if ( M_main_data.viewHolder().monitorViewCont().empty() )
    {
        QString err_msg = tr( "Empty log file [" );
        err_msg += file_path;
        err_msg += tr( "]" );
        QMessageBox::critical( this,
                               tr( "Error" ),
                               err_msg,
                               QMessageBox::Ok, QMessageBox::NoButton );
        this->setWindowTitle( tr( PACKAGE_NAME ) );
        return;
    }

    QFileInfo file_info( file_path );

    Options::instance().setGameLogDir( file_info.absoluteFilePath().toStdString() );

    if ( M_player_type_dialog )
    {
        M_player_type_dialog->updateData();
    }

    if ( M_debug_message_window )
    {
        M_debug_message_window->clearAll();
    }

    if ( M_view_config_dialog )
    {
        M_view_config_dialog->unzoom();
    }

    if ( ! M_main_data.viewConfig().anonymousMode() )
    {
        //this->setWindowTitle( file_info.baseName() + tr( " - "PACKAGE_NAME ) );
        QString name = file_info.fileName();
        if ( name.length() > 128 )
        {
            name.replace( 125, name.length() - 125, tr( "..." ) );
        }
        this->setWindowTitle( name + tr( " - "PACKAGE_NAME ) );
    }

    emit viewUpdated();

}

/*-------------------------------------------------------------------*/
/*!

 */
void
MainWindow::saveRCG()
{
    if ( M_main_data.viewHolder().monitorViewCont().empty() )
    {
        QMessageBox::warning( this,
                              tr( "Error" ),
                              tr( "No Monitor View Data!" ) );
        return;
    }

    QString default_file_name;
    {
        const MonitorViewConstPtr latest = M_main_data.viewHolder().latestViewData();

        if ( latest )
        {
            default_file_name
                = QDateTime::currentDateTime().toString( "yyyyMMddhhmm-" );

            QString left_team = QString::fromStdString( latest->leftTeam().name() );
            QString left_score = QString::number( latest->leftTeam().score() );

            QString right_team = QString::fromStdString( latest->rightTeam().name() );
            QString right_score = QString::number( latest->rightTeam().score() );

            default_file_name += left_team;
            default_file_name += tr( "_" );
            default_file_name += left_score;
            default_file_name += tr( "-" );
            default_file_name += right_team;
            default_file_name += tr( "_" );
            default_file_name += right_score;

            default_file_name += tr( ".rcg" );
        }
    }

#ifdef HAVE_LIBRCSC_GZ
    QString filter( tr( "Game Log files (*.rcg *.rcg.gz);;"
                        "All files (*)" ) );
#else
    QString filter( tr( "Game Log files (*.rcg);;"
                        "All files (*)" ) );
#endif

    QString default_dir
        = QString::fromStdString( Options::instance().gameLogDir() );
    if ( ! default_file_name.isEmpty() )
    {
        default_dir += tr( "/" );
        default_dir += default_file_name;
    }

    QString file_path = QFileDialog::getSaveFileName( this,
                                                      tr( "Save a game log file as" ),
                                                      default_dir,
                                                      filter );

    if ( file_path.isEmpty() )
    {
        std::cerr << "MainWindow::saveRCG() empty file path" << std::endl;
        return;
    }

    std::string file_path_std = file_path.toStdString();

    std::cerr << "save game log data to the file = [" << file_path_std
              << ']' << std::endl;

    // update game log dir
    QFileInfo file_info( file_path );
    Options::instance().setGameLogDir( file_info.absolutePath().toStdString() );

    // check gzip usability
    bool is_gzip = false;
    if ( file_path_std.length() > 3
         && file_path_std.compare( file_path_std.length() - 3, 3, ".gz" ) == 0 )
    {
#ifdef HAVE_LIBRCSC_GZ
        if ( file_path_std.length() <= 7
             || file_path_std.compare( file_path_std.length() - 4, 4, ".rcg.gz" ) != 0 )
        {
            file_path_std == ".rcg.gz";
        }
        is_gzip = true;
#else
        // erase '.gz'
        file_path_std.erase( file_path_std.length() - 3 );
#endif
    }

    // check the extention string
    if ( ! is_gzip )
    {
        if ( file_path_std.length() <= 4
             || file_path_std.compare( file_path_std.length() - 4, 4, ".rcg" ) != 0 )
        {
            file_path_std += ".rcg";
        }
    }

    M_main_data.saveRCG( file_path_std );
}

/*-------------------------------------------------------------------*/
/*!

 */
void
MainWindow::openDebugView()
{
    std::cerr << "MainWindow::openDebugView()" << std::endl;

    QString default_dir
        = QString::fromStdString( Options::instance().debugLogDir() );
#if 0
    QString dir_path
        = QFileDialog::getExistingDirectory( this,
                                             tr( "Choose a debug view log directory" ),
                                             default_dir );
#endif
    DirSelector selector( this,
                          tr( "Choose a debug view log directory" ),
                          default_dir );

    if ( ! selector.exec() )
    {
        std::cerr << "MainWindow::openDebugView() canceled" << std::endl;
        return;
    }

    QString dir_path = selector.dirPath();

    if ( dir_path.isEmpty() )
    {
        std::cerr << "MainWindow::openDebugView() empty dir path" << std::endl;
        return;
    }

    std::cerr << "open debug view. dir = [" << dir_path.toStdString() << std::endl;

    M_main_data.getViewHolder().openDebugView( dir_path.toStdString() );
}

/*-------------------------------------------------------------------*/
/*!

 */
void
MainWindow::saveDebugView()
{
    std::cerr << "MainWindow::saveDebugView()" << std::endl;
#if 0
    QString dir_path
        = QFileDialog::getExistingDirectory( this,
                                             tr( "Choose a directory to save a debug view logs" ) );

#endif
    DirSelector selector( this,
                          tr( "Choose a debug view log directory" ),
                          QDir::currentPath() );

    if ( ! selector.exec() )
    {
        std::cerr << "MainWindow::openDebugView() canceled" << std::endl;
        return;
    }

    QString dir_path = selector.dirPath();

    if ( dir_path.isEmpty() )
    {
        std::cerr << "MainWindow::openDebugView() empty dir path" << std::endl;
        return;
    }

    std::cerr << "save debug view. dir = [" << dir_path.toStdString() << std::endl;


    M_main_data.viewHolder().saveDebugView( dir_path.toStdString() );
}

/*-------------------------------------------------------------------*/
/*!

 */
void
MainWindow::kickOff()
{
    std::cerr << "MainWindow::kickOff()" << std::endl;
    if ( M_monitor_client
         && M_monitor_client->isConnected() )
    {
        M_monitor_client->sendKickOff();
    }
}

/*-------------------------------------------------------------------*/
/*!

 */
void
MainWindow::setLiveMode()
{
    std::cerr << "MainWindow::setLiveMode()" << std::endl;

    if ( M_monitor_client
         && M_monitor_client->isConnected() )
    {
        M_log_player->setLiveMode();
    }
}

/*-------------------------------------------------------------------*/
/*!

 */
void
MainWindow::connectMonitor()
{
    std::cerr << "MainWindow::connectMonitor()" << std::endl;
    connectMonitorTo( "localhost" );
}

/*-------------------------------------------------------------------*/
/*!

 */
void
MainWindow::connectMonitorTo()
{
    std::cerr << "MainWindow::connectMonitorTo()" << std::endl;

    if ( M_last_connected_host.isEmpty() )
    {
        M_last_connected_host = "127.0.0.1";
    }

    bool ok = true;
    QString text = QInputDialog::getText( this,
                                          tr( "Input sserver host name" ),
                                          tr( "Host name: "),
                                          QLineEdit::Normal,
                                          M_last_connected_host,
                                          & ok );
    if ( ok
         && ! text.isEmpty() )
    {
        connectMonitorTo( text.toStdString().c_str() );
    }
}

/*-------------------------------------------------------------------*/
/*!

 */
void
MainWindow::connectMonitorTo( const char * hostname )
{
    if ( std::strlen( hostname ) == 0 )
    {
        std::cerr << "Empty host name! Connection failed!" << std::endl;
        return;
    }

    std::cerr << "Connect to rcssserver on [" << hostname << "]" << std::endl;

    M_monitor_client
        = boost::shared_ptr< MonitorClient >
        ( new MonitorClient( M_main_data.getViewHolder(),
                             hostname,
                             Options::instance().port(),
                             Options::instance().clientVersion() ) );

    if ( ! M_monitor_client->isConnected() )
    {
        std::cerr << "Conenction failed." << std::endl;
        M_monitor_client.reset();
        return;
    }

    M_last_connected_host = hostname;

    // reset all data
    M_main_data.clear();

    if ( M_player_type_dialog )
    {
        M_player_type_dialog->hide();
    }

    if ( M_debug_message_window )
    {
        M_debug_message_window->clearAll();
    }

    if ( M_view_config_dialog )
    {
        M_view_config_dialog->unzoom();
    }

    if ( M_debug_server )
    {
        M_debug_server.reset();
        startDebugServer();
    }

    Options::instance().setMonitorClientMode( true );

    M_save_rcg_act->setEnabled( false );

    M_kick_off_act->setEnabled( true );
    M_set_live_mode_act->setEnabled( true );
    M_connect_monitor_act->setEnabled( false );
    M_connect_monitor_to_act->setEnabled( false );
    M_disconnect_monitor_act->setEnabled( true );
#ifndef Q_WS_WIN
    M_kill_server_act->setEnabled( true );
#endif
    M_toggle_drag_move_mode_act->setEnabled( true );
    //M_show_monitor_move_dialog_act->setEnabled( true );

    M_toggle_debug_server_act->setEnabled( true );
    M_show_image_save_dialog_act->setEnabled( false );

    connect( M_monitor_client.get(), SIGNAL( received() ),
             this, SLOT( receiveMonitorPacket() ) );
    connect( M_monitor_client.get(), SIGNAL( timeout() ),
             this, SLOT( disconnectMonitor() ) );

    M_log_player->setLiveMode();

    M_monitor_client->sendDispInit();

    if ( QApplication::overrideCursor() )
    {
        QApplication::restoreOverrideCursor();
    }

    this->setWindowTitle( tr( PACKAGE_NAME ) );
}

/*-------------------------------------------------------------------*/
/*!

 */
void
MainWindow::disconnectMonitor()
{
    //std::cerr << "MainWindow::disconnectMonitor()" << std::endl;
    if ( M_monitor_client )
    {
        M_monitor_client->disconnect();

        disconnect( M_monitor_client.get(), SIGNAL( received() ),
                    this, SLOT( receiveMonitorPacket() ) );

        disconnect( M_monitor_client.get(), SIGNAL( timeout() ),
                    this, SLOT( disconnectMonitor() ) );

        M_monitor_client.reset();
    }

    if ( M_debug_server )
    {
        M_debug_server.reset();
    }

    Options::instance().setMonitorClientMode( false );

    M_save_rcg_act->setEnabled( true );

    M_kick_off_act->setEnabled( false );
    M_set_live_mode_act->setEnabled( false );
    M_connect_monitor_act->setEnabled( true );
    M_connect_monitor_to_act->setEnabled( true );
    M_disconnect_monitor_act->setEnabled( false );
#ifndef Q_WS_WIN
    M_kill_server_act->setEnabled( false );
#endif
    M_toggle_drag_move_mode_act->setEnabled( false );
    //M_show_monitor_move_dialog_act->setEnabled( false );

    M_toggle_debug_server_act->setEnabled( false );
    M_show_image_save_dialog_act->setEnabled( true );
}

/*-------------------------------------------------------------------*/
/*!

 */
// void
// MainWindow::toggleMenuBar()
// {
//     this->menuBar()->setVisible( ! this->menuBar()->isVisible() );
// }

/*-------------------------------------------------------------------*/
/*!

 */
void
MainWindow::toggleToolBar()
{
    M_log_player_tool_bar->setVisible( ! M_log_player_tool_bar->isVisible() );
    //M_monitor_tool_bar->setVisible( ! M_monitor_tool_bar->isVisible() );
}

/*-------------------------------------------------------------------*/
/*!

 */
void
MainWindow::toggleStatusBar()
{
    this->statusBar()->setVisible( ! this->statusBar()->isVisible() );
}

/*-------------------------------------------------------------------*/
/*!

 */
void
MainWindow::toggleFullScreen()
{
    if ( this->isFullScreen() )
    {
        this->showNormal();
    }
    else
    {
        this->showFullScreen();
    }
}

/*-------------------------------------------------------------------*/
/*!

 */
void
MainWindow::showPlayerTypeDialog()
{
    //std::cerr << "MainWindow::showPlayerTypeDialog()" << std::endl;

    if ( M_player_type_dialog )
    {
        M_player_type_dialog->setVisible( ! M_player_type_dialog->isVisible() );
    }
    else
    {
        M_player_type_dialog = new PlayerTypeDialog( this, M_main_data );
        M_player_type_dialog->show();
    }
}

/*-------------------------------------------------------------------*/
/*!

 */
void
MainWindow::showMonitorMoveDialog()
{
    if ( M_monitor_move_dialog )
    {
        M_monitor_move_dialog->setVisible( ! M_monitor_move_dialog->isVisible() );
    }
    else
    {
        M_monitor_move_dialog
            = new MonitorMoveDialog( this,
                                     M_main_data,
                                     M_main_data.getTrainerData() );
        connect( M_monitor_move_dialog, SIGNAL( executed() ),
                 this, SLOT( moveObjects() ) );

        M_monitor_move_dialog->show();
    }
}

/*-------------------------------------------------------------------*/
/*!

 */
void
MainWindow::showDetailDialog()
{
    //std::cerr << "MainWindow::showDetailDialog()" << std::endl;

    if ( M_detail_dialog )
    {
        M_detail_dialog->setVisible( ! M_detail_dialog->isVisible() );
    }
    else
    {
        M_detail_dialog = new DetailDialog( this, M_main_data );
        connect( this,  SIGNAL( viewUpdated() ),
                 M_detail_dialog, SLOT( updateLabels() ) );

        M_detail_dialog->show();
    }
}

/*-------------------------------------------------------------------*/
/*!

 */
void
MainWindow::changeStyle( bool checked )
{
    if ( ! checked )
    {
        return;
    }

    QAction * action = qobject_cast< QAction * >( sender() );
    QStyle * style = QStyleFactory::create( action->data().toString() );
    Q_ASSERT( style );

    QApplication::setStyle( style );
}

/*-------------------------------------------------------------------*/
/*!

 */
void
MainWindow::showColorSettingDialog()
{
    ColorSettingDialog dlg( this );

    connect( &dlg, SIGNAL( colorChanged() ),
             M_field_canvas, SLOT( redrawAll() ) );

    dlg.exec();
}

/*-------------------------------------------------------------------*/
/*!

 */
void
MainWindow::showFontSettingDialog()
{
    FontSettingDialog dlg( this );

    connect( &dlg, SIGNAL( fontChanged() ),
             M_field_canvas, SLOT( redrawAll() ) );

    dlg.exec();
}

/*-------------------------------------------------------------------*/
/*!

 */
void
MainWindow::showViewConfigDialog()
{
    //std::cerr << "MainWindow::showViewConfigDialog()" << std::endl;

    M_view_config_dialog->setVisible( ! M_view_config_dialog->isVisible() );
}

/*-------------------------------------------------------------------*/
/*!

 */
void
MainWindow::showDebugMessageWindow()
{
    //std::cerr << "MainWindow::showDebugMessageWindow()" << std::endl;

    if ( M_debug_message_window )
    {
        M_debug_message_window
            ->setVisible( ! M_debug_message_window->isVisible() );
    }
    else
    {
        M_debug_message_window = new DebugMessageWindow( this,
                                                         M_main_data );
        connect( M_debug_message_window, SIGNAL( configured() ),
                 this, SIGNAL( viewUpdated() ) );

        M_debug_message_window->show();
    }
}

/*-------------------------------------------------------------------*/
/*!

 */
void
MainWindow::about()
{
    QString msg( tr( PACKAGE_NAME"-"VERSION"\n\n" ) );
    msg += tr( "The RoboCup Soccer Simulator LogPlayer(rcsslogplayer)\n"
               "is a game log replay tool for the RoboCup Soccer Siulator"
               "Server (rcssserver).\n"
               "the RoboCup Soccer Simulator.\n"
               "  http://sserver.sourceforge.net/\n"
               "Author:\n"
               "  The RoboCup Soccer Simulator Maintenance Committee.\n"
               "  <sserver-admin@lists.sourceforgenet>" );

    QMessageBox::about( this,
                        tr( "About "PACKAGE_NAME ),
                        msg );

    // from Qt 4.1 documents
    /*
      about() looks for a suitable icon in four locations:

      1. It prefers parent->icon() if that exists.
      2. If not, it tries the top-level widget containing parent.
      3. If that fails, it tries the active window.
      4. As a last resort it uses the Information icon.

      The about box has a single button labelled "OK".
    */
}

/*-------------------------------------------------------------------*/
/*!

 */
void
MainWindow::resizeCanvas( const QSize & size )
{
    if ( size.width() < this->minimumWidth()
         || size.height() < this->minimumHeight() )
    {
        std::cerr << "Too small canvas size ("
                  << size.width() << " "
                  << size.height() << ")"
                  << std::endl;
        return;
    }

    if ( centralWidget() )
    {
        if ( this->isMaximized()
             || this->isFullScreen() )
        {
            this->showNormal();
        }

        QRect rect = this->geometry();

        int width_diff = rect.width() - centralWidget()->width();
        int height_diff = rect.height() - centralWidget()->height();

        rect.setWidth( size.width() + width_diff );
        rect.setHeight( size.height() + height_diff );

        this->setGeometry( rect );

        //std::cerr << "centralWidget width = " << centralWidget()->width()
        //          << " height = " << centralWidget()->height()
        //          << std::endl;
    }
}

/*-------------------------------------------------------------------*/
/*!

 */
void
MainWindow::receiveMonitorPacket()
{
    //std::cerr << "receive monitor packet" << std::endl;

    if ( M_log_player->isLiveMode() )
    {
        M_log_player->showLive();
    }
    else
    {
        std::cerr << "receive monitor packet  no live" << std::endl;
        M_log_player_tool_bar->updateSlider();
    }


    if ( Options::instance().autoQuitMode() )
    {
        if ( M_main_data.viewHolder().lastPlayMode() == rcsc::PM_TimeOver )
        {
            static QDateTime s_game_end_time;

            if ( ! s_game_end_time.isValid() )
            {
                s_game_end_time = QDateTime::currentDateTime();
            }
            else
            {
                if ( s_game_end_time.secsTo( QDateTime::currentDateTime() )
                     >= Options::instance().waitSeconds() )
                {
                    std::cerr << "Elapsed " << Options::instance().waitSeconds()
                              << " seconds after game end\n"
                              << "Exit..."
                              << std::endl;
                    qApp->quit();
                }
            }
        }
    }
}

/*-------------------------------------------------------------------*/
/*!

 */
void
MainWindow::updatePositionLabel( const QPoint & point )
{
    if ( M_position_label
         && M_field_canvas
         && statusBar()
         && statusBar()->isVisible()
         )
    {
        double x = M_field_canvas->fieldX( point.x() );
        double y = M_field_canvas->fieldY( point.y() );

        char buf[32];
        std::snprintf( buf, 32,
                       "(%.2f, %.2f)",
                       x, y );

        M_position_label->setText( QString::fromAscii( buf ) );
    }
}

/*-------------------------------------------------------------------*/
/*!

 */
void
MainWindow::dropBallThere()
{
    if ( M_monitor_client
         && M_monitor_client->isConnected() )
    {
        MonitorViewConstPtr view = M_main_data.viewHolder().latestViewData();
        if ( view )
        {
            std::cerr << "drop ball at current position "
                      << std::endl;
            M_monitor_client->sendDropBall( view->ball().x(),
                                            view->ball().y() );
        }
    }
}

/*-------------------------------------------------------------------*/
/*!

 */
void
MainWindow::dropBall( const QPoint & point )
{
    if ( M_monitor_client
         && M_monitor_client->isConnected() )
    {
        double x = M_main_data.viewConfig().fieldX( point.x() );
        double y = M_main_data.viewConfig().fieldY( point.y() );

        if ( M_main_data.viewConfig().reverseSide() )
        {
            x = -x;
            y = -y;
        }
        std::cerr << "drop ball to ("
                  << x << ", " << y << ")"
                  << std::endl;
        M_monitor_client->sendDropBall( x, y );
    }
}

/*-------------------------------------------------------------------*/
/*!

 */
void
MainWindow::freeKickLeft( const QPoint & point )
{
    if ( M_monitor_client
         && M_monitor_client->isConnected() )
    {
        double x = M_main_data.viewConfig().fieldX( point.x() );
        double y = M_main_data.viewConfig().fieldY( point.y() );

        if ( M_main_data.viewConfig().reverseSide() )
        {
            x = -x;
            y = -y;
            M_monitor_client->sendFreeKickRight( x, y );
        }
        else
        {
            //std::cerr << "free kick left at ("
            //          << x << ", " << y << ")"
            //          << std::endl;
            M_monitor_client->sendFreeKickLeft( x, y );
        }
    }
}

/*-------------------------------------------------------------------*/
/*!

 */
void
MainWindow::freeKickRight( const QPoint & point )
{
    if ( M_monitor_client
         && M_monitor_client->isConnected() )
    {
        double x = M_main_data.viewConfig().fieldX( point.x() );
        double y = M_main_data.viewConfig().fieldY( point.y() );

        if ( M_main_data.viewConfig().reverseSide() )
        {
            x = -x;
            y = -y;
            M_monitor_client->sendFreeKickLeft( x, y );
        }
        else
        {
            //std::cerr << "free kick right at ("
            //          << x << ", " << y << ")"
            //          << std::endl;
            M_monitor_client->sendFreeKickRight( x, y );
        }
    }
}

/*-------------------------------------------------------------------*/
/*!

 */
void
MainWindow::movePlayer( const QPoint & point )
{
    if ( M_monitor_client
         && M_monitor_client->isConnected() )
    {
        rcsc::SideID side = M_main_data.trainerData().draggedPlayerSide();
        int unum = M_main_data.trainerData().draggedPlayerNumber();

        if ( side != rcsc::NEUTRAL
             && unum != 0 )
        {
            double x = M_main_data.viewConfig().fieldX( point.x() );
            double y = M_main_data.viewConfig().fieldY( point.y() );
            if ( M_main_data.viewConfig().reverseSide() )
            {
                x = -x;
                y = -y;
            }

            //std::cerr << "move player to " << x << ' ' << y << std::endl;
            M_main_data.getTrainerData().unsetDrag();
            M_monitor_client->sendMove( side, unum, x, y, 0.0 );
        }
    }
}

/*-------------------------------------------------------------------*/
/*!

 */
void
MainWindow::moveObjects()
{
    if ( ! M_monitor_client
         || ! M_monitor_client->isConnected() )
    {
        return;
    }

    TrainerData & data = M_main_data.getTrainerData();

    // ball
    if ( data.ballPos().valid() )
    {
        if ( rcsc::ServerParam::i().coachMode()
             || rcsc::ServerParam::i().coachWithRefereeMode() )
        {
            if ( data.ballVel().valid() )
            {
                M_monitor_client->sendTrainerMoveBall( data.ballPos().x,
                                                       data.ballPos().y,
                                                       data.ballVel().x,
                                                       data.ballVel().y );
            }
            else
            {
                M_monitor_client->sendTrainerMoveBall( data.ballPos().x,
                                                       data.ballPos().y );
            }
        }
        else
        {
            if ( data.playMode() == rcsc::PM_FreeKick_Left )
            {
                M_monitor_client->sendFreeKickLeft( data.ballPos().x,
                                                    data.ballPos().y );
            }
            else if ( data.playMode() == rcsc::PM_FreeKick_Right )
            {
                M_monitor_client->sendFreeKickRight( data.ballPos().x,
                                                     data.ballPos().y );
            }
            else
            {
                M_monitor_client->sendDropBall( data.ballPos().x,
                                                data.ballPos().y );
            }
        }
    }

    // left
    for ( int unum = 1; unum <= 11; ++unum )
    {
        rcsc::Vector2D pos = data.playerMovePos( rcsc::LEFT, unum );
        rcsc::AngleDeg body = data.playerBody( rcsc::LEFT, unum );
        if ( pos.valid() )
        {
            M_monitor_client->sendMove( rcsc::LEFT, unum,
                                        pos.x, pos.y,
                                        body.degree() );
        }
    }

    // right
    for ( int unum = 1; unum <= 11; ++unum )
    {
        rcsc::Vector2D pos = data.playerMovePos( rcsc::RIGHT, unum );
        rcsc::AngleDeg body = data.playerBody( rcsc::RIGHT, unum );
        if ( pos.valid() )
        {
            M_monitor_client->sendMove( rcsc::RIGHT, unum,
                                        pos.x, pos.y,
                                        body.degree() );
        }
    }

}
