/* -*- Mode: C++ -*-
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
 Copyright (C) 2000- RoboCup Soccer Server Maintainance Group.
 Patrick Riley, Tom Howard, Daniel Polani, Itsuki Noda,
 Mikhail Prokopenko, Jan Wendler, Hidehisa Akiyama

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
#include <config.h>
#endif

#include "logplayer.h"

#include "controler.h"

#include <rcsslogplayer/parser.h>
#include <rcsslogplayer/util.h>

#ifdef HAVE_LIBZ
#include <rcsslogplayer/gzfstream.h>
#endif

#include <boost/program_options.hpp>

#include <iostream>
#include <algorithm>
#include <sstream>
#include <vector>
#include <set>
#include <functional>
#include <iterator>

#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <csignal>
#include <cerrno> // errno

#ifdef HAVE_UNISTD_H
#include <unistd.h> // fork, execlp
#endif
#ifdef HAVE_SYS_TIME_H
#include <sys/time.h> // itimerval, setitimer
#endif
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h> // SIGCLD, SIGCHLD
#endif

#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif
#ifdef HAVE_WINDOWS_H
#include <windows.h>
#endif

extern Player player;

namespace {

#if !X_DISPLAY_MISSING
#ifndef X11R5
XtSignalId g_sig_id; /* signal handler id */
#endif
#endif //!X_DI

inline
float
quantize( const float & val,
          const float & prec )
{
    return rintf( val / prec ) * prec;
}

void
sig_handle( int )
{
    if ( player.monitorCount() <= 1 )
    {
        player.quit();
    }
}

} // end noname namespace

static
void
update()
{
#if !X_DISPLAY_MISSING
#ifdef X11R5
    XtIntervalId intid;
#endif
#endif //!X_DISPLAY_MISSING

    player.updateImpl();

#if !X_DISPLAY_MISSING
#ifdef X11R5
    intid = XtAppAddTimeOut( M_controler->app_context,
                             TIMEDELTA, (XtTimerCallbackProc)update, NULL );
#endif
#endif //!X_DISPLAY_MISSING
}

#if !X_DISPLAY_MISSING
#ifndef X11R5
void
sigalarm_handler( int )
{
    XtNoticeSignal( g_sig_id );
}
#endif
#endif //!X_DISPLAY_MISSING


const int Player::TIMEDELTA = 10;

const int Player::PLAY_CYCLE = 100;
const int Player::FEED_CYCLE = 50;
const int Player::STEP_CYCLE = 450;
const std::size_t Player::MAX_SHOWINFO = 18000;


Player::Player()
    : M_state( STATE_WAIT )
    , M_sent( 0 )
    , M_rect( 0 )
    , M_limit( 10000 )
    , M_current( 1 )
    , M_to_time( NONE )
    , M_rec_state( REC_OFF )
    , M_monitor_child( 0 )
    , M_no_window( false )
    , M_version( 0 )
    , M_playmode( rcss::rcg::PM_Null )
    , M_show_index( 0 )
{

#if !X_DISPLAY_MISSING
    M_controler = new Controler();
#endif //!X_DISPLAY_MISSING
}

Player::~Player()
{
    M_out_strm.flush();
    M_out_strm.close();

    M_com_strm.close();

#if !X_DISPLAY_MISSING
    delete M_controler;
#endif //!X_DISPLAY_MISSING
}

void
Player::run( int argc, char ** argv )
{
#if !X_DISPLAY_MISSING
    if ( ! M_controler )
    {
        return;
    }
#endif

    if ( ! parseCmdLine( argc, argv ) )
    {
        return;
    }

    if ( ! openSavedLog() )
    {
        return;
    }

    if ( ! openGameLog() )
    {
        return;
    }

    M_state = STATE_WAIT;
    M_limit = 10000;
    M_sent = 0;
    M_rect = 0;
    M_to_time = NONE;
    M_rec_state = REC_OFF;
    M_current = 1;

    M_port.init();

    M_monitor_child = startMonitor( M_monitor_command );

#if !X_DISPLAY_MISSING
    /* X logplayer */
    if ( ! M_no_window )
    {
        M_controler->init( argc, argv );
        initTimer();
        M_controler->display_status();
        M_controler->run();
    }
    else
#endif //!X_DISPLAY_MISSING
    {
        /* no window logplayer */
        /* open command file */
        if ( ! M_command_file.empty() )
        {
            M_com_strm.open( M_command_file.c_str() );
            if ( ! M_com_strm.is_open() )
            {
                std::cerr << __FILE__ << ": " << __LINE__
                          << ": Can't open command file " << M_command_file
                          << std::endl;
                M_out_strm.close();
                return;
            }
        }
        nwInit();
        nwInitTimer();
        nwLoop();
    }
}

