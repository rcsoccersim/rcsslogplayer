/*
 *Header:
 *File: logplayer.h (for C++)
 *Author: Noda Itsuki
 *Date: 1996/11/28
 *EndHeader:
 */

/*
 *Copyright:

 Copyright (C) 1996-2000 Electrotechnical Laboratory.
 Itsuki Noda, Yasuo Kuniyoshi and Hitoshi Matsubara.
 Copyright (C) 2000-2007 RoboCup Soccer Server Maintainance Group.
 Patrick Riley, Tom Howard, Daniel Polani, Itsuki Noda,
 Mikhail Prokopenko, Jan Wendler

 This file is a part of SoccerServer.

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

#ifndef RCSSLOGPLAYER_LOGPLAYER_H
#define RCSSLOGPLAYER_LOGPLAYER_H

#include "netif.h"

#include "rcgparser.hpp"
#include "rcgdatahandler.hpp"

#if !X_DISPLAY_MISSING
#include <X11/Intrinsic.h>
#endif //!X_DISPLAY_MISSING

#include <boost/shared_ptr.hpp>

#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <cstdio>

class Controler;

/*
 *===================================================================
 *Part: Player Class
 *===================================================================
 */
class Player
    : public rcss::RCGDataHandler
{
public:

    enum State {
        STATE_WAIT = -1,
        STATE_STOP = 0,
        STATE_PLAY = 1,
        STATE_REVERSE = 2,
        STATE_REW = 3,
        STATE_FOR = 4,
        STATE_PLUS = 5,
        STATE_MINUS = 6,
        STATE_BLANK = 7,
        STATE_JUMP = 8,
        STATE_NOT_JUMP = 9,
    };

private:
    enum TimeType {
        END = -1,
        FOREVER = -2,
        NONE = -3,
    };

    enum RecordMode {
        REC_OFF = 0,
        REC_ON = 1,
    };

    static const int PLAY_CYCLE; // [msec]
    static const int FEED_CYCLE; // [msec]
    static const int STEP_CYCLE; // [msec]
    static const int MAX_SHOWINFO;

		std::string M_input_file; /* input file name */
		std::string M_output_file; /* output file name */
		std::string M_command_file; /* command file name */

		std::ofstream M_out_strm; /* output file */
		std::ifstream M_com_strm; /* command file */

		int M_sent; /* send counter timer */
		int M_rect; /* recv counter timer */
		int M_limit; /* loop time */
		int	M_state; /* status */
		int M_current; /* current time */
		int M_to_time; /* jump to [to_time] */
		int	M_rec_state; /* recording state */

		bool M_no_window; /* no window? */

		Port M_port; /* communication port */

#if !X_DISPLAY_MISSING
		Controler * M_controler; /* controler interface */
#endif //!X_DISPLAY_MISSING

    int M_version; /* version of the file format */

		char M_playmode;
		team_t M_teams[2];

    server_params_t M_server_param;
    player_params_t M_player_param;
    std::vector< player_type_t > M_player_types;

    std::vector< boost::shared_ptr< showinfo_t > > M_showinfo_cache;
    std::vector< boost::shared_ptr< showinfo_t2 > > M_showinfo2_cache;
    std::size_t M_show_index;

public:

		Player();
		~Player();

		void run( int argc, char **argv );

    bool hasOutputFile()
      {
          return M_out_strm.is_open();
      }

    int state() const
      {
          return M_state;
      }


#if !X_DISPLAY_MISSING
		void initTimer();
    void comRewind();
    void comReverse();
    void comMinus();
    void comStop();
    void comPlay();
    void comPlus();
    void comForward();
    void comRecord();
    void comBlank();
    void comJump();
#endif //!X_DISPLAY_MISSING

		void quit();

    void stepForward();
    void stepBackward();
    void stop();
		void jump();

		void sendBlank();

    void nwPrintHelp();
		void nwInit();
		void nwInitTimer();
		void nwLoop();

		bool openGameLog();
		bool openSavedLog();

    void sendParams();

    void updateImpl();

    std::string statusString() const;
private:
		bool parseCmdLine( int argc, char ** argv );

		void sendLog( const std::size_t index );
		void writeLog( const std::size_t index );

    void doHandleLogVersion( int ver );

    int doGetLogVersion() const
      {
          return M_version;
      }

    void doHandleDispInfo( std::streampos,
                           const dispinfo_t & );

    void doHandleShowInfo( std::streampos,
                           const showinfo_t & );

    void doHandleShowInfo( std::streampos,
                           const short_showinfo_t2 & );

    void doHandleMsgInfo( std::streampos,
                          short,
                          const std::string & );

    void doHandlePlayMode( std::streampos,
                           char );

    void doHandleTeamInfo( std::streampos,
                           const team_t &,
                           const team_t & );

    void doHandleServerParams( std::streampos,
                               const server_params_t & );

    void doHandlePlayerParams( std::streampos,
                               const player_params_t & );

    void doHandlePlayerType( std::streampos,
                             const player_type_t & );

};

#endif
