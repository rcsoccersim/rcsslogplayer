// -*-c++-*-

/*!
  \file log_slider_tool_bar.cpp
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <QtGui>

#include "log_slider_tool_bar.h"

#include "log_player.h"
#include "main_data.h"

#include <iostream>
#include <cmath>
#include <cassert>

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

class CycleSlider
    : public QSlider {
private:

public:

    CycleSlider( Qt::Orientation orientation,
                 QWidget * parent = 0 )
        : QSlider( orientation, parent )
      { }

protected:

    void mousePressEvent( QMouseEvent * event )
      {
          if ( event->button() == Qt::LeftButton )
          {
              double rate = 0;
              if ( this->orientation() == Qt::Horizontal )
              {
                  rate = static_cast< double >( event->pos().x() )
                      / static_cast< double >( this->width() );

              }
              else
              {
                  rate = static_cast< double >( this->height() - event->pos().y() )
                      / static_cast< double >( this->height() );
              }

              int val = this->minimum()
                  + static_cast< int >( rint( ( this->maximum() - this->minimum() ) * rate ) );

              this->setValue( val );
          }

          QSlider::mousePressEvent( event );
      }

};

/*-------------------------------------------------------------------*/
/*!

*/
LogSliderToolBar::LogSliderToolBar( LogPlayer * log_player,
                                    const MainData & main_data,
                                    QMainWindow * main_win )
    : QToolBar( tr( "LogSlider" ),
                main_win )
    , M_main_data( main_data )
    , M_log_player( log_player )
{
    assert( main_win );

    this->setWindowTitle( tr( "LogSlider" ) );
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
LogSliderToolBar::~LogSliderToolBar()
{

}

/*-------------------------------------------------------------------*/
/*!

*/
void
LogSliderToolBar::createControls( LogPlayer * log_player,
                                  QMainWindow * /*main_win*/ )
{
    // visible widgets

    M_cycle_slider = new CycleSlider( this->orientation() );
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
//     connect( M_cycle_slider, SIGNAL( sliderMoved( int ) ),
//              log_player, SLOT( goToIndex( int ) ) );
    connect( M_cycle_slider, SIGNAL( valueChanged( int ) ),
             log_player, SLOT( goToIndex( int ) ) );

    this->addWidget( M_cycle_slider );

    //

    M_cycle_edit = new CycleEdit();
    M_cycle_edit->setStatusTip( tr( "Cycle Input Box." ) );
    M_cycle_edit->setToolTip( tr( "Cycle Input" ) );
    connect( M_cycle_edit, SIGNAL( returnPressed() ),
             this, SLOT( editCycle() ) );

    this->addWidget( M_cycle_edit );

    // invisible action
//     {
//         QAction * act = new QAction( tr( "Focus Cycle Edit" ), this );
// #ifdef Q_WS_MAC
//         act->setShortcut( tr( "Meta+F" ) );
// #else
//         act->setShortcut( tr( "Ctrl+F" ) );
// #endif
//         act->setStatusTip( tr( "Set the keyboard focus to the cycle edit box." ) );
//         connect( act, SIGNAL( triggered() ),
//                  M_cycle_edit, SLOT( setFocus() ) );
//         main_win->addAction( act );
//     }

    // dummy spaces
    {
        QFrame * dummy_frame = new QFrame;
        QHBoxLayout * layout = new QHBoxLayout;
        //layout->addSpacing( 10 );
        layout->addStretch( 1 );
        dummy_frame->setLayout( layout );
        this->addWidget( dummy_frame );
    }
    {
        QFrame * dummy_frame = new QFrame;
        QVBoxLayout * layout = new QVBoxLayout;
        //layout->addSpacing( 10 );
        layout->addStretch( 1 );
        dummy_frame->setLayout( layout );
        this->addWidget( dummy_frame );
    }

    connect( this, SIGNAL( orientationChanged( Qt::Orientation ) ),
             this, SLOT( changeOrientation( Qt::Orientation ) ) );
}

/*-------------------------------------------------------------------*/
/*!

*/
// void
// LogSliderToolBar::moveEvent( QMoveEvent * event )
// {
//     if ( this->orientation() == Qt::Vertical )
//     {
//         if ( this->width() > this->minimumWidth() )
//         {
//             this->resize( this->minimumWidth(), 480 );
//         }
//     }
//     else
//     {
//         if ( this->height() > this->minimumHeight() )
//         {
//             this->resize( 480, this->minimumHeight() );
//         }
//     }

//     event->accept();
// }

/*-------------------------------------------------------------------*/
/*!

*/
void
LogSliderToolBar::editCycle()
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
LogSliderToolBar::changeOrientation( Qt::Orientation orientation )
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
}

/*-------------------------------------------------------------------*/
/*!

*/
void
LogSliderToolBar::updateSlider()
{
    if ( ! this->isVisible() )
    {
        return;
    }

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
