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

#include <rcsslogplayer/handler.h>
#include <rcsslogplayer/types.h>

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
    : public rcss::rcg::Handler {
public:

    static const int TIMEDELTA;

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
    static const std::size_t MAX_SHOWINFO;

    std::string M_input_file; /* input file name */
    std::string M_output_file; /* output file name */
    std::string M_command_file; /* command file name */

    std::ofstream M_out_strm; /* output file */
    std::ifstream M_com_strm; /* command file */

    int	M_state; /* status */
    int M_sent; /* send counter timer */
    int M_rect; /* recv counter timer */
    int M_limit; /* loop time */
    int M_current; /* current time */
    int M_to_time; /* jump to [to_time] */
    int	M_rec_state; /* recording state */

    int M_monitor_child; // child process id
    std::string M_monitor_command; // monitor client command path
    bool M_no_window; /* no window? */

    Port M_port; /* communication port */

#if !X_DISPLAY_MISSING
    Controler * M_controler; /* controler interface */
#endif //!X_DISPLAY_MISSING

    int M_version; /* version of the file format */

    rcss::rcg::PlayMode M_playmode;
    rcss::rcg::TeamT M_teams[2];

    rcss::rcg::ServerParamT M_server_param;
    rcss::rcg::PlayerParamT M_player_param;
    std::vector< rcss::rcg::PlayerTypeT > M_player_types;

    std::vector< boost::shared_ptr< rcss::rcg::DispInfoT > > M_dispinfo_cache;
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

    std::size_t monitorCount() const
      {
          return M_port.monitors().size();
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
    bool parseCmdLine( int argc,
                       char ** argv );
    int startMonitor( const std::string & command );
    void sendLog( const std::size_t index );
    void writeLog( const std::size_t index );


    void serializeDisp( const rcss::rcg::DispInfoT & disp,
                        const bool disp_mode,
                        const int monitor_version,
                        std::string & msg );
    void serializeServerParam( std::string & msg );
    void serializePlayerParam( std::string & msg );
    void serializePlayerType( const rcss::rcg::PlayerTypeT & param,
                              std::string & msg );


    void doHandleLogVersion( int ver );

    int doGetLogVersion() const
      {
          return M_version;
      }

    void doHandleShowInfo( const rcss::rcg::ShowInfoT & );
    void doHandleMsgInfo( const int,
                          const int,
                          const std::string & );
    void doHandlePlayMode( const int,
                           const rcss::rcg::PlayMode );
    void doHandleTeamInfo( const int,
                           const rcss::rcg::TeamT &,
                           const rcss::rcg::TeamT & );
    void doHandleDrawClear( const int )
      { };
    void doHandleDrawPointInfo( const int,
                                const rcss::rcg::PointInfoT & )
      { }
    void doHandleDrawCircleInfo( const int,
                                 const rcss::rcg::CircleInfoT & )
      { }
    void doHandleDrawLineInfo( const int,
                               const rcss::rcg::LineInfoT & )
      { }

    void doHandlePlayerType( const rcss::rcg::PlayerTypeT & );
    void doHandlePlayerParam( const rcss::rcg::PlayerParamT & );
    void doHandleServerParam( const rcss::rcg::ServerParamT & );

    void doHandleEOF()
      { }
};

#endif
