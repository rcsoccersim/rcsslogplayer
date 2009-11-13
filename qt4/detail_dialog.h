// -*-c++-*-

/*!
  \file detail_dialog.h
  \brief Detail info Dialog class Header File.
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

#ifndef RCSSLOGPLAYER_DETAIL_DIALOG_H
#define RCSSLOGPLAYER_DETAIL_DIALOG_H

#include <QDialog>

class QLabel;
class QShowEvent;
class QWidget;
class QWheelEvent;

class MainData;

class DetailDialog
    : public QDialog {

    Q_OBJECT

private:

    const MainData & M_main_data;

    // ball info labels
    QLabel * M_ball_pos;
    QLabel * M_ball_vel;
    QLabel * M_ball_polar_vel;
    QLabel * M_ball_last_move;
    QLabel * M_ball_last_polar_move;

    // player info labels

    QLabel * M_player_number;
    QLabel * M_player_type;

    QLabel * M_player_pos;
    QLabel * M_player_vel;
    QLabel * M_player_polar_vel;

    QLabel * M_player_last_move;
    QLabel * M_player_last_polar_move;

    QLabel * M_player_body;
    QLabel * M_player_head;

    //QLabel * M_player_catch_prob;
    QLabel * M_player_tackle_prob;
    QLabel * M_player_foul_prob;

    QLabel * M_player_pointto_pos;

    QLabel * M_player_focus_target;

    QLabel * M_stamina;
    QLabel * M_stamina_capacity;
    QLabel * M_effort;
    QLabel * M_recovery;

    QLabel * M_kick_count;
    QLabel * M_dash_count;
    QLabel * M_turn_count;
    QLabel * M_say_count;
    QLabel * M_turn_neck_count;
    QLabel * M_catch_count;
    QLabel * M_move_count;
    QLabel * M_change_view_count;
    QLabel * M_tackle_count;
    QLabel * M_pointto_count;
    QLabel * M_attentionto_count;

    // not used
    DetailDialog();
    DetailDialog( const DetailDialog & );
    const DetailDialog & operator=( const DetailDialog & );

public:
    //! constructor
    DetailDialog( QWidget * parent,
                  const MainData & data );

    ~DetailDialog();

private:

    void createWidgets();

    QWidget * createBallLabels();
    QWidget * createPlayerLabels();

protected:

    void showEvent( QShowEvent * event );
    void wheelEvent( QWheelEvent * event );

public slots:

    void updateLabels();

};

#endif
