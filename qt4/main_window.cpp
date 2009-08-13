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

#include "detail_dialog.h"
#include "player_type_dialog.h"
#include "config_dialog.h"
#include "field_canvas.h"
#include "monitor_client.h"
#include "monitor_server.h"
#include "image_save_dialog.h"
#include "log_player.h"
#include "log_player_tool_bar.h"
#include "log_slider_tool_bar.h"
#include "score_board_painter.h"
#include "options.h"

#include <string>
#include <iostream>
#include <cstring>

#include "icons/rcss.xpm"

#include "icons/open.xpm"

#ifndef PACKAGE_NAME
#define PACKAGE_NAME "rcsslogplayer"
#endif

#define USE_MONITOR_CLIENT

/*-------------------------------------------------------------------*/
/*!

 */
MainWindow::MainWindow()
    : M_window_style( "plastique" )
    , M_game_log_path()
    , M_log_player( new LogPlayer( M_main_data, this ) )
    , M_config_dialog( static_cast< ConfigDialog * >( 0 ) )
    , M_detail_dialog( static_cast< DetailDialog * >( 0 ) )
    , M_player_type_dialog( static_cast< PlayerTypeDialog * >( 0 ) )
    , M_monitor_server( static_cast< MonitorServer * >( 0 ) )
    , M_monitor_client( static_cast< MonitorClient * >( 0 ) )
    , M_monitor_process( static_cast< QProcess * >( 0 ) )
{
    readSettings();

    createActions();
    createMenus();
    createToolBars();
    createStatusBar();
    // central widget
    createFieldCanvas();
    // control dialogs
    createConfigDialog();

    connect( M_field_canvas, SIGNAL( playerSelected( int ) ),
             M_config_dialog, SLOT( selectPlayer( int ) ) );
    connect( M_field_canvas, SIGNAL( focusChanged( const QPoint & ) ),
             M_config_dialog, SLOT( setFocusPoint( const QPoint & ) ) );

    connect( M_log_player, SIGNAL( updated() ),
             this, SIGNAL( viewUpdated() ) );
    connect( M_log_player, SIGNAL( updated() ),
             this, SLOT( outputCurrentData() ) );

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
        M_log_player_tool_bar->hide();
        M_log_slider_tool_bar->hide();
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
    writeSettings();
}

/*-------------------------------------------------------------------*/
/*!

 */
void
MainWindow::init()
{
    if ( Options::instance().minimumMode() )
    {
        Options::instance().toggleMinimumMode();
        toggleFieldCanvas();
    }

    if ( ! Options::instance().gameLogFile().empty() )
    {
        openRCG( QString::fromStdString( Options::instance().gameLogFile() ) );
    }
    else if ( Options::instance().connect() )
    {
        std::string host = Options::instance().serverHost();
        if ( host.empty() )
        {
            host = "127.0.0.1";
        }

        connectMonitorTo( host.c_str() );
    }

    if ( Options::instance().minimumMode() )
    {
        //
    }
    else if ( Options::instance().fullScreen() )
    {
        this->showFullScreen();
    }
    else if ( Options::instance().maximize() )
    {
        this->showMaximized();
    }
    else if ( Options::instance().canvasWidth() > 0
              && Options::instance().canvasHeight() > 0 )
    {
        resizeCanvas( QSize( Options::instance().canvasWidth(),
                             Options::instance().canvasHeight() ) );
    }

    if ( QApplication::setStyle( M_window_style ) ) // no style found
    {
        Q_FOREACH( QAction * action, M_style_act_group->actions() )
        {
            if ( action->data().toString().toLower()
                 == QApplication::style()->objectName().toLower() )
            {
                M_window_style = QApplication::style()->objectName().toLower();
                action->setChecked( true );
                break;
            }
        }
    }

    if ( ! Options::instance().monitorPath().empty()
         && Options::instance().monitorPath() != "self" )
    {
        QTimer::singleShot( 100,
                            this, SLOT( startMonitor() ) );
    }
}

/*-------------------------------------------------------------------*/
/*!

 */
void
MainWindow::readSettings()
{
#ifndef Q_WS_WIN
    QSettings settings( QDir::homePath() + "/.rcsslogplayer",
                        QSettings::IniFormat );
#else
    QSettings settings( QDir::currentPath() + "/rcsslogplayer.ini",
                        QSettings::IniFormat );
#endif

    settings.beginGroup( "Global" );

    QVariant val;

    M_window_style = settings.value( "window_style", "plastique" ).toString();
    M_game_log_path = settings.value( "game_log_path", "" ).toString();

    settings.endGroup();
}

/*-------------------------------------------------------------------*/
/*!

 */
void
MainWindow::writeSettings()
{
#ifndef Q_WS_WIN
    QSettings settings( QDir::homePath() + "/.rcsslogplayer",
                        QSettings::IniFormat );
#else
    QSettings settings( QDir::currentPath() + "/rcsslogplayer.ini",
                        QSettings::IniFormat );
#endif

    settings.beginGroup( "Global" );

    settings.setValue( "window_style", M_window_style );
    settings.setValue( "game_log_path", M_game_log_path );

    if ( ! Options::instance().minimumMode() )
    {
        settings.setValue( "window_width", this->width() );
        settings.setValue( "window_height", this->height() );
        settings.setValue( "window_x", this->pos().x() );
        settings.setValue( "window_y", this->pos().y() );
        settings.setValue( "maximize", this->isMaximized() );
        settings.setValue( "full_screen", this->isFullScreen() );
        settings.setValue( "hide_menu_bar", this->menuBar()->isHidden() );
        settings.setValue( "hide_tool_bar", ( M_log_player_tool_bar->isHidden()
                                              && M_log_slider_tool_bar->isHidden() ) );
        settings.setValue( "hide_status_bar", this->statusBar()->isHidden() );
    }

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
    createActionsHelp();

}

/*-------------------------------------------------------------------*/
/*!

 */