bool
Player::parseCmdLine( int argc, char **argv )
{
    // TODO: reimplemented with rcssconfparser
    bool help = false;
    int port_number = 0;

    namespace po = boost::program_options;

    po::options_description visibles( "Allowed options" );

    visibles.add_options()
        ( "help,h",
          "generates this message" )
        ( "file,f",
          po::value< std::string >(),
          "configuration file" )
        ( "port,p",
          po::value< int >( &port_number )->default_value( 6000, "6000" ),
          "connection port number" )
        ( "monitor,m",
          po::value< std::string >( &M_monitor_command )->default_value( "" ),
          "monitor command path" )
        ( "nowindow,n",
          po::bool_switch( &M_no_window )->default_value( false ),
          "no window mode" )
        ( "output-file,o",
          po::value< std::string >( &M_output_file )->default_value( "" ),
          "output file name" )
        ( "command-file,c",
          po::value< std::string >( &M_command_file )->default_value( "" ),
          "command file name" )
        ;

    po::options_description invisibles( "Invisibles" );
    invisibles.add_options()
        ( "input-file",
          po::value< std::string >( &M_input_file )->default_value( "" ),
          "set the path to Game Log file(.rcg) to be opened."  )
        ;
    po::positional_options_description pdesc;
    pdesc.add( "input-file", 1 ); // allowed only one rcg file

    po::options_description all_desc( "All options:" );
    all_desc.add( visibles ).add( invisibles );

    try
    {
        po::variables_map vm;
        po::command_line_parser parser( argc, argv );
        parser.options( all_desc ).positional( pdesc );
        po::store( parser.run(), vm );
        po::notify( vm );

        if ( vm.count( "help" ) )
        {
            help = true;
        }

        if ( vm.count( "file" ) )
        {
            std::ifstream fin( vm["file"].as< std::string >().c_str() );
            if ( ! fin )
            {
                std::cerr << "Could not open the configuration file ["
                          << vm["file"].as< std::string >()
                          << "]" << std::endl;
                return false;
            }

            std::vector< std::string > args;

            std::string buf;
            while ( std::getline( fin, buf ) )
            {
                std::string::size_type first_pos = buf.find_first_not_of( " \t" );
                if ( first_pos == std::string::npos
                     || buf[first_pos] == '#' )
                {
                    // empty line or comment mark
                    continue;
                }
                std::string trim_buf = "--";
                trim_buf += buf.substr( first_pos );
                /* replace from ':' to ' ' */
                std::replace( trim_buf.begin(), trim_buf.end(), ':', ' ' );
                std::istringstream istrm( trim_buf );
                std::copy( std::istream_iterator< std::string >( istrm ),
                           std::istream_iterator< std::string >(),
                           std::back_inserter( args ) );
            }

            po::store( po::command_line_parser( args ).options( all_desc ).run(),
                       vm );
            po::notify( vm );
        }

    }
    catch ( std::exception & e )
    {
        std::cerr << e.what() << std::endl;
        help = true;
    }

    if ( help
         || M_input_file.empty() )
    {
        if ( M_input_file.empty() )
        {
            std::cout << "Log file is not specified." << std::endl;
        }
#ifdef HAVE_LIBZ
        std::cout << "Usage: rcsslogplayer [options...] <GameLogFile>[.gz]\n";
#else
        std::cout << "Usage: rcsslogplayer [options...] <GameLogFile>\n";
#endif
        std::cout << visibles << std::endl;
        return false;
    }

    M_port.setListenPort( port_number );

    return true;
}

int
Player::startMonitor( const std::string & command )
{
    if ( command.empty() )
    {
        return 0;
    }

    std::cout << "Starting monitor"
              << " \"/bin/sh -c " << command << "\" ..." << std::endl;

    int pid = ::fork();
    if ( pid == -1 )
    {
        std::cerr << PACKAGE << "-" << VERSION
                  << ": Error: Could not fork to start monitor: "
                  << std::strerror( errno ) << std::endl;
        return 0;
    }

    if ( pid == 0 )
    {
        ::usleep( 500000 ); // 0.5 sec
        ::execlp( "/bin/sh", "sh", "-c", command.c_str(), (char *)NULL );
        std::cerr << PACKAGE << "-" << VERSION
                  << ": Error: Could not execute \"/bin/sh -c "
                  << command << "\": "
                  << std::strerror( errno ) << std::endl;
        return 0;
    }

    // watch the status of child process
#ifdef SIGCLD
    std::signal( SIGCLD, sig_handle );
#elif SIGCHLD
    std::signal( SIGCHLD, sig_handle );
#endif

    return pid;
}

