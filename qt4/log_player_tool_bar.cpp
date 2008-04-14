// -*-c++-*-

/*!
  \file log_player_tool_bar.cpp
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <QtGui>

#include "log_player_tool_bar.h"
#include "log_player.h"
#include "main_data.h"

#include <iostream>
#include <cassert>

#include "icons/play.xpm"
#include "icons/rev.xpm"
#include "icons/stop.xpm"
#include "icons/plus.xpm"
#include "icons/minus.xpm"
#include "icons/ff.xpm"
#include "icons/rew.xpm"
#include "icons/next_score.xpm"
#include "icons/prev_score.xpm"
#include "icons/rec.xpm"

/*-------------------------------------------------------------------*/
/*!

*/
LogPlayerToolBar::LogPlayerToolBar( LogPlayer * log_player,
                                    const MainData & main_data,
                                    QMainWindow * main_win )
    : QToolBar( tr( "LogPlayer" ),
                main_win )
    , M_main_data( main_data )
    , M_log_player( log_player )
{
    assert( main_win );

    this->setWindowTitle( tr( "LogPlayer" ) );
    this->setIconSize( QSize( 16, 16 ) );

    createControls( log_player, main_win );

    connect( this, SIGNAL( cycleChanged( int ) ),
             log_player, SLOT( goToCycle( int ) ) );
}

/*-------------------------------------------------------------------*/
/*!

*/
LogPlayerToolBar::~LogPlayerToolBar()
{

}

