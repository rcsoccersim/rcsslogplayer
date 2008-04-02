// -*-c++-*-

/*!
  \file options.h
  \brief application setting variables.
*/

/*
 *Copyright:

 Copyright (C) The RoboCup Soccer Server Maintenance Group.
 Hidehisa AKIYAMA

 This code is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

 *EndCopyright:
 */

/////////////////////////////////////////////////////////////////////

#ifndef RCSSLOGPLAYER_OPTIONS_H
#define RCSSLOGPLAYER_OPTIONS_H

#include <string>

/*!
  \class Options
  \brief application setting holder singleton.
*/
class Options {
public:
//     static const int MIN_SCORE_BOARD_HEIGHT;
//     static const int MAX_SCORE_BOARD_HEIGHT;

    static const int DEFAULT_TIMER_INTERVAL;

private:

    //
    // monitor client options
    //
    bool M_connect;
    std::string M_host;
    int M_port;
    int M_client_version;
    int M_wait_seconds;
    bool M_auto_quit_mode;
    bool M_time_shift_replay;

    //
    // logplayer options
    //
    std::string M_game_log_file; //!< game log file path to be opened
    bool M_auto_loop_mode;
    int M_timer_interval; //!< logplayer's timer interval. default 100[ms]

    //
    // window options
    //
    int M_window_x;
    int M_window_y;
    int M_window_width;
    int M_window_height;
    bool M_maximize;
    bool M_full_screen;

    int M_canvas_width;
    int M_canvas_height;

    bool M_hide_menu_bar;
    bool M_hide_tool_bar;
    bool M_hide_status_bar;

    //
    // view options
    //
    //std::string M_field_grass_type;
    //bool M_keepaway;

    bool M_anti_aliasing;

    bool M_show_score_board;
    bool M_show_team_logo;
    bool M_show_ball;
    bool M_show_player;
    bool M_show_player_number;
    bool M_show_player_type;
    bool M_show_view_area;
    bool M_show_control_area;
    bool M_show_stamina;
    bool M_show_pointto;

    double M_ball_size; //!< fixed ball radius
    double M_player_size; //!< fixed player radius

    //! private access for singleton
    Options();

    //! noncopyable
    Options( const Options & );
    //! noncopyable
    Options & operator=( const Options & );

public:
    ~Options();

    static
    Options & instance();

    /*!
      \biref analyze command line options
    */
    bool parseCmdLine( int argc,
                       char ** argv );

    //
    // monitor options
    //

    bool connect() const
      {
          return M_connect;
      }

    const
    std::string & host() const
      {
          return M_host;
      }

    int port() const
      {
          return M_port;
      }

    int clientVersion() const
      {
          return M_client_version;
      }

    int waitSeconds() const
      {
          return M_wait_seconds;
      }

    bool autoQuitMode() const
      {
          return M_auto_quit_mode;
      }

    //
    // logplayer options
    //

    const
    std::string & gameLogFile() const
      {
          return M_game_log_file;
      }

    const
    std::string & gameLogDir() const
      {
          return M_game_log_dir;
      }

    void setGameLogDir( const std::string & dir )
      {
          M_game_log_dir = dir;
      }

    bool timeShiftReplay() const
      {
          return M_time_shift_replay;
      }

    bool autoLoopMode() const
      {
          return M_auto_loop_mode;
      }

    int timerInterval() const
      {
          return M_timer_interval;
      }

    //
    // window option
    //

    int windowX() const
      {
          return M_window_x;
      }

    int windowY() const
      {
          return M_window_y;
      }

    int windowWidth() const
      {
          return M_window_width;
      }

    int windoHeight() const
      {
          return M_window_height;
      }

    bool maximize() const
      {
          return M_maximize;
      }

    bool fullScreen() const
      {
          return M_full_screen;
      }

    int canvasWidth() const
      {
          return M_canvas_width;
      }

    int canvasHeight() const
      {
          return M_canvas_height;
      }

    bool hideMenuBar() const
      {
          return M_hide_menu_bar;
      }

    bool hideToolBar() const
      {
          return M_hide_tool_bar;
      }

    bool hideStatusBar() const
      {
          return M_hide_status_bar;
      }

    //
    // view options
    //

    bool antiAliasing() const
      {
          return M_anti_aliasing;
      }

    bool showScoreBoard() const
      {
          return M_show_score_board;
      }

    bool showTeamLogo() const
      {
          return M_show_team_logo;
      }

    bool showPlayerNumber() const
      {
          return M_show_player_number;
      }

    bool showPlayerType() const
      {
          return M_show_player_type;
      }

    bool showViewArea() const
      {
          return M_show_view_area;
      }

    bool showControlrea() const
      {
          return M_show_control_area;
      }

    bool showStamina() const
      {
          return M_show_stamina;
      }

    bool showPointto() const
      {
          return M_show_pointto;
      }

    const
    double & ballSize() const
      {
          return M_ball_size;
      }

    const
    double & playerSize() const
      {
          return M_player_size;
      }

};

#endif
