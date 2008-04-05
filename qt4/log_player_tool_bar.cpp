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

class CycleEdit
    : public QLineEdit {

public:

    CycleEdit()
        : QLineEdit( tr( "0" ) )
      {
          this->setMaximumSize( 28, 20 );
          this->setMinimumSize( 16, 16 );

          QSizePolicy policy( QSizePolicy::Maximum, QSizePolicy::Maximum );
          policy.setHorizontalStretch( 1 );
          policy.setVerticalStretch( 1 );
          this->setSizePolicy( policy );

          QFont font = this->font();
          font.setPointSize( 6 );
          this->setFont( font );
      }

    virtual
    QSize sizeHint() const
      {
          QSize size = QLineEdit::sizeHint();
          if ( size.width() > 32 )
          {
              size.setWidth( 32 );
          }
          if ( size.height() > 24 )
          {
              size.setHeight( 24 );
          }
          return size;
      }

};

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

    this->setIconSize( QSize( 16, 16 ) );

    //this->setAllowedAreas( Qt::TopToolBarArea
    //                       | Qt::BottomToolBarArea );
    //this->setMovable( false );

    //this->setMaximumSize( 640, 640 );

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

    // visible widgets

    M_cycle_slider = new QSlider( this->orientation() );
    connect( this, SIGNAL( orientationChanged( Qt::Orientation ) ),
             M_cycle_slider, SLOT( setOrientation( Qt::Orientation ) ) );
    M_cycle_slider->setStatusTip( tr( "Cycle Slider." ) );
    M_cycle_slider->setToolTip( tr( "Cycle Slider" ) );
    M_cycle_slider->setRange( 0, 0 );
    M_cycle_slider->setValue( 0 );
    M_cycle_slider->setTickInterval( 1 );
    M_cycle_slider->setMaximumSize( 640, 640 );
    M_cycle_slider->setMinimumWidth( 200 );
    //M_cycle_slider->setMinimumSize( 400, 400 );
    connect( M_cycle_slider, SIGNAL( sliderMoved( int ) ),
             log_player, SLOT( goToIndex( int ) ) );

    this->addWidget( M_cycle_slider );

    M_cycle_edit = new CycleEdit();
    M_cycle_edit->setStatusTip( tr( "Cycle Input Box." ) );
    M_cycle_edit->setToolTip( tr( "Cycle Input" ) );
    connect( M_cycle_edit, SIGNAL( returnPressed() ),
             this, SLOT( editCycle() ) );

    this->addWidget( M_cycle_edit );

    // invisible action
    {
        QAction * act = new QAction( tr( "Focus Cycle Edit" ), this );
#ifdef Q_WS_MAC
        act->setShortcut( tr( "Meta+F" ) );
#else
        act->setShortcut( tr( "Ctrl+F" ) );
#endif
        connect( act, SIGNAL( triggered() ),
                 M_cycle_edit, SLOT( setFocus() ) );

        main_win->addAction( act );
    }

    connect( this, SIGNAL( orientationChanged( Qt::Orientation ) ),
             this, SLOT( changeOrientation( Qt::Orientation ) ) );
}

/*-------------------------------------------------------------------*/
/*!

*/
void
LogPlayerToolBar::editCycle()
{
    bool ok = true;
    int cycle = M_cycle_edit->text().toInt( &ok );

    if ( ok
         && cycle >= 0 )
    {
        emit cycleChanged( cycle );
    }
}

/*-------------------------------------------------------------------*/
/*!

*/
void
LogPlayerToolBar::moveEvent( QMoveEvent * event )
{
    if ( this->orientation() == Qt::Vertical )
    {
        if ( this->width() > this->minimumWidth() )
        {
            this->resize( this->minimumWidth(), 480 );
        }
    }
    else
    {
        if ( this->height() > this->minimumHeight() )
        {
            this->resize( 480, this->minimumHeight() );
        }
    }

    event->accept();
}