void
Player::quit()
{
    M_out_strm.flush();
    M_out_strm.close();

    std::exit( 0 );
}

#if !X_DISPLAY_MISSING
void
Player::initTimer()
{
#ifndef X11R5
    struct sigaction alarm_action;
    struct itimerval itv;

    g_sig_id = XtAppAddSignal( M_controler->app_context,
                               (XtSignalCallbackProc)update, NULL );
    itv.it_interval.tv_sec = 0;
    itv.it_interval.tv_usec = TIMEDELTA * 1000;
    itv.it_value.tv_sec = 0;
    itv.it_value.tv_usec = TIMEDELTA * 1000;

    alarm_action.sa_handler = &sigalarm_handler;
    alarm_action.sa_flags = 0;        /* init */              // [2000/11/20.frehberg.cs.tu-berlin.de]

    sigaction( SIGALRM, &alarm_action, NULL );
    setitimer( ITIMER_REAL, &itv, NULL );

#else
    XtIntervalId intid;
    intid = XtAppAddTimeOut( M_controler->app_context,
                             TIMEDELTA, (XtTimerCallbackProc)update, NULL );
#endif
}
#endif //!X_DISPLAY_MISSING


void
Player::stepForward()
{
    if ( ! M_dispinfo_cache.empty() )
    {
        if ( M_show_index < M_dispinfo_cache.size() )
        {
            ++M_show_index;
            sendLog( M_show_index );
            writeLog( M_show_index );
        }
        else //if ( M_show_index == M_dispinfo_cache.size() )
        {
            stop();
        }
    }

    if ( M_to_time != END
         && M_current >= M_to_time )
    {
        stop();
    }
}

void
Player::stepBackward()
{
    if ( ! M_dispinfo_cache.empty() )
    {
        if ( M_show_index > 0 )
        {
            --M_show_index;
            sendLog( M_show_index );
            writeLog( M_show_index );
        }
        else
        {
            stop();
        }
    }

    if ( M_to_time != END
         && M_current <= M_to_time )
    {
        stop();
    }
}

void
Player::stop()
{
    M_state = STATE_STOP;
    M_to_time = NONE;
#if !X_DISPLAY_MISSING
    if ( ! M_no_window )
    {
        M_controler->display_time( M_current );
        M_controler->display_status();
        M_controler->buttons_reset();
    }
#endif //!X_DISPLAY_MISSING
}



struct DispInfoCmp
    : public std::binary_function< boost::shared_ptr< rcss::rcg::DispInfoT >,
                                   boost::shared_ptr< rcss::rcg::DispInfoT >,
                                   bool > {
    result_type operator()( const first_argument_type & lhs,
                            const second_argument_type & rhs )
      {
          return lhs->show_.time_ < rhs->show_.time_;
      }
};


void
Player::jump()
{
    int max_time = ( M_dispinfo_cache.empty()
                     ? 0
                     : M_dispinfo_cache.back()->show_.time_ );

    if ( M_to_time == END )
    {
        if ( ! M_dispinfo_cache.empty() )
        {
            M_show_index = M_dispinfo_cache.size();
            sendLog( M_show_index );
            writeLog( M_show_index );
        }
    }
    else if ( M_to_time <= max_time )
    {
        if ( ! M_dispinfo_cache.empty() )
        {
            static boost::shared_ptr< rcss::rcg::DispInfoT > tmp( new rcss::rcg::DispInfoT );
            tmp->show_.time_ = static_cast< rcss::rcg::UInt32 >( M_to_time );

            std::vector< boost::shared_ptr< rcss::rcg::DispInfoT > >::iterator it
                = std::lower_bound( M_dispinfo_cache.begin(),
                                    M_dispinfo_cache.end(),
                                    tmp,
                                    DispInfoCmp() );
            if ( it != M_dispinfo_cache.end() )
            {
                M_show_index = std::distance( M_dispinfo_cache.begin(), it ) + 1;
                sendLog( M_show_index );
                writeLog( M_show_index );
            }
        }
    }
    else
    {
        M_to_time = NONE;
    }

    stop();
}


void
Player::sendBlank()
{
#if !X_DISPLAY_MISSING
    M_controler->display_time( ntohs( (unsigned short)-1 ) );
#endif //!X_DISPLAY_MISSING

    sendLog( M_show_index );
    writeLog( M_show_index );
}

