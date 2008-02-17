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
 Copyright (C) 2000, 2001 RoboCup Soccer Server Maintainance Group.
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

#include "controler.h"
#include "types.h"
#include "utility.h"

#ifdef HAVE_LIBZ
#include <rcssbase/gzip/gzfstream.hpp>
#endif

#include <boost/program_options.hpp>

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

#include <sys/time.h>

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
Int16
hdtons( const double & val )
{
    return htons( static_cast< Int16 >( rint( val * SHOWINFO_SCALE ) ) );
}

inline
Int32
hdtonl( const double & val )
{
    return htonl( static_cast< Int32 >( rint( val * SHOWINFO_SCALE2 ) ) );
}

inline
double
nltohd( const Int32 & val )
{
    return ( static_cast< double >( static_cast< Int32 >( ntohl( val ) ) )
             / SHOWINFO_SCALE2 );
}

inline
double
Quantize( const double & v,
          const double & q )
{
    return rint( v / q ) * q;
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
const int Player::MAX_SHOWINFO = 18000;


Player::Player()
    : M_show_index( 0 )
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
    if ( ! M_controler )
    {
        return;
    }

    if ( ! parseCmdLine( argc, argv ) )
    {
        return;
    }

    if ( ! openGameLog() )
    {
        return;
    }

    if ( ! openSavedLog() )
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
        ( "file",
          po::value< std::string >(),
          "configuration file" )
        ( "port,p",
          po::value< int >( &port_number )->default_value( 6000, "6000" ),
          "connection port number" )
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
          "set the path to Game Log file(.rcg) to be loaded."  )
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
    if ( ! M_showinfo2_cache.empty() )
    {
        if ( M_show_index < M_showinfo2_cache.size() )
        {
            ++M_show_index;
            sendLog( M_show_index );
            writeLog( M_show_index );
        }
        else //if ( M_show_index == M_showinfo2_cache.size() )
        {
            stop();
        }
    }
    else if ( ! M_showinfo_cache.empty() )
    {
        if ( M_show_index < M_showinfo_cache.size() )
        {
            ++M_show_index;
            sendLog( M_show_index );
            writeLog( M_show_index );
        }
        else
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
    if ( ! M_showinfo2_cache.empty() )
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
    else if ( ! M_showinfo_cache.empty() )
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



struct ShowinfoCmp
    : public std::binary_function< boost::shared_ptr< showinfo_t >,
                                   boost::shared_ptr< showinfo_t >,
                                   bool > {
    result_type operator()( const first_argument_type & lhs,
                            const second_argument_type & rhs )
      {
          return ntohs( lhs->time ) < ntohs( rhs->time );
      }
};

struct Showinfo2Cmp
    : public std::binary_function< boost::shared_ptr< showinfo_t2 >,
                                   boost::shared_ptr< showinfo_t2 >,
                                   bool > {
    result_type operator()( const first_argument_type & lhs,
                            const second_argument_type & rhs )
      {
          return ntohs( lhs->time ) < ntohs( rhs->time );
      }
};

void
Player::jump()
{
    int max_time = ( ! M_showinfo2_cache.empty()
                     ? ntohs( M_showinfo2_cache.back()->time )
                     : ! M_showinfo_cache.empty()
                     ? ntohs( M_showinfo_cache.back()->time )
                     : 0 );

    if ( M_to_time == END )
    {
        if ( ! M_showinfo2_cache.empty() )
        {
            M_show_index = M_showinfo2_cache.size();
            sendLog( M_show_index );
            writeLog( M_show_index );
        }
        else if ( ! M_showinfo_cache.empty() )
        {
            M_show_index = M_showinfo_cache.size();
            sendLog( M_show_index );
            writeLog( M_show_index );
        }
    }
    else if ( M_to_time <= max_time )
    {
        if ( ! M_showinfo2_cache.empty() )
        {
            boost::shared_ptr< showinfo_t2 > tmp( new showinfo_t2 );
            tmp->time = htons( (Int16)M_to_time );
            std::vector< boost::shared_ptr< showinfo_t2 > >::iterator it
                = std::lower_bound( M_showinfo2_cache.begin(),
                                    M_showinfo2_cache.end(),
                                    tmp,
                                    Showinfo2Cmp() );
            if ( it != M_showinfo2_cache.end() )
            {
                M_show_index = std::distance( M_showinfo2_cache.begin(), it ) + 1;
                sendLog( M_show_index );
                writeLog( M_show_index );
            }
        }
        else if ( ! M_showinfo_cache.empty() )
        {
            boost::shared_ptr< showinfo_t > tmp( new showinfo_t );
            tmp->time = htons( (Int16)M_to_time );
            std::vector< boost::shared_ptr< showinfo_t > >::iterator it
                = std::lower_bound( M_showinfo_cache.begin(),
                                    M_showinfo_cache.end(),
                                    tmp,
                                    ShowinfoCmp() );
            if ( it != M_showinfo_cache.end() )
            {
                M_show_index = std::distance( M_showinfo_cache.begin(), it ) + 1;
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
        int n = M_port.recv_info();
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
            std::snprintf( cycle, 15, "jump to %d : ", M_to_time );
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

    if ( M_version == REC_VERSION_4 )
    {

    }
    else if ( M_version == REC_VERSION_3 )
    {
        static char pm = static_cast< char >( 0 );
        static team_t teams[2] = { { "", 0 },
                                   { "", 0 } };

        if ( M_showinfo2_cache.size() < index )
        {
            return;
        }

        boost::shared_ptr< showinfo_t2 > show2 = M_showinfo2_cache[index - 1];

        // if playmode has changed wirte playmode
        if ( pm != show2->pmode )
        {
            pm = show2->pmode;
            Int16 mode = htons( PM_MODE );
            M_out_strm.write( reinterpret_cast< const char * >( &mode ),
                              sizeof( mode ) );
            M_out_strm.write( reinterpret_cast< const char * >( &pm ),
                              sizeof( pm ) );
        }

        // if teams or score has changed, write teams and score
        if ( std::strcmp( teams[0].name, show2->team[0].name )
             || teams[0].score != show2->team[0].score
             || std::strcmp( teams[1].name, show2->team[1].name )
             || teams[1].score != show2->team[1].score
             )
        {
            std::memcpy( teams[0].name, show2->team[0].name, 16 );
            teams[0].score = show2->team[0].score;
            std::memcpy( teams[1].name, show2->team[1].name, 16 );
            teams[1].score = show2->team[1].score;

            Int16 mode = htons( TEAM_MODE );
            M_out_strm.write( reinterpret_cast< const char * >( &mode ),
                              sizeof( mode ) );
            M_out_strm.write( reinterpret_cast< const char * >( teams ),
                              sizeof( team_t ) * 2 );
        }

        // write positional data
        short_showinfo_t2 short_show2;
        short_show2.ball = show2->ball;
        for ( int i = 0; i < MAX_PLAYER * 2; ++i )
        {
            short_show2.pos[i] = show2->pos[i];
        }
        short_show2.time = show2->time;

        Int16 mode = htons( SHOW_MODE );
        M_out_strm.write( reinterpret_cast< const char * >( &mode ),
                          sizeof( mode ) );
        M_out_strm.write( reinterpret_cast< const char * >( &short_show2 ),
                          sizeof( short_showinfo_t2 ) );
    }
    else if ( M_version == REC_VERSION_2 )
    {
        if ( M_showinfo_cache.size() < index )
        {
            return;
        }

        boost::shared_ptr< showinfo_t > show = M_showinfo_cache[index - 1];

        Int16 mode = htons( SHOW_MODE );
        M_out_strm.write( reinterpret_cast< const char * >( &mode ),
                          sizeof( mode ) );
        M_out_strm.write( reinterpret_cast< const char * >( &(*show) ),
                          sizeof( showinfo_t ) );
    }
}

void
Player::sendLog( const std::size_t index )
{
    if ( index == 0 )
    {
        return;
    }

    dispinfo_t disp;
    dispinfo_t2 disp2;

    disp.mode = htons( SHOW_MODE );
    if ( index <= M_showinfo_cache.size() )
    {
        disp.body.show = *M_showinfo_cache[index - 1];
        M_current = ntohs( disp.body.show.time );
    }

    disp2.mode = htons( SHOW_MODE );
    if ( index <= M_showinfo2_cache.size() )
    {
        disp2.body.show = *M_showinfo2_cache[index - 1];
        M_current = ntohs( disp2.body.show.time );
    }

    int counter = 0;
    for ( std::vector< Monitor >::const_iterator p = M_port.monitors().begin();
          p != M_port.monitors().end();
          ++p )
    {
        ++counter;
        if ( p->version_ == 1 )
        {
            if ( index <= M_showinfo_cache.size() )
            {
                M_port.send_info( disp, p->addr_ );
            }
        }
        else if ( p->version_ == 2 )
        {
            if ( index <= M_showinfo2_cache.size() )
            {
                M_port.send_info( disp2, p->addr_ );
            }
        }
        else if ( p->version_ == 3 )
        {
            if ( index <= M_showinfo2_cache.size() )
            {
                std::string msg;
                serializeShow( disp2.body.show, msg );
                M_port.send_info( msg, p->addr_ );
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
    rcss::gz::gzifstream fin( M_input_file.c_str() );
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

    try
    {
        rcss::RCGParser parser( *this );

        while ( parser.parse( fin ) )
        {
            // do nothing
        }
    }
    catch ( const std::string & e )
    {
        std::cerr << "Error reading log: " << e << std::endl;
        return false;
    }

    std::cout << "showinfo size = " << M_showinfo_cache.size()
              << std::endl;
    if ( ! M_showinfo_cache.empty() )
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
    if ( M_version < REC_VERSION_3 )
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
                dispinfo_t2 disp2;

                disp2.mode = htons( PARAM_MODE );
                disp2.body.sparams = M_server_param;
                M_port.send_info( disp2, p->addr_ );

                disp2.mode = htons( PPARAM_MODE );
                disp2.body.pparams = M_player_param;
                M_port.send_info( disp2, p->addr_ );

                disp2.mode = htons( PT_MODE );
                for ( std::vector< player_type_t >::iterator it = M_player_types.begin();
                      it != M_player_types.end();
                      ++it )
                {
                    disp2.body.ptinfo = *it;
                    M_port.send_info( disp2, p->addr_ );
                }
            }
            else if ( p->version_ == 3 )
            {
                std::string msg;
                serializeServerParam( msg );
                M_port.send_info( msg, p->addr_ );

                serializePlayerParam( msg );
                M_port.send_info( msg, p->addr_ );

                for ( std::vector< player_type_t >::iterator it = M_player_types.begin();
                      it != M_player_types.end();
                      ++it )
                {
                    serializePlayerType( *it, msg );
                    M_port.send_info( msg, p->addr_ );
                }
            }

            S_sent_set.insert( &(*p) );
        }
    }
}


void
Player::serializeShow( const showinfo_t2 & show,
                       std::string & msg )
{
    const double PREC = 0.0001;
    const double DPREC = 0.001;

    std::ostringstream ostr;

    ostr << "(show " << htons( show.time );
    ostr << " (pm " << static_cast< int >( show.pmode ) << ")";
    ostr << " (tm"
         << ' ' << M_teams_v4[0].name
         << ' ' << M_teams_v4[1].name
         << ' ' << M_teams_v4[0].score
         << ' ' << M_teams_v4[1].score;
    if ( M_teams_v4[0].pen_score + M_teams_v4[0].pen_miss > 0
         || M_teams_v4[1].pen_score + M_teams_v4[1].pen_miss > 0 )
    {
        ostr << ' ' << M_teams_v4[0].pen_score
             << ' ' << M_teams_v4[0].pen_miss
             << ' ' << M_teams_v4[1].pen_score
             << ' ' << M_teams_v4[1].pen_miss;
    }
    ostr << ')';

    ostr << " ((b)"
         << ' ' << Quantize( nltohd( show.ball.x ), PREC )
         << ' ' << Quantize( nltohd( show.ball.y ), PREC )
         << ' ' << Quantize( nltohd( show.ball.deltax ), PREC )
         << ' ' << Quantize( nltohd( show.ball.deltax ), PREC )
         << ')';

    for ( int i = 0; i < MAX_PLAYER*2; ++i )
    {
        char side = ( i < MAX_PLAYER ? 'l' : 'r' );
        int unum = ( i < MAX_PLAYER ? i + 1 : i + 1 - MAX_PLAYER );
        ostr << " ("
             << '(' << side << ' ' << unum << ')'
             << ' ' << ntohs( show.pos[i].type )
             << ' ' << std::hex << std::showbase
             << static_cast< long >( ntohs( show.pos[i].mode ) )
             << std::dec << std::noshowbase;
        ostr << ' ' << Quantize( nltohd( show.pos[i].x ), PREC )
             << ' ' << Quantize( nltohd( show.pos[i].y ), PREC )
             << ' ' << Quantize( nltohd( show.pos[i].deltax ), PREC )
             << ' ' << Quantize( nltohd( show.pos[i].deltay ), PREC )
             << ' ' << Quantize( Rad2Deg( nltohd( show.pos[i].body_angle ) ), DPREC )
             << ' ' << Quantize( Rad2Deg( nltohd( show.pos[i].head_angle ) ), DPREC );
        ostr << " (v "
             << ( ntohs( show.pos[i].view_quality ) ? "h " : "l " )
             << Quantize( Rad2Deg( nltohd( show.pos[i].view_width ) ), DPREC ) << ')';
        ostr << " (s "
             << nltohd( show.pos[i].stamina ) << ' '
             << nltohd( show.pos[i].effort ) << ' '
             << nltohd( show.pos[i].recovery ) << ')';
        ostr << " (c "
             << ntohs( show.pos[i].kick_count ) << ' '
             << ntohs( show.pos[i].dash_count ) << ' '
             << ntohs( show.pos[i].turn_count ) << ' '
             << ntohs( show.pos[i].catch_count ) << ' '
             << ntohs( show.pos[i].move_count ) << ' '
             << ntohs( show.pos[i].tneck_count ) << ' '
             << ntohs( show.pos[i].chg_view_count ) << ' '
             << ntohs( show.pos[i].say_count ) << ' '
             << 0 << ' ' // tackle
             << 0 << ' ' // pointto
             << 0 << ')'; // attentionto
        ostr << ')';
    }

    msg = ostr.str();
}

void
Player::serializeServerParam( std::string & msg )
{
    std::ostringstream ostr;

    ostr << "(server_param ";

    for ( std::map< std::string, std::string >::const_iterator it = M_server_param_map.begin();
          it != M_server_param_map.end();
          ++it )
    {
        ostr << " (" << it->first << ' ' << it->second << ")";
    }
    ostr << ")";

    msg = ostr.str();
}

void
Player::serializePlayerParam( std::string & msg )
{
    std::ostringstream ostr;

    ostr << "(player_param ";

    for ( std::map< std::string, std::string >::const_iterator it = M_player_param_map.begin();
          it != M_player_param_map.end();
          ++it )
    {
        ostr << " (" << it->first << ' ' << it->second << ")";
    }
    ostr << ')';

    msg = ostr.str();
}

void
Player::serializePlayerType( const player_type_t & param,
                             std::string & msg )
{
    std::ostringstream ostr;

    ostr << "(player_type "
         << "(id " << ntohs( param.id ) << ')'
         << "(player_speed_max " << nltohd( param.player_speed_max )  << ')'
         << "(stamina_inc_max " << nltohd( param.stamina_inc_max ) << ')'
         << "(player_decay " << nltohd( param.player_decay ) << ')'
         << "(inertia_moment " << nltohd( param.inertia_moment ) << ')'
         << "(dash_power_rate " << nltohd( param.dash_power_rate ) << ')'
         << "(player_size " << nltohd( param.player_size ) << ')'
         << "(kickable_margin " << nltohd( param.kickable_margin ) << ')'
         << "(kick_rand " << nltohd( param.kick_rand ) << ')'
         << "(extra_stamina " << nltohd( param.extra_stamina ) << ')'
         << "(effort_max " << nltohd( param.effort_max ) << ')'
         << "(effort_min " << nltohd( param.effort_min ) << ')'
         << ')';
    msg = ostr.str();
}


void
Player::doHandleLogVersion( int ver )
{
    M_version = ver;

    if ( ver >= REC_VERSION_2
         && M_out_strm.is_open() )
    {
        char buf[5];
        buf[0] = 'U';
        buf[1] = 'L';
        buf[2] = 'G';

        if ( ver >= REC_VERSION_4 )
        {
            int version_char = ver - static_cast< int >( '0' );
            buf[3] = static_cast< char >( version_char );
        }
        else
        {
            buf[3] = static_cast< char >( ver );
        }

        M_out_strm.write( buf, 4 );
    }
}

void
Player::doHandleDispInfo( std::streampos pos,
                          const dispinfo_t & info )
{
    if ( ntohs( info.mode ) == SHOW_MODE )
    {
        doHandleShowInfo( pos, info.body.show );
    }
}

void
Player::doHandleShowInfo( std::streampos pos,
                          const showinfo_t & info )
{
    if ( M_showinfo_cache.size() > MAX_SHOWINFO )
    {
        return;
    }

    //
    // update team
    //

    M_teams[0] = info.team[0];
    M_teams[1] = info.team[1];

    M_teams_v4[0].name = info.team[0].name;
    M_teams_v4[0].score = ntohs( info.team[0].score );
    M_teams_v4[1].name = info.team[1].name;
    M_teams_v4[1].score = ntohs( info.team[1].score );

    //
    // register new show 1
    //

    M_showinfo_cache.push_back( boost::shared_ptr< showinfo_t >
                                ( new showinfo_t( info ) ) );

    //
    // register new show 2
    //

    boost::shared_ptr< showinfo_t2 > show2( new showinfo_t2 );

    show2->pmode = info.pmode;
    show2->team[0] = info.team[0];
    show2->team[1] = info.team[1];
    show2->ball.x = nstonl( info.pos[0].x );
    show2->ball.y = nstonl( info.pos[0].y );

    for ( int i = 0; i < MAX_PLAYER * 2; ++i )
    {
        show2->pos[i].mode = info.pos[i+1].enable;
        show2->pos[i].body_angle = (Int32)htonl((Int32)(Deg2Rad( (double)(Int16)ntohs( info.pos[i+1].angle ) ) * SHOWINFO_SCALE2));
        show2->pos[i].x = nstonl( info.pos[i+1].x );
        show2->pos[i].y = nstonl( info.pos[i+1].y );
    }
    show2->time = info.time;

    M_showinfo2_cache.push_back( show2 );
}

void
Player::doHandleShowInfo( std::streampos pos,
                          const short_showinfo_t2 & info )
{
    if ( M_showinfo_cache.size() > MAX_SHOWINFO )
    {
        return;
    }

    //
    // register new show 1
    //

    boost::shared_ptr< showinfo_t > show( new showinfo_t );
    show->pmode = M_playmode;
    show->team[0] = M_teams[0];
    show->team[1] = M_teams[1];
    show->pos[0].x = nltons( info.ball.x );
    show->pos[0].y = nltons( info.ball.y );
    for ( int i = 0; i < MAX_PLAYER * 2; ++i )
    {
        show->pos[i+1].enable = info.pos[i].mode;
        show->pos[i+1].angle = htons((Int16)(Rad2Deg( (double)(Int32)ntohl( info.pos[i].body_angle ) ) / SHOWINFO_SCALE2) );
        show->pos[i+1].x = nltons( info.pos[i].x );
        show->pos[i+1].y = nltons( info.pos[i].y );
        show->pos[i+1].unum = htons((i % MAX_PLAYER) + 1);
        show->pos[i+1].side = htons(((i < MAX_PLAYER) ? LEFT : RIGHT ));
    }
    show->time = info.time;

    M_showinfo_cache.push_back( show );

    //
    // register new show 2
    //

    boost::shared_ptr< showinfo_t2 > show2( new showinfo_t2 );
    show2->pmode = M_playmode;
    show2->team[0] = M_teams[0];
    show2->team[1] = M_teams[1];
    show2->ball = info.ball;
    for ( int i = 0; i < MAX_PLAYER * 2; ++i )
    {
        show2->pos[i] = info.pos[i];
    }
    show2->time = info.time;

    M_showinfo2_cache.push_back( show2 );
}

void
Player::doHandleMsgInfo( std::streampos ,
                         Int16 ,
                         const std::string &  )
{

}

void
Player::doHandlePlayMode( std::streampos,
                          char pm )
{
    M_playmode = pm;
}

void
Player::doHandleTeamInfo( std::streampos,
                          const team_t & team_l,
                          const team_t & team_r )
{
    M_teams[0] = team_l;
    M_teams[1] = team_r;

    M_teams_v4[0].name = team_l.name;
    M_teams_v4[0].score = ntohs( team_l.score );
    M_teams_v4[1].name = team_r.name;
    M_teams_v4[1].score = ntohs( team_r.score );
}

void
Player::doHandleServerParams( std::streampos,
                              const server_params_t & params )
{
    M_server_param = params;

    if ( M_out_strm.is_open() )
    {
        Int16 mode = htons( PARAM_MODE );
        M_out_strm.write( reinterpret_cast< const char * >( &mode ),
                          sizeof( mode ) );
        M_out_strm.write( reinterpret_cast< const char * >( &params ),
                          sizeof( server_params_t ) );
    }
}

void
Player::doHandlePlayerParams( std::streampos,
                              const player_params_t & params )
{
    M_player_param = params;

    if ( M_out_strm.is_open() )
    {
        Int16 mode = htons( PPARAM_MODE );
        M_out_strm.write( reinterpret_cast< const char * >( &mode ),
                          sizeof( mode ) );
        M_out_strm.write( reinterpret_cast< const char * >( &params ),
                          sizeof( player_params_t ) );
    }
}


void
Player::doHandlePlayerType( std::streampos,
                            const player_type_t & info )
{
    M_player_types.push_back( info );

    if ( M_out_strm.is_open() )
    {
        Int16 mode = htons( PT_MODE );
        M_out_strm.write( reinterpret_cast< const char * >( &mode ),
                          sizeof( mode ) );
        M_out_strm.write( reinterpret_cast< const char * >( &info ),
                          sizeof( player_type_t ) );
    }
}



void
Player::doHandleShowBegin( const int time )
{
    if ( M_showinfo_cache.size() > MAX_SHOWINFO )
    {
        return;
    }

    // v1 data
    boost::shared_ptr< showinfo_t > show( new showinfo_t );
    show->pmode = M_playmode;
    show->team[0] = M_teams[0];
    show->team[1] = M_teams[1];
    show->time = htons( time );

    M_showinfo_cache.push_back( show );

    // create v2 data
    boost::shared_ptr< showinfo_t2 > show2( new showinfo_t2 );
    show2->pmode = M_playmode;
    show2->team[0] = M_teams[0];
    show2->team[1] = M_teams[1];
    show2->time = htons( time );

    M_showinfo2_cache.push_back( show2 );

}

void
Player::doHandleShowEnd()
{

}

void
Player::doHandleBall( const int time,
                      const BallT & ball )
{
    if ( M_showinfo_cache.empty()
         || M_showinfo2_cache.empty() )
    {
        return;
    }

    boost::shared_ptr< showinfo_t > show = M_showinfo_cache.back();
    boost::shared_ptr< showinfo_t2 > show2 = M_showinfo2_cache.back();
    if ( show->time != (Int16)htons( time )
         || show2->time != (Int16)htons( time ) )
    {
        std::cerr << "doHandleBall " << time << " time mismatch" << std::endl;
        return;
    }

    // v1 data
    show->pos[0].x = hdtons( ball.x );
    show->pos[0].y = hdtons( ball.y );

    // v2 data
    show2->ball.x = hdtonl( ball.x );
    show2->ball.y = hdtonl( ball.y );
    show2->ball.deltax = hdtonl( ball.vx );
    show2->ball.deltay = hdtonl( ball.vy );
}

void
Player::doHandlePlayer( const int time,
                        const PlayerT & player )
{
    if ( M_showinfo_cache.empty()
         || M_showinfo2_cache.empty() )
    {
        return;
    }

    boost::shared_ptr< showinfo_t > show = M_showinfo_cache.back();
    boost::shared_ptr< showinfo_t2 > show2 = M_showinfo2_cache.back();
    if ( show->time != (Int16)htons( time )
         || show2->time != (Int16)htons( time ) )
    {
        return;
    }

    const int idx = ( player.side == 'l'
                      ? player.unum - 1
                      : MAX_PLAYER + player.unum - 1 );
    if ( idx < 0 || MAX_PLAYER*2 <= idx )
    {
        return;
    }

    // v1 data
    show->pos[idx+1].enable = htons( player.state );
    show->pos[idx+1].side = htons( player.side == 'l' ? LEFT : RIGHT );
    show->pos[idx+1].unum = htons( player.unum );
    show->pos[idx+1].angle = htons( (Int16)player.body );
    show->pos[idx+1].x = hdtons( player.x );
    show->pos[idx+1].y = hdtons( player.y );

    // v2 data
    show2->pos[idx].mode = htons( player.state );
    show2->pos[idx].type = htons( player.type );
    show2->pos[idx].x = hdtonl( player.x );
    show2->pos[idx].y = hdtonl( player.y );
    show2->pos[idx].deltax = hdtonl( player.vx );
    show2->pos[idx].deltay = hdtonl( player.vy );
    show2->pos[idx].body_angle = hdtonl( Deg2Rad( player.body ) );
    show2->pos[idx].head_angle = hdtonl( Deg2Rad( player.neck ) );
    show2->pos[idx].view_width = hdtonl( Deg2Rad( player.view_width ) );
    show2->pos[idx].view_quality = htons( player.view_quality ? 1 : 0 );
    show2->pos[idx].stamina = hdtonl( player.stamina );
    show2->pos[idx].effort = hdtonl( player.effort );
    show2->pos[idx].recovery = hdtonl( player.recovery );
    show2->pos[idx].kick_count = htons( player.n_kick );
    show2->pos[idx].dash_count = htons( player.n_dash );
    show2->pos[idx].turn_count = htons( player.n_turn );
    show2->pos[idx].say_count = htons( player.n_say );
    show2->pos[idx].tneck_count = htons( player.n_turn_neck );
    show2->pos[idx].catch_count = htons( player.n_catch );
    show2->pos[idx].move_count = htons( player.n_move );
    show2->pos[idx].chg_view_count = htons( player.n_change_view );
}

void
Player::doHandleMsg( const int time,
                     const int board,
                     const char * msg )
{

}

void
Player::doHandlePlayMode( const int time,
                          const PlayMode pm )
{
    M_playmode = static_cast< char >( pm );
}

void
Player::doHandleTeam( const int time,
                      const TeamT & team_l,
                      const TeamT & team_r )
{
    std::snprintf( M_teams[0].name, 16, "%s", team_l.name.c_str() );
    M_teams[0].score = htons( team_l.score );
    std::snprintf( M_teams[1].name, 16, "%s", team_r.name.c_str() );
    M_teams[1].score = htons( team_r.score );

    M_teams_v4[0] = team_l;
    M_teams_v4[1] = team_r;
}

void
Player::doHandleServerParams( const std::map< std::string, std::string > & param_map )
{
    M_server_param_map = param_map;

    std::map< std::string, std::string >::const_iterator it;

    if ( ( it = param_map.find( "goal_width" ) ) != param_map.end() )
    {
        M_server_param.gwidth = hdtonl( std::atof( it->second.c_str() ) );
    }
    if ( ( it = param_map.find( "ball_size" ) ) != param_map.end() )
    {
        M_server_param.bsize = hdtonl( std::atof( it->second.c_str() ) );
    }
    if ( ( it = param_map.find( "ball_decay" ) ) != param_map.end() )
    {
        M_server_param.bdecay = hdtonl( std::atof( it->second.c_str() ) );
    }
    if ( ( it = param_map.find( "player_size" ) ) != param_map.end() )
    {
        M_server_param.psize = hdtonl( std::atof( it->second.c_str() ) );
    }
    if ( ( it = param_map.find( "kickable_margin" ) ) != param_map.end() )
    {
        M_server_param.kmargin = hdtonl( std::atof( it->second.c_str() ) );
    }

    if ( M_out_strm.is_open() )
    {
        std::string msg;
        serializeServerParam( msg );
        M_out_strm << msg << '\n';
    }
}

void
Player::doHandlePlayerParams( const std::map< std::string, std::string > & param_map )
{
    M_player_param_map = param_map;

    //M_player_param;

    if ( M_out_strm.is_open() )
    {
        std::string msg;
        serializePlayerParam( msg );
        M_out_strm << msg << '\n';
    }
}

void
Player::doHandlePlayerType( const std::map< std::string, std::string > & param_map )
{
    player_type_t param;

    std::map< std::string, std::string >::const_iterator it;

    if ( ( it = param_map.find( "id" ) ) != param_map.end() )
    {
        param.id = htons( std::atoi( it->second.c_str() ) );
    }
    if ( ( it = param_map.find( "player_speed_max" ) ) != param_map.end() )
    {
        param.player_speed_max = hdtonl( std::atof( it->second.c_str() ) );
    }
    if ( ( it = param_map.find( "stamina_inc_max" ) ) != param_map.end() )
    {
        param.stamina_inc_max = hdtonl( std::atof( it->second.c_str() ) );
    }
    if ( ( it = param_map.find( "player_decay" ) ) != param_map.end() )
    {
        param.player_decay = hdtonl( std::atof( it->second.c_str() ) );
    }
    if ( ( it = param_map.find( "inertia_moment" ) ) != param_map.end() )
    {
        param.inertia_moment = hdtonl( std::atof( it->second.c_str() ) );
    }
    if ( ( it = param_map.find( "dash_power_rate" ) ) != param_map.end() )
    {
        param.dash_power_rate = hdtonl( std::atof( it->second.c_str() ) );
    }
    if ( ( it = param_map.find( "player_size" ) ) != param_map.end() )
    {
        param.player_size = hdtonl( std::atof( it->second.c_str() ) );
    }
    if ( ( it = param_map.find( "kickable_margin" ) ) != param_map.end() )
    {
        param.kickable_margin = hdtonl( std::atof( it->second.c_str() ) );
    }
    if ( ( it = param_map.find( "kick_rand" ) ) != param_map.end() )
    {
        param.kick_rand = hdtonl( std::atof( it->second.c_str() ) );
    }
    if ( ( it = param_map.find( "extra_stamina" ) ) != param_map.end() )
    {
        param.extra_stamina = hdtonl( std::atof( it->second.c_str() ) );
    }
    if ( ( it = param_map.find( "effort_max" ) ) != param_map.end() )
    {
        param.effort_max = hdtonl( std::atof( it->second.c_str() ) );
    }
    if ( ( it = param_map.find( "effort_min" ) ) != param_map.end() )
    {
        param.effort_min = hdtonl( std::atof( it->second.c_str() ) );
    }

    M_player_types.push_back( param );

    if ( M_out_strm.is_open() )
    {
        std::string msg;
        serializePlayerType( param, msg );
        M_out_strm << msg << '\n';
    }
}