/*-------------------------------------------------------------------*/
/*!

*/
void
LogPlayerToolBar::changeOrientation( Qt::Orientation orientation )
{
    if ( orientation == Qt::Vertical )
    {
        M_cycle_slider->setMinimumSize( 16, 100 );
        M_cycle_slider->setMaximumSize( 16, 640 );
    }
    else
    {
        M_cycle_slider->setMinimumSize( 200, 16 );
        M_cycle_slider->setMaximumSize( 640, 16 );
    }
#if 0
    std::cerr << "--------------------"
              << "Orientation = " << orientation << '\n'
              << " x, y, width, height    "
              << x() << ' ' << y() << ' '
              << width() << ' ' << height() << '\n'
              << " baseSize = width, height    "
              << baseSize().width() << ' ' << baseSize().height() << '\n'
              << " frameSize = width, height    "
              << frameSize().width() << ' ' << frameSize().height() << '\n'
              << " childrenRect = top, left, width, height    "
              << childrenRect().top() << ' ' << childrenRect().left() << ' '
              << childrenRect().width() << ' ' << childrenRect().height() << '\n'
              << " geometry = x, y, width, height    "
              << geometry().x() << ' ' << geometry().y() << ' '
              << geometry().width() << ' ' << geometry().y() << '\n'
              << " frameGeometry = x, y, width, height    "
              << frameGeometry().x() << ' ' << frameGeometry().y() << ' '
              << frameGeometry().width() << ' ' << frameGeometry().height() << '\n'
              << " normalGeometry = x, y, width, height    "
              << normalGeometry().x() << ' ' << normalGeometry().y() << ' '
              << normalGeometry().width() << ' ' << normalGeometry().height() << '\n'
              << " minimumSize = width, height    "
              << minimumWidth() << ' ' << minimumHeight() << '\n'
              << " maximumSize = width, height    "
              << maximumWidth() << ' ' << maximumHeight() << '\n'
              << " sizeHint = width, height()    "
              << sizeHint().width() << ' ' << sizeHint().height() << '\n'
              << " sizePolicy = horizontal, vertical    "
              << sizePolicy().horizontalPolicy() << ' ' << sizePolicy().verticalPolicy() << '\n'
        ;

    /*
    std::cerr << "------\n"
              << " cycleEdit "
              << " x, y, width, height    "
              << M_cycle_slider->x() << ' ' << M_cycle_slider->y() << ' '
              << M_cycle_slider->width() << ' ' << M_cycle_slider->height() << '\n'
              << " baseSize = width, height    "
              << M_cycle_slider->baseSize().width() << ' '
              << M_cycle_slider->baseSize().height() << '\n'
              << " frameSize = width, height    "
              << M_cycle_slider->frameSize().width() << ' '
              << M_cycle_slider->frameSize().height() << '\n'
              << " childrenRect = top, left, width, height    "
              << M_cycle_slider->childrenRect().top() << ' '
              << M_cycle_slider->childrenRect().left() << ' '
              << M_cycle_slider->childrenRect().width() << ' '
              << childrenRect().height() << '\n'
              << " geometry = x, y, width, height    "
              << M_cycle_slider->geometry().x() << ' '
              << M_cycle_slider->geometry().y() << ' '
              << M_cycle_slider->geometry().width() << ' '
              << geometry().y() << '\n'
              << " frameGeometry = x, y, width, height    "
              << M_cycle_slider->frameGeometry().x() << ' '
              << M_cycle_slider->frameGeometry().y() << ' '
              << M_cycle_slider->frameGeometry().width() << ' '
              << M_cycle_slider->frameGeometry().height() << '\n'
              << " normalGeometry = x, y, width, height    "
              << M_cycle_slider->normalGeometry().x() << ' '
              << M_cycle_slider->normalGeometry().y() << ' '
              << M_cycle_slider->normalGeometry().width() << ' '
              << M_cycle_slider->normalGeometry().height() << '\n'
              << " minimumSize = width, height    "
              << M_cycle_slider->minimumWidth() << ' '
              << M_cycle_slider->minimumHeight() << '\n'
              << " maximumSize = width, height    "
              << M_cycle_slider->maximumWidth() << ' '
              << M_cycle_slider->maximumHeight() << '\n'
              << " sizeHint = width, height()    "
              << M_cycle_slider->sizeHint().width() << ' '
              << M_cycle_slider->sizeHint().height() << '\n'
              << " sizePolicy = horizontal, vertical    "
              << M_cycle_slider->sizePolicy().horizontalPolicy() << ' '
              << M_cycle_slider->sizePolicy().verticalPolicy() << '\n'
        ;
    */
    std::cerr << std::flush;
#endif
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

/*-------------------------------------------------------------------*/
/*!

*/
void
LogPlayerToolBar::updateSlider()
{
    if ( ! this->isVisible() )
    {
        return;
    }

    //std::cerr << "LogPlaeyrToolBar::update()" << std::endl;

    int size = M_main_data.dispHolder().dispInfoCont().size();

    if ( size == 1 ) size = 0;

    if ( M_cycle_slider->maximum() != size )
    {
        M_cycle_slider->setRange( 0, size );
    }

    int val = M_main_data.index();

    if ( M_cycle_slider->value() != val )
    {
        M_cycle_slider->setValue( val );
    }
}