void
MainWindow::createActionsFile()
{
    M_open_act = new QAction( QIcon( QPixmap( open_xpm ) ),
                              tr( "&Open rcg file..." ), this );
#ifdef Q_WS_MAC
    M_open_act->setShortcut( Qt::META + Qt::Key_O );
#else
    M_open_act->setShortcut( Qt::CTRL + Qt::Key_O );
#endif
    M_open_act->setStatusTip( tr( "Open RoboCup Game Log file." ) );
    connect( M_open_act, SIGNAL( triggered() ),
             this, SLOT( openRCG() ) );
    this->addAction( M_open_act );
    //
    M_open_output_act = new QAction( //QIcon( QPixmap( open_xpm ) ),
                                    tr( "Record data as..." ), this );
    M_open_output_act->setEnabled( false );
#ifdef Q_WS_MAC
    M_open_output_act->setShortcut( Qt::META + Qt::ALT + Qt::Key_O );
#else
    M_open_output_act->setShortcut( Qt::CTRL + Qt::ALT + Qt::Key_O );
#endif
    M_open_output_act->setStatusTip( tr( "Open the output file to record log data segments." ) );
    connect( M_open_output_act, SIGNAL( triggered() ),
             this, SLOT( openOutputFile() ) );
    this->addAction( M_open_output_act );
    //
    M_save_image_act = new QAction( tr( "Save images" ), this );
    M_save_image_act->setEnabled( false );
    M_save_image_act->setStatusTip( tr( "Open the dialog to save field status as image data." ) );
    connect( M_save_image_act, SIGNAL( triggered() ),
             this, SLOT( saveImage() ) );
    this->addAction( M_save_image_act );

    //
    M_exit_act = new QAction( tr( "&Quit" ), this );
#ifdef Q_WS_MAC
    M_exit_act->setShortcut( Qt::META + Qt::Key_Q );
#else
    M_exit_act->setShortcut( Qt::CTRL + Qt::Key_Q );
#endif
    M_exit_act->setStatusTip( tr( "Exit the application." ) );
    connect( M_exit_act, SIGNAL( triggered() ),
             this, SLOT( close() ) );
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
    M_kick_off_act->setShortcut( Qt::META + Qt::Key_K );
#else
    M_kick_off_act->setShortcut( Qt::CTRL + Qt::Key_K );
#endif
    M_kick_off_act->setStatusTip( tr( "Send kick-off command to the connected rcssserver." ) );
    M_kick_off_act->setEnabled( false );
#ifdef USE_MONITOR_CLIENT
    connect( M_kick_off_act, SIGNAL( triggered() ),
             this, SLOT( kickOff() ) );
    this->addAction( M_kick_off_act );
#endif
    //
    M_set_live_mode_act = new QAction( //QIcon( QPixmap( logplayer_live_mode_xpm ) ),
                                      tr( "&Live Mode" ),
                                      this );
#ifdef Q_WS_MAC
    M_set_live_mode_act->setShortcut( Qt::META + Qt::Key_L );
#else
    M_set_live_mode_act->setShortcut( Qt::CTRL + Qt::Key_L );
#endif
    M_set_live_mode_act->setStatusTip( tr( "Set monitor to the live mode" ) );
    M_set_live_mode_act->setEnabled( false );
#ifdef USE_MONITOR_CLIENT
    connect( M_set_live_mode_act, SIGNAL( triggered() ),
             this, SLOT( setLiveMode() ) );
    this->addAction( M_set_live_mode_act );
#endif
    //
    M_connect_monitor_act = new QAction( tr( "&Connect" ), this );
#ifdef Q_WS_MAC
    M_connect_monitor_act->setShortcut( Qt::META + Qt::Key_C );
#else
    M_connect_monitor_act->setShortcut( Qt::CTRL + Qt::Key_C );
#endif
    M_connect_monitor_act
        ->setStatusTip( "Connect to the rcssserver on localhost" );
    M_connect_monitor_act->setEnabled( true );
#ifdef USE_MONITOR_CLIENT
    connect( M_connect_monitor_act, SIGNAL( triggered() ),
             this, SLOT( connectMonitor() ) );
    this->addAction( M_connect_monitor_act );
#endif
    //
    M_connect_monitor_to_act = new QAction( tr( "Connect &to ..." ), this );
    M_connect_monitor_to_act
        ->setStatusTip( tr( "Connect to the rcssserver on the remote host." ) );
    M_connect_monitor_to_act->setEnabled( true );
    connect( M_connect_monitor_to_act, SIGNAL( triggered() ),
             this, SLOT( connectMonitorTo() ) );
    this->addAction( M_connect_monitor_to_act );
    //
    M_disconnect_monitor_act = new QAction( tr( "&Disconnect" ), this );
    M_disconnect_monitor_act->setStatusTip( tr( "Disonnect from rcssserver." ) );
    M_disconnect_monitor_act->setEnabled( false );
#ifdef USE_MONITOR_CLIENT
    connect( M_disconnect_monitor_act, SIGNAL( triggered() ),
             this, SLOT( disconnectMonitor() ) );
    this->addAction( M_disconnect_monitor_act );
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
    M_toggle_menu_bar_act->setShortcut( Qt::META + Qt::Key_M );
#else
    M_toggle_menu_bar_act->setShortcut( Qt::CTRL + Qt::Key_M );
#endif
    M_toggle_menu_bar_act->setStatusTip( tr( "Show/Hide menu bar." ) );
    connect( M_toggle_menu_bar_act, SIGNAL( triggered() ),
             this, SLOT( toggleMenuBar() ) );
    this->addAction( M_toggle_menu_bar_act );
    //
    M_toggle_tool_bar_act = new QAction( tr( "&Tool Bar" ), this );
    M_toggle_tool_bar_act->setStatusTip( tr( "Show/Hide tool bar." ) );
    connect( M_toggle_tool_bar_act, SIGNAL( triggered() ),
             this, SLOT( toggleToolBar() ) );
    this->addAction( M_toggle_tool_bar_act );
    //
    M_toggle_status_bar_act = new QAction( tr( "&Status Bar" ), this );
    M_toggle_status_bar_act->setStatusTip( tr( "Show/Hide status bar." ) );
    connect( M_toggle_status_bar_act, SIGNAL( triggered() ),
             this, SLOT( toggleStatusBar() ) );
    this->addAction( M_toggle_status_bar_act );
    //
    M_toggle_field_canvas_act = new QAction( tr( "Minimum" ), this );
    M_toggle_field_canvas_act->setStatusTip( tr( "Show/Hide field." ) );
    connect( M_toggle_field_canvas_act, SIGNAL( triggered() ),
             this, SLOT( toggleFieldCanvas() ) );
    this->addAction( M_toggle_field_canvas_act );
    //
    M_full_screen_act = new QAction( tr( "&Full Screen" ), this );
    M_full_screen_act->setShortcut( tr( "F11" ) );
    //M_full_screen_act->setShortcut( tr( "Alt+Enter" ) );
    //M_full_screen_act->setShortcut( Qt::ALT + Qt::Key_Return );
    //M_full_screen_act->setShortcut( Qt::ALT + Qt::Key_Enter );
    M_full_screen_act->setStatusTip( tr( "Toggle full screen." ) );
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
    M_show_player_type_dialog_act->setShortcut( Qt::META + Qt::Key_H );
#else
    M_show_player_type_dialog_act->setShortcut( Qt::CTRL + Qt::Key_H );
#endif
    M_show_player_type_dialog_act
        ->setStatusTip( tr( "Show player type parameter dialog." ) );
    connect( M_show_player_type_dialog_act, SIGNAL( triggered() ),
             this, SLOT( showPlayerTypeDialog() ) );
    this->addAction( M_show_player_type_dialog_act );
    //
    M_show_detail_dialog_act = new QAction( tr( "Object &Detail" ), this );
#ifdef Q_WS_MAC
    M_show_detail_dialog_act->setShortcut( Qt::META + Qt::Key_D );
#else
    M_show_detail_dialog_act->setShortcut( Qt::CTRL + Qt::Key_D );
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
    M_show_config_dialog_act = new QAction( tr( "&Config" ), this );
#ifdef Q_WS_MAC
    M_show_config_dialog_act->setShortcut( tr( "Meta+V" ) );
#else
    M_show_config_dialog_act->setShortcut( tr( "Ctrl+V" ) );
#endif
    M_show_config_dialog_act->setStatusTip( tr( "Show config dialog." ) );
    connect( M_show_config_dialog_act, SIGNAL( triggered() ),
             this, SLOT( showConfigDialog() ) );
    this->addAction( M_show_config_dialog_act );
}