void
Player::updateImpl()
{
    M_sent += TIMEDELTA;
    M_rect += TIMEDELTA;

    if ( M_sent >= M_limit )
    {
        switch ( M_state ) {

        case STATE_PLUS: // 1 step forward
            M_limit = TIMEDELTA;
            stepForward();
            stop();
            break;

        case STATE_MINUS: // 1 step back
            M_limit = TIMEDELTA;
            stepBackward();
            stop();
            break;

        case STATE_PLAY: // forward play
            M_limit = PLAY_CYCLE;
            stepForward();
            break;

        case STATE_REVERSE: // backward play
            M_limit = PLAY_CYCLE;
            stepBackward();
            break;

        case STATE_STOP:
            M_limit = PLAY_CYCLE;
            //stop();
            break;

        case STATE_FOR: // accelerated forward play
            //M_limit = FEED_CYCLE;
            stepForward();
            break;

        case STATE_REW: // accelerated backword play
            //M_limit = FEED_CYCLE;
            stepBackward();
            break;

        case STATE_BLANK:
            M_limit = PLAY_CYCLE;
            sendBlank();
            break;

        case STATE_JUMP:
            M_limit = TIMEDELTA;
            jump();
            break;

        case STATE_WAIT:
            M_limit = TIMEDELTA;
            M_state = STATE_STOP;
            break;

        default:
            break;
        }

        M_sent = 0;
    }

    if ( M_rect >= 400 )
    {
        size_t monitor_size = M_port.monitors().size();
        int n = M_port.recv();
        if ( n > 0
             && M_state == STATE_WAIT )
        {
            M_limit = PLAY_CYCLE;
            //if ( ! std::strncmp( M_port.rbuf, "(dispinit", 9 ) )
            if ( monitor_size != M_port.monitors().size() )
            {
                M_sent = 0;
                M_state = STATE_STOP;
            }
            else
            {
                M_state = STATE_STOP;
#if !X_DISPLAY_MISSING
                if ( ! M_no_window )
                {
                    M_controler->display_status();
                }
#endif //!X_DISPLAY_MISSING
            }
        }

        if ( n > 0 )
        {
            sendParams();
            sendLog( M_show_index );
        }
        M_rect = 0;
    }
}

std::string
Player::statusString() const
{
    switch ( M_state ) {
    case STATE_WAIT:
        return "wait for monitor";;
        break;

    case STATE_STOP:
        return std::string( "stop : " ) += M_input_file;
        break;

    case STATE_PLAY:
        return std::string( "play : " ) += M_input_file;
        break;

    case STATE_REVERSE:
        return std::string( "reverse : " ) += M_input_file;
        break;

    case STATE_REW:
        return std::string( "rewind : " ) += M_input_file;
        break;

    case STATE_FOR:
        return std::string( "forward : " ) += M_input_file;
        break;

    case STATE_PLUS:
        return std::string( "+1 : " ) += M_input_file;
        break;

    case STATE_MINUS:
        return std::string( "-1 : " ) += M_input_file;
        break;

    case STATE_BLANK:
        return std::string( "sending blank" );
        break;

    case STATE_JUMP:
        {
            char cycle[32];
            snprintf( cycle, 15, "jump to %d : ", M_to_time );
            return std::string( cycle ) += M_input_file;
        }
        break;

    case STATE_NOT_JUMP:
        return std::string( "can't jump" ) ;
        break;

    default :
        return std::string( "error" );
        break;
    }

    return std::string( "error" );
}


