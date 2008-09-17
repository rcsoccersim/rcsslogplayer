// -*-c++-*-
/* -*- Mode: C -*-
 *Header:
 *File: logplayer.C
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


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "logplayer.h"

#include <iostream>

#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <csignal>
#include <cerrno>
#include <cstring>

#include <sys/select.h> // select() : POSIX 1003.1-2001
#include <sys/time.h> // select() : previous standards
#include <sys/types.h> // select() : previous standards
#include <unistd.h> // select() : previous standards

#define PROMPT		"LogPlayer=> "

extern Player player;

void
nw_sigalarm_handler()
{
    player.updateImpl();
}


void
Player::nwInit()
{
    const int interval_msec = 250;

    struct timeval wait;

    fd_set read_fds;
    fd_set read_fds_back;

    FD_ZERO( &read_fds );
    //FD_SET( 0, &read_fds ); // stdin
    FD_SET( M_port.socket().getFD(), &read_fds );
    read_fds_back = read_fds;

    std::cout << "Wait for monitor..." << std::endl;;

    while ( 1 )
    {
        read_fds = read_fds_back;
        wait.tv_sec = interval_msec / 1000;
        wait.tv_usec = ( interval_msec % 1000 ) * 1000;
        //select( 0, NULL, NULL, NULL, &wait );
        int ret = ::select( M_port.socket().getFD() + 1,
                            &read_fds,
                            static_cast< fd_set * >( 0 ),
                            static_cast< fd_set * >( 0 ),
                            &wait );
        if ( ret < 0 )
        {
            std::perror( "select" );
            break;
        }
        else if ( ret == 0 )
        {
            // no message. timed out
            continue;
        }
        else
        {
//             if ( FD_ISSET( 0, &read_fds ) )
//             {
//                 std::string buf;
//                 std::cin >> buf;
//                 std::cout << "input [" << buf << "]" << std::endl;
//                 processStdin( buf );
//                 std::cout << "\n"PROMPT << std::flush;
//             }
//             else
            if ( FD_ISSET( M_port.socket().getFD(), &read_fds ) )
            {
                std::size_t monitor_size = M_port.monitors().size();
                if ( M_port.recv() > 0
                     && monitor_size != M_port.monitors().size() )
                {
                    M_state = STATE_STOP;
                    std::cout << "LogPlayer is started.\n" << std::endl;
                    sendParams();
                    sendLog( M_show_index );
                    break;
                }
            }
        }
    }
}

void
Player::nwInitTimer()
{
    struct sigaction alarm_action;
    struct itimerval itv;

    M_rect = 0;
    M_sent = 0;

    itv.it_interval.tv_sec = 0;
    itv.it_interval.tv_usec = TIMEDELTA * 1000;
    itv.it_value.tv_sec = 0;
    itv.it_value.tv_usec = TIMEDELTA * 1000;

    alarm_action.sa_handler = (void (*)(int))nw_sigalarm_handler;
    //	alarm_action.sa_flags = 0;                // [2000/11/20.frehberg.cs.tu-berlin.de]
    // [2002/12/06 jellekok] fgets unblocked each time a signal was released
    // changing the flags to SA_RESTART solved the problem.
    alarm_action.sa_flags = SA_RESTART;
    sigaction( SIGALRM, &alarm_action, NULL );
    setitimer( ITIMER_REAL, &itv, NULL );
}

void
Player::nwPrintHelp()
{
    std::cout << "Available commands:\n"
              << " help\n"
              << " quit\n"
              << " play <Cycle1> to <Cycle1>\n"
              << " stop"
              << " jump to <Cycle2>\n"
              << " feed to <Cycle2>\n"
              << " step to <Cycle2>\n"
              << " sleep {<Time[msec]>|forever}\n"
              << " load <CommandFile>"
              << "\n"
              << "   Cycle1 := {<Integer>|end|here}\n"
              << "   Cycle2 := {<Integer>|end}\n"
              << std::endl;
}

void
Player::nwLoop()
{
    /*
      help
      quit
      play <Cycle1> to <Cycle1>
      stop
      jump to <Cycle2>
      feed to <Cycle2>
      step to <Cycle2>
      sleep <Time[msec]>
      load <CommandFile>

      Cycle1 := {<Integer>|end|here}
      Cycle2 := {<Integer>|end}
    */

    std::string buf;
    char com[64];
    char arg[3][64];
    bool stdin_flag = false;

    if ( ! M_com_strm.is_open() )
    {
        stdin_flag = true;
    }

    while ( 1 )
    {
        if ( stdin_flag )
        {
            std::cout << PROMPT << std::flush;
            if ( ! std::cin.good() )
            {
                std::cout << "  The status of stdin is not good.\n"
                          << "  If you run the log player as the background process,"
                          << "  you must run the rcsslogplayer as the foreground process."
                          << std::endl;
                break;
            }
            std::getline( std::cin, buf );
        }
        else
        {
            if ( M_state != STATE_STOP )
            {
                sigpause( SIGUSR1 );
                continue;
            }
            if ( ! std::getline( M_com_strm, buf ) )
            {
                stdin_flag = true;
                continue;
            }

            if ( buf.empty()
                 || buf[0] == '#' || buf[0] == '\n' )
            {
                continue;
            }
        }

        if ( std::sscanf( buf.c_str(), "%63s", com ) < 1 )
        {
            std::cerr << "Illegal command: [" << buf << "]" << std::endl;
            continue;
        }

        if ( ! std::strcmp( com, "quit" ) )
        {
            if ( ! stdin_flag )
            {
                std::cout << buf << std::endl;
            }
            break;
        }
        else if ( ! std::strcmp( com, "play" ) )
        {
            int tmp;
            int from, to;

            int n = std::sscanf( buf.c_str(), "%63s %63s %63s %63s",
                                 com, arg[0], arg[1], arg[2] );
            if ( n == 4 && ! std::strcmp( arg[1], "to" ) )
            {
                if ( ! std::strcmp( arg[0], "end" ) )
                {
                    from = END;
                }
                else if ( ! std::strcmp( arg[0], "here" ) )
                {
                    from = M_current;
                }
                else if ( std::sscanf( arg[0], "%d", &tmp ) == 1 )
                {
                    from = tmp;
                }
                else
                {
                    std::cerr << "Illegal command: [" << buf << "]" << std::endl;
                    continue;
                }

                if ( ! std::strcmp( arg[2], "end" ) )
                {
                    to = END;
                }
                else if ( ! std::strcmp( arg[2], "here" ) )
                {
                    to = M_current;
                }
                else if ( std::sscanf( arg[2], "%d", &tmp ) == 1 )
                {
                    to = tmp;
                }
                else
                {
                    std::cerr << "Illegal command: [" << buf << "]" << std::endl;
                    continue;
                }
            }
            else
            {
                std::cerr << "Illegal command: [" << buf << "]" << std::endl;
                continue;
            }

            if ( from == to )
            {
                std::cerr << "Can't play. (from = to)" << std::endl;
                continue;
            }

            std::cout << "commmand [" << buf << "]" << std::endl;

            if ( from != M_current )
            {
                M_to_time = from;
                jump();
                if ( M_state == STATE_NOT_JUMP )
                {
                    std::cerr << "Can't play. (can't go to"
                              << M_to_time << ")" << std::endl;
                    M_state = STATE_STOP;
                    continue;
                }
            }

            if ( to > M_current || to == END )
            {
                M_state = STATE_PLAY;
                M_to_time = to;
                M_limit = PLAY_CYCLE;
            }
            else if ( to < M_current )
            {
                M_state = STATE_REVERSE;
                M_to_time = to;
                M_limit = PLAY_CYCLE;
            }
        }
        else if ( ! std::strcmp( com, "stop" ) )
        {
            std::cout << "commmand [" << buf << "]" << std::endl;
            stop();
            stdin_flag = true;
        }
        else if ( ! std::strcmp( com, "jump" )
                  || ! std::strcmp( com, "feed" )
                  || ! std::strcmp( com, "step" ) )
        {
            int tmp;
            int n = std::sscanf( buf.c_str(),
                                 "%63s %63s %63s",
                                 com, arg[0], arg[1] );
            if ( n == 3 && ! std::strcmp( arg[0], "to" ) )
            {
                if ( std::sscanf( arg[1], "%d", &tmp ) == 1 )
                {
                    M_to_time = tmp;
                }
                else if ( ! std::strcmp( arg[1], "end" ) )
                {
                    M_to_time = END;
                }
                else
                {
                    std::cerr << "Illegal command: [" << buf << "]" << std::endl;
                    continue;
                }
            }
            else
            {
                std::cerr << "Illegal command: [" << buf << "]" << std::endl;
                continue;
            }

            if ( M_to_time == M_current )
            {
                std::cerr << "Can't go to current time. Current time: "
                          << M_to_time << std::endl;
                continue;
            }

            std::cout << "commmand [" << buf << "]" << std::endl;

            if ( ! std::strcmp( com, "jump" ) )
            {
                jump();
                if ( M_state == STATE_NOT_JUMP )
                {
                    std::cerr << "Can't jump to " << M_to_time << "."
                              << std::endl;
                    continue;
                    M_state = STATE_STOP;
                }
            }
            else
            {
                if ( ! std::strcmp( com, "feed" ) )
                {
                    M_limit = FEED_CYCLE;
                }
                else
                {
                    M_limit = STEP_CYCLE;
                }

                if ( M_to_time > M_current || M_to_time == END )
                {
                    M_state = STATE_FOR;
                }
                else if ( M_to_time < M_current )
                {
                    M_state = STATE_REW;
                }
                else
                {
                    M_state = STATE_STOP;
                }
            }
        }
        else if ( ! std::strcmp( com, "sleep" ) )
        {
            unsigned wait_time = 0;
            if ( std::sscanf( buf.c_str(), "%63s %d", com, &wait_time ) == 2 )
            {
                if ( ! stdin_flag )
                {
                    std::cout << buf << std::endl;
                }
                M_limit = wait_time;
                stop();
                M_sent = 0;
                M_state = STATE_WAIT;
            }
            else
            {
                std::cerr << "Illegal command: [" << buf << "]" << std::endl;
            }

            std::cout << "commmand [" << buf << "]" << std::endl;

        }
        else if ( ! std::strcmp( com, "load" ) )
        {
            if ( ! stdin_flag )
            {
                std::cerr << "load command can be available only stdin mode."
                          << std::endl;
                continue;
            }

            char filepath[256];
            if ( std::sscanf( buf.c_str(), "%63s %255s",
                              com, filepath ) == 2 )
            {
                M_com_strm.close();
                M_com_strm.open( filepath );
                if ( ! M_com_strm.is_open() )
                {
                    std::cerr << "Failed to open the command file ["
                              << filepath << "]" << std::endl;
                }
                else
                {
                    stdin_flag = false;
                }
            }
            else
            {
                std::cerr << "Illegal command: [" << buf << "]" << std::endl;
            }
        }
        else if ( ! std::strcmp( com, "help" ) )
        {
            if ( stdin_flag )
            {
                nwPrintHelp();
            }
        }
        else
        {
            std::cerr << "Illegal command: [" << buf << "]" << std::endl;
        }
    }
}