/*-------------------------------------------------------------------*/
/*!

 */
void
MainWindow::createActionsHelp()
{
    M_about_act = new QAction( QIcon( QPixmap( rcss_xpm ) ),
                               tr( "&About" ), this );
    M_about_act->setStatusTip( tr( "Show the about dialog." ) );
    connect( M_about_act, SIGNAL( triggered() ), this, SLOT( about() ) );
    this->addAction( M_about_act );

    M_shortcut_keys_act = new QAction( tr( "Shortcut Keys" ), this );
    M_shortcut_keys_act->setStatusTip( tr( "Print available shortcut keys." ) );
    connect( M_shortcut_keys_act, SIGNAL( triggered() ),
             this, SLOT( printShortcutKeys() ) );
    this->addAction( M_shortcut_keys_act );
}

/*-------------------------------------------------------------------*/
/*!

 */
void
MainWindow::createMenus()
{
    createMenuFile();
    createMenuView();
#ifdef USE_MONITOR_CLIENT
    createMenuMonitor();
#endif
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
    menu->addAction( M_open_output_act );

    menu->addSeparator();
    menu->addAction( M_save_image_act );

    menu->addSeparator();
    menu->addAction( M_exit_act );
}

/*-------------------------------------------------------------------*/
/*!

 */
void
MainWindow::createMenuMonitor()
{
#ifdef USE_MONITOR_CLIENT
    QMenu * menu = menuBar()->addMenu( tr( "&Monitor" ) );

    menu->addAction( M_kick_off_act );
    menu->addAction( M_set_live_mode_act );

    menu->addSeparator();
    menu->addAction( M_connect_monitor_act );
    menu->addAction( M_connect_monitor_to_act );
    menu->addAction( M_disconnect_monitor_act );
#endif
}

/*-------------------------------------------------------------------*/
/*!

 */
void
MainWindow::createMenuView()
{
    QMenu * menu = menuBar()->addMenu( tr( "&View" ) );

    menu->addAction( M_toggle_field_canvas_act );

    menu->addSeparator();

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
    menu->addAction( M_show_config_dialog_act );
}

/*-------------------------------------------------------------------*/
/*!

 */
void
MainWindow::createMenuHelp()
{
    QMenu * menu = menuBar()->addMenu( tr( "&Help" ) );
    menu->addAction( M_about_act );
    menu->addAction( M_shortcut_keys_act );

    menu->addSeparator();
    menu->addAction( tr( "About Qt" ), qApp, SLOT( aboutQt() ) );
}

/*-------------------------------------------------------------------*/
/*!

 */
void
MainWindow::createToolBars()
{
    M_log_player_tool_bar = new LogPlayerToolBar( M_log_player,
                                                  M_main_data,
                                                  this );
    connect( M_log_player_tool_bar, SIGNAL( recordToggled( bool ) ),
             this, SLOT( toggleRecord( bool ) ) );

    this->addToolBar( Qt::TopToolBarArea, M_log_player_tool_bar );

    //

    M_log_slider_tool_bar = new LogSliderToolBar( M_log_player,
                                                  M_main_data,
                                                  this );

    connect( this,  SIGNAL( viewUpdated() ),
             M_log_slider_tool_bar, SLOT( updateSlider() ) );

    this->addToolBar( Qt::TopToolBarArea, M_log_slider_tool_bar );
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
    this->setCentralWidget( M_field_canvas );

    M_field_canvas->setFocus();

    connect( this, SIGNAL( viewUpdated() ),
             M_field_canvas, SLOT( update() ) );

    connect( M_field_canvas, SIGNAL( mouseMoved( const QPoint & ) ),
             this, SLOT( updatePositionLabel( const QPoint & ) ) );

    connect( M_field_canvas, SIGNAL( dropBall( const QPoint & ) ),
             this, SLOT( dropBall( const QPoint & ) ) );
    connect( M_field_canvas, SIGNAL( freeKickLeft( const QPoint & ) ),
             this, SLOT( freeKickLeft( const QPoint & ) ) );
    connect( M_field_canvas, SIGNAL( freeKickRight( const QPoint & ) ),
             this, SLOT( freeKickRight( const QPoint & ) ) );

    // create & set context menus
    {
        QMenu * menu = new QMenu( M_field_canvas );
        menu->addAction( M_open_act );
#ifdef USE_MONITOR_CLIENT
        menu->addAction( M_connect_monitor_act );
#endif

        M_field_canvas->setNormalMenu( menu );
    }
    {
        QMenu * menu = new QMenu( M_field_canvas );
        menu->addAction( M_open_act );
#ifdef USE_MONITOR_CLIENT
        menu->addAction( M_connect_monitor_act );
#endif

        M_field_canvas->setSystemMenu( menu );
    }
    {
        QMenu * menu = new QMenu( M_field_canvas );
        menu->addAction( M_kick_off_act );
        menu->addSeparator();
        menu->addAction( tr( "Drop Ball" ),
                         M_field_canvas, SLOT( dropBall() ) );
        menu->addAction( tr( "Free Kick Left" ),
                         M_field_canvas, SLOT( freeKickLeft() ) );
        menu->addAction( tr( "Free Kick Right" ),
                         M_field_canvas, SLOT( freeKickRight() ) );
        menu->addSeparator();

        M_field_canvas->setMonitorMenu( menu );
    }
}