void
Player::writeLog( const std::size_t index )
{
    if ( M_rec_state != REC_ON )
    {
        return;
    }

    if ( index == 0 )
    {
        return;
    }

    if ( ! M_out_strm.is_open() )
    {
        return;
    }

    if ( M_dispinfo_cache.size() < index )
    {
        return;
    }

    if ( doGetLogVersion() >= rcss::rcg::REC_VERSION_4 )
    {
        static const char * s_playmode_strings[] = PLAYMODE_STRINGS;
        static rcss::rcg::PlayMode s_playmode = rcss::rcg::PM_Null;
        static rcss::rcg::TeamT s_teams[2];

        boost::shared_ptr< rcss::rcg::DispInfoT > disp = M_dispinfo_cache[index - 1];

        // if playmode has changed wirte playmode
        if ( s_playmode != disp->pmode_ )
        {
            s_playmode = disp->pmode_;

            M_out_strm << "(playmode " << disp->show_.time_
                       << ' ' << s_playmode_strings[s_playmode] << ")\n";
        }

        if ( ! s_teams[0].equals( disp->team_[0] )
             || ! s_teams[1].equals( disp->team_[1] ) )
        {
            s_teams[0] = disp->team_[0];
            s_teams[1] = disp->team_[1];

            M_out_strm << "(team " << disp->show_.time_
                       << ' ' << ( s_teams[0].name_.empty() ? "null" : s_teams[0].name_.c_str() )
                       << ' ' << ( s_teams[1].name_.empty() ? "null" : s_teams[1].name_.c_str() )
                       << ' ' << s_teams[0].score_
                       << ' ' << s_teams[1].score_;
            if ( s_teams[0].penaltyTrial() > 0
                 || s_teams[1].penaltyTrial() > 0 )
            {
                M_out_strm << ' ' << s_teams[0].pen_score_
                           << ' ' << s_teams[0].pen_miss_
                           << ' ' << s_teams[0].pen_score_
                           << ' ' << s_teams[0].pen_miss_;
            }
            M_out_strm << ")\n";
        }

        std::string msg;
        serializeDisp( *disp, false, 0, msg );
        M_out_strm << msg << '\n';
    }
    else if ( doGetLogVersion() == rcss::rcg::REC_VERSION_3 )
    {
        static rcss::rcg::PlayMode s_playmode = rcss::rcg::PM_Null;
        static rcss::rcg::TeamT s_teams[2];

        boost::shared_ptr< rcss::rcg::DispInfoT > disp = M_dispinfo_cache[index - 1];

        // if playmode has changed wirte playmode
        if ( s_playmode != disp->pmode_ )
        {
            s_playmode = disp->pmode_;

            rcss::rcg::Int16 mode = htons( rcss::rcg::PM_MODE );
            char pm = static_cast< char >( s_playmode );
            M_out_strm.write( reinterpret_cast< const char * >( &mode ),
                              sizeof( mode ) );
            M_out_strm.write( reinterpret_cast< const char * >( &pm ),
                              sizeof( pm ) );
        }

        // if teams or score has changed, write teams and score
        if ( ! s_teams[0].equals( disp->team_[0] )
             || ! s_teams[1].equals( disp->team_[1] ) )
        {
            s_teams[0] = disp->team_[0];
            s_teams[1] = disp->team_[1];

            rcss::rcg::Int16 mode = htons( rcss::rcg::TEAM_MODE );
            rcss::rcg::team_t teams[2];
            rcss::rcg::convert( s_teams[0], teams[0] );
            rcss::rcg::convert( s_teams[1], teams[1] );
            M_out_strm.write( reinterpret_cast< const char * >( &mode ),
                              sizeof( mode ) );
            M_out_strm.write( reinterpret_cast< const char * >( teams ),
                              sizeof( rcss::rcg::team_t ) * 2 );
        }

        // write positional data
        rcss::rcg::Int16 mode = htons( rcss::rcg::SHOW_MODE );
        rcss::rcg::short_showinfo_t2 show;
        rcss::rcg::convert( disp->show_, show );
        M_out_strm.write( reinterpret_cast< const char * >( &mode ),
                          sizeof( mode ) );
        M_out_strm.write( reinterpret_cast< const char * >( &show ),
                          sizeof( rcss::rcg::short_showinfo_t2 ) );
    }
    else if ( doGetLogVersion() == rcss::rcg::REC_VERSION_2 )
    {
        boost::shared_ptr< rcss::rcg::DispInfoT > disp = M_dispinfo_cache[index - 1];

        rcss::rcg::Int16 mode = htons( rcss::rcg::SHOW_MODE );
        rcss::rcg::showinfo_t show;
        rcss::rcg::convert( static_cast< char >( disp->pmode_ ),
                            disp->team_[0],
                            disp->team_[1],
                            disp->show_,
                            show );

        M_out_strm.write( reinterpret_cast< const char * >( &mode ),
                          sizeof( mode ) );
        M_out_strm.write( reinterpret_cast< const char * >( &show ),
                          sizeof( rcss::rcg::showinfo_t ) );
    }
    else // REC_OLD_VERSION
    {
        boost::shared_ptr< rcss::rcg::DispInfoT > disp = M_dispinfo_cache[index - 1];

        rcss::rcg::dispinfo_t new_disp;
        new_disp.mode = htons( rcss::rcg::SHOW_MODE );
        rcss::rcg::convert( static_cast< char >( disp->pmode_ ),
                            disp->team_[0],
                            disp->team_[1],
                            disp->show_,
                            new_disp.body.show );

        M_out_strm.write( reinterpret_cast< const char * >( &disp ),
                          sizeof( rcss::rcg::dispinfo_t ) );
    }
}

