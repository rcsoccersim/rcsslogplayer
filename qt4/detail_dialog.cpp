// -*-c++-*-

/*!
  \file detail_dialog.cpp
  \brief Detail info Dialog class Source File.
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

#include "detail_dialog.h"

#include "main_data.h"
#include "options.h"
#include "vector_2d.h"

#include <iostream>
#include <cstdio>

/*-------------------------------------------------------------------*/
/*!

*/
DetailDialog::DetailDialog( QWidget * parent,
                            const MainData & main_data )
    : QDialog( parent )
    , M_main_data( main_data )
{
    this->setWindowTitle( tr( "Detail Dialog" ) );

    //QFont font = this->font();
    //font.setPointSize( 8 );
    //this->setFont( font );

    createWidgets();

    //this->setWindowOpacity( 0.8 ); // window transparency
}

/*-------------------------------------------------------------------*/
/*!

*/
DetailDialog::~DetailDialog()
{
    //std::cerr << "delete DetailDialog" << std::endl;
}

/*-------------------------------------------------------------------*/
/*!

*/
void
DetailDialog::createWidgets()
{
    QVBoxLayout * layout = new QVBoxLayout();
    layout->setMargin( 2 );
    layout->setSpacing( 2 );


    layout->addWidget( createBallLabels(),
                       1, Qt::AlignHCenter );

    layout->addWidget( createPlayerLabels(),
                       1, Qt::AlignHCenter );

    this->setLayout( layout );
}

/*-------------------------------------------------------------------*/
/*!

*/
QWidget *
DetailDialog::createBallLabels()
{
    QGroupBox * group_box = new QGroupBox( tr( "Ball" ) );
    group_box->setMinimumWidth( 160 );

    QGridLayout * layout = new QGridLayout();
    layout->setMargin( 1 );
    layout->setSpacing( 0 );
    layout->setColumnMinimumWidth( 0, this->fontMetrics().width( tr( "AttentionTo " ) ) );
    layout->setColumnMinimumWidth( 1, this->fontMetrics().width( tr( " -12.345 -12.345" ) ) );
    layout->setColumnMinimumWidth( 2, this->fontMetrics().width( tr( " (123.456, -123.4)" ) ) );

    int row = 0;

    layout->addWidget( new QLabel( tr( "Pos " ) ), row, 0, Qt::AlignRight );
    M_ball_pos = new QLabel( tr( "   0.00,   0.00" ) );
    layout->addWidget( M_ball_pos, row, 1, Qt::AlignRight );
    ++row;

    layout->addWidget( new QLabel( tr( "Vel " ) ), row, 0, Qt::AlignRight );
    M_ball_vel = new QLabel( tr( "   0.00,   0.00" ) );
    layout->addWidget( M_ball_vel, row, 1, Qt::AlignRight );
    M_ball_polar_vel = new QLabel( tr( " (  0.000,    0.0)" ) );
    layout->addWidget( M_ball_polar_vel, row, 2, Qt::AlignRight );
    ++row;

    layout->addWidget( new QLabel( tr( "LastMove " ) ), row, 0, Qt::AlignRight );
    M_ball_last_move = new QLabel( tr( " 000.00, 000.00" ) );
    layout->addWidget( M_ball_last_move, row, 1, Qt::AlignRight );
    M_ball_last_polar_move = new QLabel( tr( " (  0.000,    0.0)" ) );
    layout->addWidget( M_ball_last_polar_move, row, 2, Qt::AlignRight );
    ++row;

    group_box->setLayout( layout );
    return group_box;
}