/*-------------------------------------------------------------------*/
/*!

 */
void
MainWindow::createConfigDialog()
{
    if ( M_config_dialog )
    {
        return;
    }

    M_config_dialog = new ConfigDialog( this, M_main_data );

    M_config_dialog->hide();

    connect( M_config_dialog, SIGNAL( configured() ),
             this, SIGNAL( viewUpdated() ) );

    connect( M_config_dialog, SIGNAL( canvasResized( const QSize & ) ),
             this, SLOT( resizeCanvas( const QSize & ) ) );

    // register short cut keys
    {
        // z
        QAction * act = new QAction( tr( "ZoomIn" ), this );
        act->setShortcut( Qt::Key_Z );
        act->setStatusTip( tr( "Zoom in." ) );
        this->addAction( act );
        connect( act, SIGNAL( triggered() ),
                 M_config_dialog, SLOT( zoomIn() ) );
    }
    {
        // x
        QAction * act = new QAction( tr( "ZoomOut" ), this );
        act->setShortcut( Qt::Key_X );
        act->setStatusTip( tr( "Zoom out." ) );
        this->addAction( act );
        connect( act, SIGNAL( triggered() ),
                 M_config_dialog, SLOT( zoomOut() ) );
    }
    {
        // Ctrl + z
        QAction * act = new QAction( tr( "ZoomOut" ), this );
#ifdef Q_WS_MAC
        act->setShortcut( Qt::META + Qt::Key_Z );
#else
        act->setShortcut( Qt::CTRL + Qt::Key_Z );
#endif
        act->setStatusTip( tr( "Zoom out." ) );
        this->addAction( act );
        connect( act, SIGNAL( triggered() ),
                 M_config_dialog, SLOT( zoomOut() ) );
    }
    {
        // i
        QAction * act = new QAction( tr( "Fit Field Size" ), this );
        act->setShortcut( Qt::Key_I );
        act->setStatusTip( tr( "Fit field size to the screen." ) );
        this->addAction( act );
        connect( act, SIGNAL( triggered() ),
                 M_config_dialog, SLOT( unzoom() ) );
    }

    // field style
//     {
//         QAction * act = new QAction( tr( "Show Keepaway Area" ), this );
//         act->setStatusTip( tr( "Show keepaway area." ) );
//         this->addAction( act );
//         connect( act, SIGNAL( triggered() ),
//                  M_config_dialog, SLOT( toggleShowKeepawayArea() ) );
//     }

    // player detail
    {
        // n
        QAction * act = new QAction( tr( "Show Player Number" ), this );
        act->setShortcut( Qt::Key_N );
        act->setStatusTip( tr( "Show/Hide player number." ) );
        this->addAction( act );
        connect( act, SIGNAL( triggered() ),
                 M_config_dialog, SLOT( toggleShowPlayerNumber() ) );
    }
    {
        // h
        QAction * act = new QAction( tr( "Show Player Type Id" ), this );
        act->setShortcut( Qt::Key_H );
        act->setStatusTip( tr( "Show/Hide player type id." ) );
        this->addAction( act );
        connect( act, SIGNAL( triggered() ),
                 M_config_dialog, SLOT( toggleShowPlayerType() ) );
    }
    {
        // s
        QAction * act = new QAction( tr( "Show Stamina" ), this );
        act->setShortcut( Qt::Key_S );
        act->setStatusTip( tr( "Show/Hide player's stamina." ) );
        this->addAction( act );
        connect( act, SIGNAL( triggered() ),
                 M_config_dialog, SLOT( toggleShowStamina() ) );
    }
    {
        // Ctrl + s
        QAction * act = new QAction( tr( "Show Stamina Capacity" ), this );
#ifdef Q_WS_MAC
        act->setShortcut( Qt::META + Qt::Key_S );
#else
        act->setShortcut( Qt::CTRL + Qt::Key_S );
#endif
        act->setStatusTip( tr( "Show/Hide player's stamina capacity." ) );
        this->addAction( act );
        connect( act, SIGNAL( triggered() ),
                 M_config_dialog, SLOT( toggleShowStaminaCapacity() ) );
    }
    {
        // v
        QAction * act = new QAction( tr( "Show View Area" ), this );
        act->setShortcut( Qt::Key_V );
        act->setStatusTip( tr( "Show/Hide player's view area." ) );
        this->addAction( act );
        connect( act, SIGNAL( triggered() ),
                 M_config_dialog, SLOT( toggleShowViewArea() ) );
    }
    {
        // c
        QAction * act = new QAction( tr( "Show Catch Area" ), this );
        act->setShortcut( Qt::Key_C );
        act->setStatusTip( tr( "Show/Hide goalie's catchable area." ) );
        this->addAction( act );
        connect( act, SIGNAL( triggered() ),
                 M_config_dialog, SLOT( toggleShowCatchArea() ) );
    }
    {
        // k
        QAction * act = new QAction( tr( "Show Tackle Area" ), this );
        act->setShortcut( Qt::Key_T );
        act->setStatusTip( tr( "Show/Hide player's tackle area if player can tackle the ball." ) );
        this->addAction( act );
        connect( act, SIGNAL( triggered() ),
                 M_config_dialog, SLOT( toggleShowTackleArea() ) );
    }
    {
        // k
        QAction * act = new QAction( tr( "Show Kick Accel Area" ), this );
        act->setShortcut( Qt::Key_K );
        act->setStatusTip( tr( "Show/Hide player's kick accel area if player can kick the ball." ) );
        this->addAction( act );
        connect( act, SIGNAL( triggered() ),
                 M_config_dialog, SLOT( toggleShowKickAccelArea() ) );
    }
    {
        // p
        QAction * act = new QAction( tr( "Show Pointto Point" ), this );
        act->setShortcut( Qt::Key_P );
        act->setStatusTip( tr( "Show/Hide player's pointing position." ) );
        this->addAction( act );
        connect( act, SIGNAL( triggered() ),
                 M_config_dialog, SLOT( toggleShowPointto() ) );
    }

    // show/hide
//     {
//         // Ctrl + s
//         QAction * act = new QAction( tr( "Show Score Board" ), this );
//  #ifdef Q_WS_MAC
//         act->setShortcut( Qt::META + Qt::Key_S );
// #else
//         act->setShortcut( Qt::CTRL + Qt::Key_S );
// #endif
//         this->addAction( act );
//         connect( act, SIGNAL( triggered() ),
//                  M_config_dialog, SLOT( toggleShowScoreBoard() ) );
//     }
    {
        // Ctrl + b
        QAction * act = new QAction( tr( "Show Ball" ), this );
#ifdef Q_WS_MAC
        act->setShortcut( Qt::META + Qt::Key_B );
#else
        act->setShortcut( Qt::CTRL + Qt::Key_B );
#endif
        act->setStatusTip( tr( "Show/Hide ball." ) );
        this->addAction( act );
        connect( act, SIGNAL( triggered() ),
                 M_config_dialog, SLOT( toggleShowBall() ) );
    }
    {
        // Ctrl + p
        QAction * act = new QAction( tr( "Show Players" ), this );
#ifdef Q_WS_MAC
        act->setShortcut( Qt::META + Qt::Key_P );
#else
        act->setShortcut( Qt::CTRL + Qt::Key_P );
#endif
        act->setStatusTip( tr( "Show/Hide players." ) );
        this->addAction( act );
        connect( act, SIGNAL( triggered() ),
                 M_config_dialog, SLOT( toggleShowPlayer() ) );
    }
    {
        // f
        QAction * act = new QAction( tr( "Show Flags" ), this );
#ifdef Q_WS_MAC
        act->setShortcut( Qt::META + Qt::Key_F );
#else
        act->setShortcut( Qt::CTRL + Qt::Key_F );
#endif
        act->setStatusTip( tr( "Show/Hide flags." ) );
        this->addAction( act );
        connect( act, SIGNAL( triggered() ),
                 M_config_dialog, SLOT( toggleShowFlag() ) );
    }
    {
        // o
        QAction * act = new QAction( tr( "Show Offside Line" ), this );
        act->setShortcut( Qt::Key_O );
        act->setStatusTip( tr( "Show/Hide offside lines." ) );
        this->addAction( act );
        connect( act, SIGNAL( triggered() ),
                 M_config_dialog, SLOT( toggleShowOffsideLine() ) );
    }

    // number 1-10
    for ( int i = 0; i < 10; ++i )
    {
        {
            QAction * act = new QAction( QString( "Select Left %1" ).arg( i ), this );
            act->setShortcut( Qt::Key_0 + i );
            act->setStatusTip( QString( "Select left player %1" ).arg( i ) );
            this->addAction( act );
            connect( act, SIGNAL( triggered() ),
                     M_config_dialog, SLOT( selectPlayerWithKey() ) );
        }
        {
            QAction * act = new QAction( QString( "Selct Right %1" ).arg( i ), this );
#ifdef Q_WS_MAC
            act->setShortcut( Qt::META + Qt::Key_0 + i );
#else
            act->setShortcut( Qt::CTRL + Qt::Key_0 + i );
#endif
            act->setStatusTip( QString( "Select right player %1" ).arg( i ) );
            this->addAction( act );
            connect( act, SIGNAL( triggered() ),
                     M_config_dialog, SLOT( selectPlayerWithKey() ) );
        }
    }
    // number 11
    {
        {
            QAction * act = new QAction( tr( "Select Left 11" ), this );
            act->setShortcut( Qt::Key_Minus );
            act->setStatusTip( tr( "Select left player 11" ) );
            this->addAction( act );
            connect( act, SIGNAL( triggered() ),
                     M_config_dialog, SLOT( selectPlayerWithKey() ) );
        }
        {
            QAction * act = new QAction( tr( "Select Right 11" ), this );
#ifdef Q_WS_MAC
            act->setShortcut( Qt::META + Qt::Key_Minus );
#else
            act->setShortcut( Qt::CTRL + Qt::Key_Minus );
#endif
            act->setStatusTip( tr( "Select right player 11" ) );
            this->addAction( act );
            connect( act, SIGNAL( triggered() ),
                     M_config_dialog, SLOT( selectPlayerWithKey() ) );
        }
    }
    // b
    {
        QAction * act = new QAction( tr( "Focus Ball" ), this );
        act->setShortcut( Qt::Key_B );
        act->setStatusTip( tr( "Toggle automatic ball focus mode." ) );
        this->addAction( act );
        connect( act, SIGNAL( triggered() ),
                 M_config_dialog, SLOT( toggleFocusBall() ) );
    }
    // f
    {
        QAction * act = new QAction( tr( "Focus Player" ), this );
        act->setShortcut( Qt::Key_F );
        act->setStatusTip( tr( "Toggle automatic player focus mode." ) );
        this->addAction( act );
        connect( act, SIGNAL( triggered() ),
                 M_config_dialog, SLOT( toggleFocusPlayer() ) );
    }
    // a
    {
        QAction * act = new QAction( tr( "Select auto all" ), this );
        act->setShortcut( Qt::Key_A );
        act->setStatusTip( tr( "Toggle automatic player selection from all players." ) );
        this->addAction( act );
        connect( act, SIGNAL( triggered() ),
                 M_config_dialog, SLOT( toggleSelectAutoAll() ) );
    }
    // l
    {
        QAction * act = new QAction( tr( "Select auto left" ), this );
        act->setShortcut( Qt::Key_L );
        act->setStatusTip( tr( "Toggle automatic player selection from left team." ) );
        this->addAction( act );
        connect( act, SIGNAL( triggered() ),
                 M_config_dialog, SLOT( toggleSelectAutoLeft() ) );
    }
    // r
    {
        QAction * act = new QAction( tr( "Select auto right" ), this );
        act->setShortcut( Qt::Key_R );
        act->setStatusTip( tr( "Toggle automatic player selection from right team." ) );
        this->addAction( act );
        connect( act, SIGNAL( triggered() ),
                 M_config_dialog, SLOT( toggleSelectAutoRight() ) );
    }
    // u
    {
        QAction * act = new QAction( tr( "Unselect" ), this );
        act->setShortcut( Qt::Key_U );
        act->setStatusTip( tr( "Unselect the player." ) );
        this->addAction( act );
        connect( act, SIGNAL( triggered() ),
                 M_config_dialog, SLOT( setUnselect() ) );
    }
}