void
Player::sendLog( const std::size_t index )
{
    if ( index == 0 )
    {
        return;
    }

    if ( index <= M_dispinfo_cache.size() )
    {
        boost::shared_ptr< rcss::rcg::DispInfoT > disp = M_dispinfo_cache[index - 1];
        M_current = disp->show_.time_;

        int counter = 0;
        for ( std::vector< Monitor >::const_iterator p = M_port.monitors().begin();
              p != M_port.monitors().end();
              ++p )
        {
            ++counter;
            if ( p->version_ == 1 )
            {
                rcss::rcg::dispinfo_t new_disp;
                new_disp.mode = htons( rcss::rcg::SHOW_MODE );
                rcss::rcg::convert( static_cast< char >( disp->pmode_ ),
                                    disp->team_[0],
                                    disp->team_[1],
                                    disp->show_,
                                    new_disp.body.show );
                M_port.send( new_disp, p->addr_ );
            }
            else if ( p->version_ == 2 )
            {
                rcss::rcg::dispinfo_t2 new_disp2;
                new_disp2.mode = htons( rcss::rcg::SHOW_MODE );
                rcss::rcg::convert( static_cast< char >( disp->pmode_ ),
                                    disp->team_[0],
                                    disp->team_[1],
                                    disp->show_,
                                    new_disp2.body.show );
                M_port.send( new_disp2, p->addr_ );
            }
            else if ( p->version_ >= 3 )
            {
                std::string msg;
                serializeDisp( *disp, true, p->version_, msg );
                M_port.send( msg, p->addr_ );
            }
        }
    }

#if !X_DISPLAY_MISSING
    if ( ! M_no_window )
    {
        M_controler->display_time( M_current );
        M_controler->display_status();
        M_controler->buttons_reset();
    }
#endif //!X_DISPLAY_MISSING
}


bool
Player::openGameLog()
{
    // open the file
#ifdef HAVE_LIBZ
    rcss::gzifstream fin( M_input_file.c_str() );
#else
    std::ifstream fin( M_input_file.c_str(),
                       std::ios_base::in
                       | std::ios_base::binary );
#endif
    if ( ! fin.is_open() )
    {
        std::cerr << "Error: Can't open log file " << M_input_file
                  << std::endl;
        return false;
    }

    rcss::rcg::Parser parser( *this );
    int count = -1;
    while ( parser.parse( fin ) )
    {
        if ( ++count % 512 == 0 )
        {
            std::fprintf( stdout, "parsing... %d\r", // %zd, size_t
                          static_cast< int >( M_dispinfo_cache.size() ) );
            std::fflush( stdout );
        }
    }

    if ( ! fin.eof() )
    {
        std::cerr << "Error reading log: " << std::endl;
        return false;
    }

    std::cout << "showinfo size = " << M_dispinfo_cache.size()
              << std::endl;
    if ( ! M_dispinfo_cache.empty() )
    {
        M_show_index = 1;
    }
    return true;
}

bool
Player::openSavedLog()
{
    if ( M_output_file.empty() )
    {
        return true;
    }

    // open the file
    M_out_strm.open( M_output_file.c_str(),
                     std::ios_base::binary | std::ios_base::out );
    if ( ! M_out_strm.is_open() )
    {
        std::cerr << "Can't open output file " << M_output_file << std::endl;
        return false;
    }

    return true;
}


void
Player::sendParams()
{
    if ( M_version < rcss::rcg::REC_VERSION_3 )
    {
        return;
    }

    static std::set< const Monitor * > S_sent_set;
    for ( std::vector< Monitor >::const_iterator p = M_port.monitors().begin();
          p != M_port.monitors().end();
          ++p )
    {
        if ( S_sent_set.find( &(*p) ) == S_sent_set.end() )
        {
            if ( p->version_ == 2 )
            {
                rcss::rcg::dispinfo_t2 disp2;

                disp2.mode = htons( rcss::rcg::PARAM_MODE );
                rcss::rcg::convert( M_server_param, disp2.body.sparams );
                M_port.send( disp2, p->addr_ );

                disp2.mode = htons( rcss::rcg::PPARAM_MODE );
                rcss::rcg::convert( M_player_param, disp2.body.pparams );
                M_port.send( disp2, p->addr_ );

                disp2.mode = htons( rcss::rcg::PT_MODE );
                for ( std::vector< rcss::rcg::PlayerTypeT >::iterator it = M_player_types.begin();
                      it != M_player_types.end();
                      ++it )
                {
                    rcss::rcg::convert( *it, disp2.body.ptinfo );
                    M_port.send( disp2, p->addr_ );
                }
            }
            else if ( p->version_ >= 3 )
            {
                std::string msg;
                serializeServerParam( msg );
                M_port.send( msg, p->addr_ );

                serializePlayerParam( msg );
                M_port.send( msg, p->addr_ );

                for ( std::vector< rcss::rcg::PlayerTypeT >::iterator it = M_player_types.begin();
                      it != M_player_types.end();
                      ++it )
                {
                    serializePlayerType( *it, msg );
                    M_port.send( msg, p->addr_ );
                }
            }

            S_sent_set.insert( &(*p) );
        }
    }
}