/*-------------------------------------------------------------------*/
/*!

*/
QWidget *
DetailDialog::createPlayerLabels()
{
    QGroupBox * group_box = new QGroupBox( tr( "Player" ) );
    group_box->setMinimumWidth( 160 );

    QGridLayout * layout = new QGridLayout();
    layout->setMargin( 1 );
    layout->setSpacing( 0 );
    layout->setColumnMinimumWidth( 0, this->fontMetrics().width( tr( "AttentionTo " ) ) );
    layout->setColumnMinimumWidth( 1, this->fontMetrics().width( tr( " -12.345 -12.345" ) ) );
    layout->setColumnMinimumWidth( 2, this->fontMetrics().width( tr( " (123.456, -123.4)" ) ) );

    int row = 0;

    layout->addWidget( new QLabel( tr( "Unum " ) ), row, 0, Qt::AlignRight );
    M_player_number = new QLabel( tr( " Neutral  0" ) );
    layout->addWidget( M_player_number, row, 1, Qt::AlignRight );
    ++row;

    layout->addWidget( new QLabel( tr( "Type " ) ), row, 0, Qt::AlignRight );
    M_player_type = new QLabel( tr( " 0" ) );
    layout->addWidget( M_player_type, row, 1, Qt::AlignRight );
    ++row;

    layout->addWidget( new QLabel( tr( "Pos " ) ), row, 0, Qt::AlignRight );
    M_player_pos = new QLabel( tr( "   0.00,   0.00" ) );
    layout->addWidget( M_player_pos, row, 1, Qt::AlignRight );
    ++row;

    layout->addWidget( new QLabel( tr( "Vel " ) ), row, 0, Qt::AlignRight );
    M_player_vel= new QLabel( tr( "   0.00,   0.00" ) );
    layout->addWidget( M_player_vel, row, 1, Qt::AlignRight );
    M_player_polar_vel = new QLabel( tr( " (  0.000,    0.0)" ) );
    layout->addWidget( M_player_polar_vel, row, 2, Qt::AlignRight );
    ++row;

    layout->addWidget( new QLabel( tr( "LastMove " ) ), row, 0, Qt::AlignRight );
    M_player_last_move = new QLabel( tr( "   0.00,   0.00" ) );
    layout->addWidget( M_player_last_move, row, 1, Qt::AlignRight );
    M_player_last_polar_move = new QLabel( tr( " (  0.000,    0.0)" ) );
    layout->addWidget( M_player_last_polar_move, row, 2, Qt::AlignRight );
    ++row;

    layout->addWidget( new QLabel( tr( "Body " ) ), row, 0, Qt::AlignRight );
    M_player_body = new QLabel( tr( "   0.0" ) );
    layout->addWidget( M_player_body, row, 1, Qt::AlignRight );
    ++row;

    layout->addWidget( new QLabel( tr( "Head " ) ), row, 0, Qt::AlignRight );
    M_player_head = new QLabel( tr( "   0.0" ) );
    layout->addWidget( M_player_head, row, 1, Qt::AlignRight );
    ++row;

    layout->addWidget( new QLabel( tr( "PointtoPos " ), group_box ),
                       row, 0, Qt::AlignRight );
    M_player_pointto_pos = new QLabel( tr( "      -,      -" ), group_box );
    layout->addWidget( M_player_pointto_pos, row, 1, Qt::AlignRight );
    ++row;

    layout->addWidget( new QLabel( tr( "Focus " ), group_box ),
                       row, 0, Qt::AlignRight );
    M_player_focus_target = new QLabel( tr( "      -,      -" ), group_box );
    layout->addWidget( M_player_focus_target, row, 1, Qt::AlignRight );
    ++row;

    layout->addWidget( new QLabel( tr( "Stamina " ) ), row, 0, Qt::AlignRight );
    M_stamina = new QLabel( tr( "    0.0" ) );
    layout->addWidget( M_stamina, row, 1, Qt::AlignRight );
    ++row;

    layout->addWidget( new QLabel( tr( "Effort " ) ), row, 0, Qt::AlignRight );
    M_effort = new QLabel( tr( " 0.000 / 0.000" ) );
    layout->addWidget( M_effort, row, 1, Qt::AlignRight );
    ++row;

    layout->addWidget( new QLabel( tr( "Recovery " ) ), row, 0, Qt::AlignRight );
    M_recovery = new QLabel( tr( " 0.000 / 1.0" ) );
    layout->addWidget( M_recovery, row, 1, Qt::AlignRight );
    ++row;

    layout->addWidget( new QLabel( tr( "Kick " ) ), row, 0, Qt::AlignRight );
    M_kick_count = new QLabel( tr( "     0" ) );
    layout->addWidget( M_kick_count, row, 1, Qt::AlignRight );
    ++row;

    layout->addWidget( new QLabel( tr( "Dash " ) ), row, 0, Qt::AlignRight );
    M_dash_count = new QLabel( tr( "     0" ) );
    layout->addWidget( M_dash_count, row, 1, Qt::AlignRight );
    ++row;

    layout->addWidget( new QLabel( tr( "Turn " ) ), row, 0, Qt::AlignRight );
    M_turn_count = new QLabel( tr( "     0" ) );
    layout->addWidget( M_turn_count, row, 1, Qt::AlignRight );
    ++row;

    layout->addWidget( new QLabel( tr( "Say " ) ), row, 0, Qt::AlignRight );
    M_say_count = new QLabel( tr( "     0" ) );
    layout->addWidget( M_say_count, row, 1, Qt::AlignRight );
    ++row;

    layout->addWidget( new QLabel( tr( "TurnNeck " ) ), row, 0, Qt::AlignRight );
    M_turn_neck_count = new QLabel( tr( "     0" ) );
    layout->addWidget( M_turn_neck_count, row, 1, Qt::AlignRight );
    ++row;

    layout->addWidget( new QLabel( tr( "Catch " ) ), row, 0, Qt::AlignRight );
    M_catch_count = new QLabel( tr( "     0" ) );
    layout->addWidget( M_catch_count, row, 1, Qt::AlignRight );
    ++row;

    layout->addWidget( new QLabel( tr( "Move " ) ), row, 0, Qt::AlignRight );
    M_move_count = new QLabel( tr( "     0" ) );
    layout->addWidget( M_move_count, row, 1, Qt::AlignRight );
    ++row;

    layout->addWidget( new QLabel( tr( "ChbView " ) ), row, 0, Qt::AlignRight );
    M_change_view_count = new QLabel( tr( "     0" ) );
    layout->addWidget( M_change_view_count, row, 1, Qt::AlignRight );
    ++row;

    layout->addWidget( new QLabel( tr( "Tackle " ), group_box ),
                       row, 0, Qt::AlignRight );
    M_tackle_count = new QLabel( tr( "     0" ), group_box );
    layout->addWidget( M_tackle_count, row, 1, Qt::AlignRight );
    ++row;

    layout->addWidget( new QLabel( tr( "Pointto " ), group_box ),
                       row, 0, Qt::AlignRight );
    M_pointto_count = new QLabel( tr( "     0" ), group_box );
    layout->addWidget( M_pointto_count, row, 1, Qt::AlignRight );
    ++row;

    layout->addWidget( new QLabel( tr( "AttentionTo " ), group_box ),
                       row, 0, Qt::AlignRight );
    M_attentionto_count = new QLabel( tr( "     0" ), group_box );
    layout->addWidget( M_attentionto_count, row, 1, Qt::AlignRight );
    ++row;

    group_box->setLayout( layout );
    return group_box;
}