/*-------------------------------------------------------------------*/
/*!

 */
void
MainWindow::createMonitorServer()
{
    closeMonitorServer();

    M_monitor_server = new MonitorServer( this,
                                          M_main_data,
                                          Options::instance().monitorPort() );

    if ( ! M_monitor_server->isConnected() )
    {
        delete M_monitor_server;
        M_monitor_server = static_cast< MonitorServer * >( 0 );
        return;
    }

    connect( M_log_player, SIGNAL( updated() ),
             M_monitor_server, SLOT( sendToClients() ) );
}

/*-------------------------------------------------------------------*/
/*!

 */
void
MainWindow::closeMonitorServer()
{
    if ( M_monitor_server )
    {
        disconnect( M_log_player, SIGNAL( updated() ),
                    M_monitor_server, SLOT( sendToClients() ) );
        delete M_monitor_server;
        M_monitor_server = static_cast< MonitorServer * >( 0 );
    }
}

/*-------------------------------------------------------------------*/
/*!

 */
void
MainWindow::closeEvent( QCloseEvent * event )
{
    event->ignore();

    qApp->quit();
}

/*-------------------------------------------------------------------*/
/*!

 */
void
MainWindow::resizeEvent( QResizeEvent * event )
{
    event->accept();

    if ( M_config_dialog
         && M_config_dialog->isVisible() )
    {
        M_config_dialog->updateFieldScale();
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
#ifdef HAVE_LIBZ
    QString filter( tr( "Game Log files (*.rcg *.rcg.gz);;"
                        "All files (*)" ) );
#else
    QString filter( tr( "Game Log files (*.rcg);;"
                        "All files (*)" ) );
#endif
    if ( ! Options::instance().gameLogFile().empty() )
    {
        M_game_log_path = QString::fromStdString( Options::instance().gameLogFile() );
    }

    QString file_path = QFileDialog::getOpenFileName( this,
                                                      tr( "Choose a game log file to open" ),
                                                      M_game_log_path,
                                                      filter );

    if ( file_path.isEmpty() )
    {
        //std::cerr << "MainWindow::opneRCG() empty file path" << std::endl;
        return;
    }

    std::cerr << "open file = [" << file_path.toStdString() << ']' << std::endl;

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

    M_log_player->stop();
    disconnectMonitor();
    M_open_output_act->setEnabled( false );
    M_save_image_act->setEnabled( false );
    M_log_player_tool_bar->checkRecord( false );
    M_log_player_tool_bar->enableRecord( false );
    M_main_data.closeOutputFile();

    if ( ! M_main_data.openRCG( file_path, this ) )
    {
        if ( Options::instance().autoQuitMode() )
        {
            std::cerr << "***ERROR*** Failed to read [" << file_path.toStdString()
                      << "]" << std::endl;
            QTimer::singleShot( 100,
                                qApp, SLOT( quit() ) );
        }
        else
        {
            QString err_msg = tr( "Failed to read [" );
            err_msg += file_path;
            err_msg += tr( "]" );
            QMessageBox::critical( this,
                                   tr( "Error" ),
                                   err_msg,
                                   QMessageBox::Ok, QMessageBox::NoButton );
            this->setWindowTitle( tr( PACKAGE_NAME ) );
            this->statusBar()->showMessage( tr( "Ready" ) );
        }
        return;
    }

    if ( M_main_data.dispHolder().dispInfoCont().empty() )
    {
        if ( Options::instance().autoQuitMode() )
        {
            std::cerr << "***ERROR*** Empty log file ["
                      << file_path.toStdString() << "]" << std::endl;
            QTimer::singleShot( 100,
                                qApp, SLOT( quit() ) );
        }
        else
        {
            QString err_msg = tr( "Empty log file [" );
            err_msg += file_path;
            err_msg += tr( "]" );
            QMessageBox::critical( this,
                                   tr( "Error" ),
                                   err_msg,
                                   QMessageBox::Ok, QMessageBox::NoButton );
            this->setWindowTitle( tr( PACKAGE_NAME ) );
            this->statusBar()->showMessage( tr( "Ready" ) );
        }
        return;
    }

    // update last opened file path
    QFileInfo file_info( file_path );
    M_game_log_path = file_info.absoluteFilePath();
    Options::instance().setGameLogFile( M_game_log_path.toStdString() );

    if ( M_player_type_dialog
         && M_player_type_dialog->isVisible() )
    {
        M_player_type_dialog->updateData();
    }

    if ( M_config_dialog )
    {
        M_config_dialog->unzoom();
    }

    // set window title
    QString name = file_info.fileName();
    if ( name.length() > 128 )
    {
        name.replace( 125, name.length() - 125, tr( "..." ) );
    }
    this->setWindowTitle( name + tr( " - "PACKAGE_NAME ) );
    this->statusBar()->showMessage( name );

    createMonitorServer();
    M_open_output_act->setEnabled( true );
    M_save_image_act->setEnabled( true );

    if ( Options::instance().autoQuitMode() )
    {
        M_log_player->playForward();
    }

    emit viewUpdated();
}

/*-------------------------------------------------------------------*/
/*!

 */
void
MainWindow::openOutputFile()
{
    QString filter( tr( "Game Log files (*.rcg);;"
                        "All files (*)" ) );
    QString dir_path;

    if ( ! M_game_log_path.isEmpty() )
    {
        QFileInfo file_info( M_game_log_path );
        dir_path = file_info.absolutePath();
    }

    QString file_path = QFileDialog::getSaveFileName( this,
                                                      tr( "Output game log data to the file" ),
                                                      dir_path,
                                                      filter );

    if ( file_path.isEmpty() )
    {
        //std::cerr << "MainWindow::opneRCG() empty file path" << std::endl;
        return;
    }

    if ( ! file_path.endsWith( ".rcg" ) )
    {
        file_path.append( ".rcg" );
    }

    std::cerr << "output file = [" << file_path.toStdString() << ']' << std::endl;

    if ( M_main_data.openOutputFile( file_path ) )
    {
        M_log_player_tool_bar->enableRecord( true );
    }
}

/*-------------------------------------------------------------------*/
/*!

 */
void
MainWindow::saveImage()
{
    M_log_player->stop();

    ImageSaveDialog dlg( this,
                         M_field_canvas,
                         M_main_data );

    dlg.exec();
}

/*-------------------------------------------------------------------*/
/*!

 */
void
MainWindow::toggleRecord( bool checked )
{
    M_main_data.setEnableRecord( checked );
}

/*-------------------------------------------------------------------*/
/*!

 */
void
MainWindow::outputCurrentData()
{
    M_main_data.outputCurrentData();
}

/*-------------------------------------------------------------------*/
/*!

 */
void
MainWindow::startMonitor()
{
    M_monitor_process = new QProcess( this );

    connect( M_monitor_process, SIGNAL( error( QProcess::ProcessError ) ),
             this, SLOT( printMonitorError( QProcess::ProcessError ) ) );
    connect( M_monitor_process, SIGNAL( finished( int, QProcess::ExitStatus ) ),
             this, SLOT( printMonitorExit( int, QProcess::ExitStatus ) ) );

    QString command = QString::fromStdString( Options::instance().monitorPath() );

    std::cout << "starting monitor \"" << command.toStdString() << "\" ..."
              << std::endl;

    M_monitor_process->start( command );

    //QProcess::startDetached( command );
}

/*-------------------------------------------------------------------*/
/*!

 */
void
MainWindow::printMonitorError( QProcess::ProcessError error )
{
    switch ( error ) {
    case QProcess::FailedToStart:
        std::cerr << "Failed to start the monitor ["
                  << Options::instance().monitorPath() << "]" << std::endl;
        break;
    case QProcess::Crashed:
        std::cerr << "Crashed the monitor ["
                  << Options::instance().monitorPath() << "]" << std::endl;
        break;
    case QProcess::Timedout:
        std::cerr << "Timeout the monitor ["
                  << Options::instance().monitorPath() << "]" << std::endl;
        break;
    case QProcess::WriteError:
        std::cerr << "Write error to the monitor ["
                  << Options::instance().monitorPath() << "]" << std::endl;
        break;
    case QProcess::ReadError:
        std::cerr << "Read error to the monitor ["
                  << Options::instance().monitorPath() << "]" << std::endl;
        break;
    case QProcess::UnknownError:
        std::cerr << "Unknown error to the monitor ["
                  << Options::instance().monitorPath() << "]" << std::endl;
        break;
    default:
        break;
    }

    this->close();
}

/*-------------------------------------------------------------------*/
/*!

 */
void
MainWindow::printMonitorExit( int exit_code,
                              QProcess::ExitStatus exit_status )
{
    switch ( exit_status ) {
    case QProcess::NormalExit:
        break;
    case QProcess::CrashExit:
        std::cerr << "monitor exit with some error. exit code=" << exit_code
                  << std::endl;
        break;
    default:
        break;
    }

    this->close();
}

/*-------------------------------------------------------------------*/
/*!

 */
void
MainWindow::kickOff()
{
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
    QString host = QString::fromStdString( Options::instance().serverHost() );
    if ( host.isEmpty() )
    {
        host = "127.0.0.1";
    }

    connectMonitorTo( host.toStdString().c_str() );
}

/*-------------------------------------------------------------------*/
/*!

 */
void
MainWindow::connectMonitorTo()
{
    QString host = QString::fromStdString( Options::instance().serverHost() );
    if ( host.isEmpty() )
    {
        host = "127.0.0.1";
    }

    bool ok = true;
    QString text = QInputDialog::getText( this,
                                          tr( "Input sserver host name" ),
                                          tr( "Host name: "),
                                          QLineEdit::Normal,
                                          host,
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

    M_open_output_act->setEnabled( false );

    closeMonitorServer();

    std::cerr << "Connect to [" << hostname << "] ..." << std::endl;

    M_monitor_client = new MonitorClient( this,
                                          M_main_data.dispHolder(),
                                          hostname,
                                          Options::instance().serverPort(),
                                          Options::instance().clientVersion() );

    if ( ! M_monitor_client->isConnected() )
    {
        std::cerr << "Conenction failed." << std::endl;
        delete M_monitor_client;
        M_monitor_client = static_cast< MonitorClient * >( 0 );
        return;
    }

    // reset all data
    M_main_data.clear();

    if ( M_player_type_dialog )
    {
        M_player_type_dialog->hide();
    }

    if ( M_config_dialog )
    {
        M_config_dialog->unzoom();
    }

    Options::instance().setMonitorClientMode( true );
    Options::instance().setServerHost( hostname );

    M_save_image_act->setEnabled( false );

    M_open_output_act->setEnabled( true );

    M_kick_off_act->setEnabled( true );
    M_set_live_mode_act->setEnabled( true );
    M_connect_monitor_act->setEnabled( false );
    M_connect_monitor_to_act->setEnabled( false );
    M_disconnect_monitor_act->setEnabled( true );

    connect( M_monitor_client, SIGNAL( received() ),
             this, SLOT( receiveMonitorPacket() ) );
//     connect( M_monitor_client, SIGNAL( timeout() ),
//              this, SLOT( disconnectMonitor() ) );

    M_log_player->setLiveMode();

    M_monitor_client->sendDispInit();

    if ( QApplication::overrideCursor() )
    {
        QApplication::restoreOverrideCursor();
    }

    this->setWindowTitle( tr( PACKAGE_NAME ) );

    createMonitorServer();
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

        disconnect( M_monitor_client, SIGNAL( received() ),
                    this, SLOT( receiveMonitorPacket() ) );

        disconnect( M_monitor_client, SIGNAL( timeout() ),
                    this, SLOT( disconnectMonitor() ) );

        delete M_monitor_client;
        M_monitor_client = static_cast< MonitorClient * >( 0 );
    }

    Options::instance().setMonitorClientMode( false );

    M_kick_off_act->setEnabled( false );
    M_set_live_mode_act->setEnabled( false );
    M_connect_monitor_act->setEnabled( true );
    M_connect_monitor_to_act->setEnabled( true );
    M_disconnect_monitor_act->setEnabled( false );
}

/*-------------------------------------------------------------------*/
/*!

 */
void
MainWindow::toggleMenuBar()
{
    this->menuBar()->setVisible( ! this->menuBar()->isVisible() );
}

/*-------------------------------------------------------------------*/
/*!

 */
void
MainWindow::toggleToolBar()
{
    M_log_player_tool_bar->setVisible( ! M_log_player_tool_bar->isVisible() );
    M_log_slider_tool_bar->setVisible( ! M_log_slider_tool_bar->isVisible() );
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
MainWindow::toggleFieldCanvas()
{
    static QSize s_old_canvas_size = M_field_canvas->size();

    Options::instance().toggleMinimumMode();

    const bool visible = ! Options::instance().minimumMode();
    M_position_label->setVisible( visible );
    if ( ! visible
         && M_detail_dialog
         && M_detail_dialog->isVisible() )
    {
        M_detail_dialog->hide();
    }
    if ( ! visible
         && M_config_dialog
         && M_config_dialog->isVisible() )
    {
        M_config_dialog->hide();
    }
    M_toggle_tool_bar_act->setEnabled( visible );
    M_toggle_status_bar_act->setEnabled( visible );
    M_full_screen_act->setEnabled( visible );
    M_show_config_dialog_act->setEnabled( visible );
    M_show_detail_dialog_act->setEnabled( visible );

    if ( Options::instance().minimumMode() )
    {
        s_old_canvas_size = M_field_canvas->size();

        QSize new_canvas_size( 280, 120 ); // for 3 lines

        QRect win_rect = this->geometry();

        this->setMinimumWidth( win_rect.width() - s_old_canvas_size.width() + new_canvas_size.width() );
        this->setMinimumHeight( win_rect.height() - s_old_canvas_size.height() + new_canvas_size.height() );

        // relocate toolbars
        this->addToolBar( Qt::TopToolBarArea, M_log_player_tool_bar );
        this->addToolBarBreak( Qt::TopToolBarArea );
        this->addToolBar( Qt::TopToolBarArea, M_log_slider_tool_bar );

        M_log_player_tool_bar->show();
        M_log_slider_tool_bar->show();

        M_log_player_tool_bar->setMovable( false );
        M_log_slider_tool_bar->setMovable( false );

#if QT_VERSION >= 0x040300
        M_log_player_tool_bar->setFloatable( false );
        M_log_slider_tool_bar->setFloatable( false );
#endif

        M_toggle_field_canvas_act->setText( tr( "Show Field" ) );

        resizeCanvas( new_canvas_size );
    }
    else
    {
        this->setMinimumWidth( 280 );
        this->setMinimumHeight( 220 );
        this->setMaximumSize( QWIDGETSIZE_MAX, QWIDGETSIZE_MAX );

#if QT_VERSION >= 0x040300
        this->removeToolBarBreak( M_log_slider_tool_bar );
#else
        this->addToolBar( Qt::TopToolBarArea, M_log_player_tool_bar );
        this->addToolBar( Qt::TopToolBarArea, M_log_slider_tool_bar );
#endif

        M_log_player_tool_bar->setMovable( true );
        M_log_slider_tool_bar->setMovable( true );
#if QT_VERSION >= 0x040300
        M_log_player_tool_bar->setFloatable( true );
        M_log_slider_tool_bar->setFloatable( true );
#endif

        M_toggle_field_canvas_act->setText( tr( "Minimum" ) );

        resizeCanvas( s_old_canvas_size );
    }
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
MainWindow::showDetailDialog()
{
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
MainWindow::showConfigDialog()
{
    M_config_dialog->setVisible( ! M_config_dialog->isVisible() );

    if ( M_config_dialog->isVisible() )
    {
        M_config_dialog->setFixedSize( M_config_dialog->size() );
    }
}

/*-------------------------------------------------------------------*/
/*!

 */
void
MainWindow::about()
{
    QString msg( tr( PACKAGE_NAME"-"VERSION"\n\n" ) );
    msg += tr( "The RoboCup Soccer Simulator LogPlayer (rcsslogplayer) is a game log replay tool\n"
               "for the RoboCup Soccer Siulator Server (rcssserver).\n"
               "\n"
               "The RoboCup Soccer Simulator Official Web Page:\n"
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
MainWindow::printShortcutKeys()
{
    QDialog dialog( this );
    QVBoxLayout * layout = new QVBoxLayout();

    QTableWidget * table_widget = new QTableWidget( &dialog );
    table_widget->insertColumn( 0 );
    table_widget->insertColumn( 1 );

    QStringList header;
    header.push_back( tr( "key" ) );
    header.push_back( tr( "action" ) );
    table_widget->setHorizontalHeaderLabels( header );

    table_widget->horizontalHeader()->setStretchLastSection( true );
    table_widget->horizontalHeader()->setResizeMode( QHeaderView::ResizeToContents );
    table_widget->verticalHeader()->hide();

    int row = 0;

    Q_FOREACH( QAction * act, this->actions() )
    {
        if ( ! act->shortcut().isEmpty() )
        {
            //std::cout << '[' << (*it)->shortcut().toString().toStdString() << "] "
            //          << QString( (*it)->text() ).remove( QChar( '&' ) ).toStdString()
            //    //<< ", " << (*it)->statusTip().toStdString()
            //          << '\n';
            table_widget->insertRow( row );
            table_widget->setItem ( row, 0, new QTableWidgetItem( act->shortcut().toString() ) );
            table_widget->setItem ( row, 1, new QTableWidgetItem( QString( act->statusTip() ).remove( QChar( '&' ) ) ) );
            ++row;
        }
    }

    table_widget->setSortingEnabled( true );
//     std::cout <<  "table row_count = " << table_widget->rowCount()
//               <<  "table col_count = " << table_widget->columnCount()
//               << std::endl;

    layout->addWidget( table_widget );
    dialog.setLayout( layout );
    if ( row > 0 )
    {
        dialog.exec();
    }
}

/*-------------------------------------------------------------------*/
/*!

 */
void
MainWindow::resizeCanvas( const QSize & size )
{
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

        if ( width_diff + size.width() < this->minimumWidth() )
        {
            std::cerr << "Too small canvas width "
                      << size.width() << " "
                      << " minimum=" << this->minimumWidth()
                      << std::endl;
            return;
        }

        if ( height_diff + size.height() < this->minimumHeight() )
        {
            std::cerr << "Too small canvas height "
                      << size.height() << " "
                      << " minimum=" << this->minimumHeight()
                      << std::endl;
            return;
        }

        rect.setWidth( size.width() + width_diff );
        rect.setHeight( size.height() + height_diff );

        this->setGeometry( rect );
    }
}

/*-------------------------------------------------------------------*/
/*!

 */
void
MainWindow::receiveMonitorPacket()
{
    if ( M_log_player->isLiveMode() )
    {
        M_log_player->showLive();
    }
    else
    {
        M_log_slider_tool_bar->updateSlider();
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
         && statusBar()->isVisible() )
    {
        double x = Options::instance().fieldX( point.x() );
        double y = Options::instance().fieldY( point.y() );

        char buf[32];
        snprintf( buf, 32,
                  "(%.2f, %.2f)",
                  x, y );

        M_position_label->setText( QString::fromAscii( buf ) );
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
        double x = Options::instance().fieldX( point.x() );
        double y = Options::instance().fieldY( point.y() );

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
        double x = Options::instance().fieldX( point.x() );
        double y = Options::instance().fieldY( point.y() );

        std::cerr << "free kick left at ("
                  << x << ", " << y << ")"
                  << std::endl;
        M_monitor_client->sendFreeKickLeft( x, y );
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
        double x = Options::instance().fieldX( point.x() );
        double y = Options::instance().fieldY( point.y() );

        std::cerr << "free kick right at ("
                  << x << ", " << y << ")"
                  << std::endl;
        M_monitor_client->sendFreeKickRight( x, y );
    }
}