void
Player::serializeDisp( const rcss::rcg::DispInfoT & disp,
                       const bool disp_mode,
                       const int monitor_version,
                       std::string & msg )
{
    const float PREC = 0.0001f;
    const float DPREC = 0.001f;

    std::ostringstream ostr;

    ostr << "(show " << disp.show_.time_;
    if ( disp_mode )
    {
        ostr << " (pm " << disp.pmode_ << ')';
        ostr << " (tm"
             << ' ' << ( disp.team_[0].name_.empty() ? "null" : disp.team_[0].name_.c_str() )
             << ' ' << ( disp.team_[1].name_.empty() ? "null" : disp.team_[1].name_.c_str() )
             << ' ' << disp.team_[0].score_
             << ' ' << disp.team_[1].score_;
        if ( disp.team_[0].penaltyTrial() > 0
             || disp.team_[1].penaltyTrial() > 0 )
        {
            ostr << ' ' << disp.team_[0].pen_score_
                 << ' ' << disp.team_[0].pen_miss_
                 << ' ' << disp.team_[0].pen_score_
                 << ' ' << disp.team_[0].pen_miss_;
        }
        ostr << ')';
    }

    ostr << " ((b)"
         << ' ' << quantize( disp.show_.ball_.x_, PREC )
         << ' ' << quantize( disp.show_.ball_.y_, PREC )
         << ' ' << quantize( disp.show_.ball_.vx_, PREC )
         << ' ' << quantize( disp.show_.ball_.vy_, PREC )
         << ')';

    for ( int i = 0; i < rcss::rcg::MAX_PLAYER*2; ++i )
    {
        const rcss::rcg::PlayerT & p = disp.show_.player_[i];

        ostr << " ((" << p.side_ << ' ' << p.unum_ << ')'
             << ' ' << p.type_
             << ' ' << std::hex << std::showbase << p.state_ << std::dec << std::noshowbase;
        ostr << ' ' << quantize( p.x_, PREC )
             << ' ' << quantize( p.y_, PREC )
             << ' ' << quantize( p.vx_, PREC )
             << ' ' << quantize( p.vy_, PREC )
             << ' ' << quantize( p.body_, DPREC )
             << ' ' << quantize( p.neck_, DPREC );
        if ( p.point_x_ != rcss::rcg::SHOWINFO_SCALE2F
             && p.point_x_ != rcss::rcg::SHOWINFO_SCALE2F )
        {
            ostr << ' ' << quantize( p.point_x_, PREC )
                 << ' ' << quantize( p.point_y_, PREC );
        }
        ostr << " (v " << p.view_quality_ << ' ' << quantize( p.view_width_, DPREC ) << ')';
        ostr << " (s "
             << quantize( p.stamina_, 0.001f ) << ' '
             << quantize( p.effort_, 0.0001f ) << ' '
             << quantize( p.recovery_, 0.0001f );
        if ( monitor_version >= 4
             || doGetLogVersion() == rcss::rcg::REC_VERSION_5 )
        {
            ostr << ' ' << quantize( p.stamina_capacity_, 0.001f );
        }
        ostr << ')';

        ostr << " (c "
             << p.kick_count_ << ' '
             << p.dash_count_ << ' '
             << p.turn_count_ << ' '
             << p.catch_count_ << ' '
             << p.move_count_ << ' '
             << p.turn_neck_count_ << ' '
             << p.change_view_count_ << ' '
             << p.say_count_ << ' '
             << p.tackle_count_ << ' '
             << p.pointto_count_ << ' '
             << p.attentionto_count_ << ')';
        ostr << ')';
    }
    ostr << ')';

    msg = ostr.str();
}


void
Player::serializePlayerType( const rcss::rcg::PlayerTypeT & param,
                             std::string & msg )
{
    std::ostringstream ostr;

    param.print( ostr );

    msg = ostr.str();
}


void
Player::serializePlayerParam( std::string & msg )
{
    std::ostringstream ostr;

    M_player_param.print( ostr );

    msg = ostr.str();
}


void
Player::serializeServerParam( std::string & msg )
{
    std::ostringstream ostr;

    M_server_param.print( ostr );

    msg = ostr.str();
}