/*-------------------------------------------------------------------*/
/*!

*/
void
DetailDialog::showEvent( QShowEvent * event )
{
    updateLabels();

    event->accept();
}

/*-------------------------------------------------------------------*/
/*!

*/
void
DetailDialog::updateLabels()
{
    if ( ! this->isVisible() )
    {
        return;
    }

    DispConstPtr disp = M_main_data.getDispInfo( M_main_data.index() );

    if ( ! disp )
    {
        return;
    }

    DispConstPtr prev_disp
        = ( M_main_data.index() <= 0
            ? DispConstPtr()
            : M_main_data.getDispInfo( M_main_data.index() - 1 ) );


    char buf[64];

    // update ball

    const rcss::rcg::BallT & ball = disp->show_.ball_;

    std::snprintf( buf, 64, " %6.2f, %6.2f",
                   ball.x_, ball.y_ );
    M_ball_pos->setText( QString::fromAscii( buf ) );

    {
        Vector2D vel( ball.vx_, ball.vy_ );
        std::snprintf( buf, 64, " %6.2f, %6.2f",
                       vel.x, vel.y );
        M_ball_vel->setText( QString::fromAscii( buf ) );

        std::snprintf( buf, 64, " (%7.3f, %6.1f)",
                       vel.r(), vel.th().degree() );
        M_ball_polar_vel->setText( QString::fromAscii( buf ) );
    }

    if ( ! prev_disp )
    {
        M_ball_last_move->setText( tr( "   0.00,   0.00" ) );
        M_ball_last_polar_move->setText( tr( " (  0.000,    0.0)" ) );
    }
    else
    {
        Vector2D move( ball.x_ - prev_disp->show_.ball_.x_,
                       ball.y_ - prev_disp->show_.ball_.y_ );

        std::snprintf( buf, 64, " %6.2f, %6.2f",
                       move.x, move.y );
        M_ball_last_move->setText( QString::fromAscii( buf ) );

        std::snprintf( buf, 64, " (%7.3f, %6.1f)",
                       move.r(), move.th().degree() );
        M_ball_last_polar_move->setText( QString::fromAscii( buf ) );
    }

    // update player

    int number = Options::instance().selectedNumber();
    if ( number == 0 || 11 < std::abs( number ) )
    {
        return;
    }

    const int player_index = std::abs( number ) - 1 + ( number > 0 ? 0 : 11 );
    const rcss::rcg::PlayerT & player = disp->show_.player_[player_index];
    const rcss::rcg::PlayerTypeT & player_type = M_main_data.playerType( player.type_ );

    // side, number
    std::snprintf( buf, 64, " %8s  %2d",
                   number > 0 ? "   Left" : "   Right",
                   number > 0 ? number : -number );
    M_player_number->setText( QString::fromAscii( buf ) );

    // player type
    std::snprintf( buf, 64, " %2d",
                   player.type_ );
    M_player_type->setText( QString::fromAscii( buf ) );

    // pos
    std::snprintf( buf, 64, " %6.2f, %6.2f",
                   player.x_, player.y_ );
    M_player_pos->setText( QString::fromAscii( buf ) );

    // velocity, speed, speed dir
    if ( player.hasVelocity() )
    {
        Vector2D vel( player.vx_, player.vy_ );

        std::snprintf( buf, 64, " %6.2f, %6.2f",
                       vel.x, vel.y );
        M_player_vel->setText( QString::fromAscii( buf ) );

        std::snprintf( buf, 64, " (%7.3f, %6.1f)",
                       vel.r(), vel.th().degree() );
        M_player_polar_vel->setText( QString::fromAscii( buf ) );
    }
    else
    {
        M_player_vel->setText( tr( "      -,      -") );
        M_player_polar_vel->setText( tr( "      -,      -") );
    }

    if ( prev_disp )
    {
        const rcss::rcg::PlayerT & last_player = prev_disp->show_.player_[player_index];
        Vector2D last_move( player.x_ - last_player.x_,
                            player.y_ - last_player.y_ );

        std::snprintf( buf, 64, " %6.2f, %6.2f",
                       last_move.x, last_move.y );
        M_player_last_move->setText( QString::fromAscii( buf ) );

        std::snprintf( buf, 64, " (%7.3f, %6.1f)",
                       last_move.r(), last_move.th().degree() );
        M_player_last_polar_move->setText( QString::fromAscii( buf ) );
    }
    else
    {
        M_player_last_move->setText( tr( "   0.00,   0.00" ) );
        M_player_last_polar_move->setText( tr( "  (  0.000,    0.0)" ) );
    }

    // body angle
    std::snprintf( buf, 64," %6.1f",
                   player.body_ );
    M_player_body->setText( QString::fromAscii( buf ) );

    // neck, head angle
    if ( player.hasNeck() )
    {
        AngleDeg head = player.body_ + player.neck_;

        std::snprintf( buf, 64, " %6.1f(%6.1f)",
                       head.degree(), player.neck_ );
        M_player_head->setText( QString::fromAscii( buf ) );
    }
    else
    {
        M_player_head->setText( tr( "     -" ) );
    }

    // pointto point
    if ( player.isPointing() )
    {
        std::snprintf( buf, 64, " %6.2f, %6.2f",
                       player.point_x_, player.point_y_ );
        M_player_pointto_pos->setText( QString::fromAscii( buf ) );
    }
    else
    {
        M_player_pointto_pos->setText( tr( "      -,      -") );
    }

    // focus target
    if ( player.isFocusing() )
    {
        std::snprintf( buf, 64, " %5s %2d",
                       player.focus_side_ == 'l' ? "Right" : "Left",
                       player.focus_unum_ );
        M_player_focus_target->setText( QString::fromAscii( buf ) );
    }
    else
    {
        M_player_focus_target->setText( tr( " --- ") );
    }

    // stamina, effort, recovery
    if ( player.hasStamina() )
    {
        std::snprintf( buf, 64, " %6.1f",
                       player.stamina_ );
        M_stamina->setText( QString::fromAscii( buf ) );

        std::snprintf( buf, 64, " %5.3f / %5.3f",
                       player.effort_, player_type.effort_max_ );
        M_effort->setText( QString::fromAscii( buf ) );

        std::snprintf( buf, 64, " %5.3f / 1.0",
                       player.recovery_ );
        M_recovery->setText( QString::fromAscii( buf ) );
    }

    // command count
    {
        std::snprintf( buf, 64, "  %5d",
                       player.kick_count_ );
        M_kick_count->setText( QString::fromAscii( buf ) );

        std::snprintf( buf, 64, "  %5d",
                       player.dash_count_ );
        M_dash_count->setText( QString::fromAscii( buf ) );

        std::snprintf( buf, 64, "  %5d",
                       player.turn_count_ );
        M_turn_count->setText( QString::fromAscii( buf ) );

        std::snprintf( buf, 64, "  %5d",
                       player.say_count_ );
        M_say_count->setText( QString::fromAscii( buf ) );

        std::snprintf( buf, 64, "  %5d",
                       player.turn_neck_count_ );
        M_turn_neck_count->setText( QString::fromAscii( buf ) );

        std::snprintf( buf, 64, "  %5d",
                       player.catch_count_ );
        M_catch_count->setText( QString::fromAscii( buf ) );

        std::snprintf( buf, 64, "  %5d",
                       player.move_count_ );
        M_move_count->setText( QString::fromAscii( buf ) );

        std::snprintf( buf, 64, "  %5d",
                       player.change_view_count_ );
        M_change_view_count->setText( QString::fromAscii( buf ) );

        std::snprintf( buf, 64, "  %5d",
                       player.tackle_count_ );
        M_tackle_count->setText( QString::fromAscii( buf ) );

        std::snprintf( buf, 64, "  %5d",
                       player.pointto_count_ );
        M_pointto_count->setText( QString::fromAscii( buf ) );

        std::snprintf( buf, 64, "  %5d",
                       player.attentionto_count_ );
        M_attentionto_count->setText( QString::fromAscii( buf ) );
    }
}