/*-------------------------------------------------------------------*/
/*!

*/
void
LogPlayerToolBar::createControls( LogPlayer * log_player,
                                  QMainWindow * main_win )
{
    // visible actions
    {
        QAction * act
            = new QAction( QIcon( QPixmap( prev_score_xpm ) ),
                           tr( "Previous score" ), this );
        act->setShortcut( tr( "Ctrl+G" ) );
        act->setStatusTip( tr( "Go to the previous goal scene.(" )
                           + act->shortcut().toString() + tr(")" ) );
        connect( act, SIGNAL( triggered() ),
                 log_player, SLOT( goToPrevScore() ) );
        this->addAction( act );
        main_win->addAction( act );
    }
    {
        QAction * act
            = new QAction( QIcon( QPixmap( rew_xpm ) ),
                           tr( "Rewind" ), this );
#ifdef Q_WS_MAC
        act->setShortcut( Qt::META + Qt::Key_Down );
#else
        act->setShortcut( Qt::CTRL + Qt::Key_Down );
#endif
        act->setStatusTip( tr( "Rewind the log player.(" )
                           + act->shortcut().toString() + tr( ")" ) );
        connect( act, SIGNAL( triggered() ),
                 log_player, SLOT( accelerateBack() ) );
        this->addAction( act );
        main_win->addAction( act );
    }
    {
        QAction * act
            = new QAction( QIcon( QPixmap( rev_xpm ) ),
                           tr( "Play Backward" ), this );
        act->setShortcut( Qt::Key_Down );
        act->setStatusTip( tr( "Play backward the log player.(" )
                           + act->shortcut().toString() + tr( ")" ) );
        connect( act, SIGNAL( triggered() ),
                 log_player, SLOT( playBack() ) );
        this->addAction( act );
        main_win->addAction( act );
    }
    {
        QAction * act
            = new QAction( QIcon( QPixmap( minus_xpm ) ),
                           tr( "Step Back" ), this );
        act->setShortcut( Qt::Key_Left );
        act->setStatusTip( tr( "One step back the log player.(" )
                           + act->shortcut().toString() + tr( ")" ) );
        connect( act, SIGNAL( triggered() ),
                 log_player, SLOT( stepBack() ) );
        this->addAction( act );
        main_win->addAction( act );
    }
    {
        QAction * act
            = new QAction( QIcon( QPixmap( stop_xpm ) ),
                           tr( "Play/Stop" ), this );
        act->setShortcut( Qt::Key_Space );
        act->setStatusTip( tr( "Play/Stop the log player. (" )
                           + act->shortcut().toString() + tr( ")" ) );
        connect( act, SIGNAL( triggered() ),
                 log_player, SLOT( playOrStop() ) );
        this->addAction( act );
        main_win->addAction( act );
    }
    {
        QAction * act
            = new QAction( QIcon( QPixmap( plus_xpm ) ),
                           tr( "Step Forward" ), this );
        act->setShortcut( Qt::Key_Right );
        act->setStatusTip( tr( "One step forward the log player. (" )
                           + act->shortcut().toString() + tr( ")" ) );
        connect( act, SIGNAL( triggered() ),
                 log_player, SLOT( stepForward() ) );
        this->addAction( act );
        main_win->addAction( act );
    }
    {
        QAction * act
            = new QAction( QIcon( QPixmap( play_xpm ) ),
                           tr( "Play Forward" ), this );
        act->setShortcut( Qt::Key_Up );
        act->setStatusTip( tr( "Play forward the log player.(" )
                           + act->shortcut().toString() + tr( ")" ) );
        connect( act, SIGNAL( triggered() ),
                 log_player, SLOT( playForward() ) );
        this->addAction( act );
        main_win->addAction( act );
    }
    {
        QAction * act
            = new QAction( QIcon( QPixmap( ff_xpm ) ),
                           tr( "Fast Forward" ), this );
#ifdef Q_WS_MAC
        act->setShortcut( Qt::META + Qt::Key_Up );
#else
        act->setShortcut( Qt::CTRL + Qt::Key_Up );
#endif
        act->setStatusTip( tr( "Fast forward the log player.(" )
                           + act->shortcut().toString() + tr( ")" ) );
        connect( act, SIGNAL( triggered() ),
                 log_player, SLOT( accelerateForward() ) );
        this->addAction( act );
        main_win->addAction( act );
    }
    {
        QAction * act
            = new QAction( QIcon( QPixmap( next_score_xpm ) ),
                           tr( "Next score" ), this );
        act->setShortcut( Qt::Key_G );
        act->setStatusTip( tr( "Go to the next goal scene.(" )
                           + act->shortcut().toString() + tr( ")" ) );
        connect( act, SIGNAL( triggered() ),
                 log_player, SLOT( goToNextScore() ) );
        this->addAction( act );
        main_win->addAction( act );
    }

    this->addSeparator();

    {
        M_toggle_record_act
            = new QAction( QIcon( QPixmap( rec_xpm ) ),
                           tr( "Record" ), this );
        M_toggle_record_act->setCheckable( true );
        M_toggle_record_act->setEnabled( false );
// #ifdef Q_WS_MAC
//         M_toggle_record_act->setShortcut( Qt::META + Qt::Key_R );
// #else
//         M_toggle_record_act->setShortcut( Qt::CTRL + Qt::Key_R );
// #endif
        M_toggle_record_act->setStatusTip( tr( "Record.(" )
                                           + M_toggle_record_act->shortcut().toString()
                                           + tr( ")" ) );
        connect( M_toggle_record_act, SIGNAL( triggered( bool ) ),
                 this, SIGNAL( recordToggled( bool ) ) );
        this->addAction( M_toggle_record_act );
        main_win->addAction( M_toggle_record_act );
    }

    // invisible actions
    {
        QAction * act
            = new QAction( //QIcon( QPixmap( go_first_xpm ) ),
                           tr( "Go first" ), this );
        act->setShortcut( Qt::Key_Home );
        act->setStatusTip( tr( "Go to the first.(" )
                           + act->shortcut().toString() + tr( ")" ) );
        connect( act, SIGNAL( triggered() ),
                 log_player, SLOT( goToFirst() ) );
        //this->addAction( act );
        main_win->addAction( act );
    }
    {
        QAction * act
            = new QAction( //QIcon( QPixmap( go_last_xpm ) ),
                           tr( "Go last" ), this );
        act->setShortcut( Qt::Key_End );
        act->setStatusTip( tr( "Go to the last.(" )
                           + act->shortcut().toString() + tr( ")" ) );
        connect( act, SIGNAL( triggered() ),
                 log_player, SLOT( goToLast() ) );
        //this->addAction( act );
        main_win->addAction( act );
    }
}

/*-------------------------------------------------------------------*/
/*!

*/
void
LogPlayerToolBar::checkRecord( bool checked )
{
    M_toggle_record_act->setChecked( checked );
}

/*-------------------------------------------------------------------*/
/*!

*/
void
LogPlayerToolBar::enableRecord( bool checked )
{
    M_toggle_record_act->setEnabled( checked );
}

/*-------------------------------------------------------------------*/
/*!

*/
void
LogPlayerToolBar::stop()
{
    M_log_player->stop();
}

/*-------------------------------------------------------------------*/
/*!

*/
void
LogPlayerToolBar::playForward()
{
    M_log_player->playForward();
}

/*-------------------------------------------------------------------*/
/*!

*/
void
LogPlayerToolBar::playBack()
{
    M_log_player->playBack();
}