void
Player::doHandleLogVersion( int ver )
{
    M_version = ver;

    // reset variables
    M_server_param = rcss::rcg::ServerParamT();
    M_player_param = rcss::rcg::PlayerParamT();
    M_player_types.clear();

    M_dispinfo_cache.clear();

    M_playmode = rcss::rcg::PM_Null;
    M_teams[0].clear();
    M_teams[1].clear();

    // write to output file
    if ( M_out_strm.is_open() )
    {
        if ( ver == rcss::rcg::REC_VERSION_2
             || ver == rcss::rcg::REC_VERSION_3 )
        {
            char buf[5];
            buf[0] = 'U';
            buf[1] = 'L';
            buf[2] = 'G';
            buf[3] = static_cast< char >( ver );
            M_out_strm.write( buf, 4 );
        }
        else if ( ver == rcss::rcg::REC_VERSION_4 )
        {
            M_out_strm << "ULG4\n";
        }
        else if ( ver == rcss::rcg::REC_VERSION_5 )
        {
            M_out_strm << "ULG5\n";
        }
    }
}

void
Player::doHandleShowInfo( const rcss::rcg::ShowInfoT & info )
{
    if ( M_dispinfo_cache.size() > MAX_SHOWINFO )
    {
        return;
    }

    //
    // register new disp info
    //

    boost::shared_ptr< rcss::rcg::DispInfoT > disp( new rcss::rcg::DispInfoT );

    disp->pmode_ = M_playmode;
    disp->team_[0] = M_teams[0];
    disp->team_[1] = M_teams[1];
    disp->show_ = info;

    M_dispinfo_cache.push_back( disp );
}

void
Player::doHandleMsgInfo( const int,
                         const int,
                         const std::string & )
{

}

void
Player::doHandlePlayMode( const int,
                          const rcss::rcg::PlayMode pm )
{
    M_playmode = pm;
}

void
Player::doHandleTeamInfo( const int,
                          const rcss::rcg::TeamT & team_l,
                          const rcss::rcg::TeamT & team_r )
{
    M_teams[0] = team_l;
    M_teams[1] = team_r;
}


void
Player::doHandlePlayerType( const rcss::rcg::PlayerTypeT & param )
{
    M_player_types.push_back( param );

    if ( M_out_strm.is_open() )
    {
        if ( doGetLogVersion() == rcss::rcg::REC_VERSION_3 )
        {
            rcss::rcg::Int16 mode = htons( rcss::rcg::PT_MODE );
            rcss::rcg::player_type_t info;
            rcss::rcg::convert( param, info );
            M_out_strm.write( reinterpret_cast< const char * >( &mode ),
                              sizeof( mode ) );
            M_out_strm.write( reinterpret_cast< const char * >( &info ),
                              sizeof( rcss::rcg::player_type_t ) );
        }
        else if ( doGetLogVersion() >= rcss::rcg::REC_VERSION_4 )
        {
            param.print( M_out_strm ) << '\n';
        }
    }
}


void
Player::doHandlePlayerParam( const rcss::rcg::PlayerParamT & param )
{
    M_player_param = param;

    if ( M_out_strm.is_open() )
    {
        if ( doGetLogVersion() == rcss::rcg::REC_VERSION_3 )
        {
            rcss::rcg::Int16 mode = htons( rcss::rcg::PPARAM_MODE );
            rcss::rcg::player_params_t new_param;
            rcss::rcg::convert( param, new_param );
            M_out_strm.write( reinterpret_cast< const char * >( &mode ),
                              sizeof( mode ) );
            M_out_strm.write( reinterpret_cast< const char * >( &new_param ),
                              sizeof( rcss::rcg::player_params_t ) );
        }
        else if ( doGetLogVersion() >= rcss::rcg::REC_VERSION_4 )
        {
            param.print( M_out_strm ) << '\n';
        }
    }
}


void
Player::doHandleServerParam( const rcss::rcg::ServerParamT & param )
{
    M_server_param = param;

    if ( M_out_strm.is_open() )
    {
        if ( doGetLogVersion() == rcss::rcg::REC_VERSION_3 )
        {
            rcss::rcg::Int16 mode = htons( rcss::rcg::PARAM_MODE );
            rcss::rcg::server_params_t new_param;
            rcss::rcg::convert( param, new_param );
            M_out_strm.write( reinterpret_cast< const char * >( &mode ),
                              sizeof( mode ) );
            M_out_strm.write( reinterpret_cast< const char * >( &new_param ),
                              sizeof( rcss::rcg::server_params_t ) );
        }
        else if ( doGetLogVersion() >= rcss::rcg::REC_VERSION_4 )
        {
            param.print( M_out_strm ) << '\n';
        }
    }
}
