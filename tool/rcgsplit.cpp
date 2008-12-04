// -*-c++-*-

/*!
  \file rcgsplit.cpp
  \date 2007-05-11
  \brief rcg splitter source file
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

#include <rcsslogplayer/parser.h>
#include <rcsslogplayer/handler.h>
#include <rcsslogplayer/util.h>

#ifdef HAVE_LIBZ
#include <rcsslogplayer/gzfstream.h>
#endif

#ifdef HAVE_BOOST_PROGRAM_OPTIONS
#include <boost/program_options.hpp>
#endif

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <cmath>
#include <cstdio>

#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif

namespace rcss {
namespace rcg {


class RCGSplitter
    : public Handler {
private:
    // options
    std::string M_filepath;
    bool M_verbose;
    int M_span_cycle;
    int M_segment_start_cycle;
    int M_segment_end_cycle;

    // game log data
    int M_version;

    ServerParamT M_server_param;
    PlayerParamT M_player_param;
    std::vector< PlayerTypeT > M_player_types;

    int M_time;
    PlayMode M_playmode;
    TeamT M_team_l;
    TeamT M_team_r;

    // output file info
    int M_start_cycle;
    std::ofstream M_fout;

public:
    RCGSplitter()
        : M_span_cycle( 12000 ),
          M_segment_start_cycle( -1 ),
          M_segment_end_cycle( -1 ),
          M_version( 0 ),
          M_start_cycle( 0 )
      { }

    bool parseCmdLine( int argc,
                       char ** argv );

    const
    std::string & filepath() const
      {
          return M_filepath;
      }

    int currentTime() const
      {
          return M_time;
      }
private:

    virtual
    void doHandleLogVersion( int ver );

    virtual
    int doGetLogVersion() const
      {
          return M_version;
      }

    virtual
    void doHandleShowInfo( const ShowInfoT & );

    virtual
    void doHandleMsgInfo( const int,
                          const int,
                          const std::string & );

    virtual
    void doHandlePlayMode( const int,
                           const PlayMode );

    virtual
    void doHandleTeamInfo( const int,
                           const TeamT &,
                           const TeamT & );

    virtual
    void doHandleDrawClear( const int )
      { };

    virtual
    void doHandleDrawPointInfo( const int,
                                const PointInfoT & )
      { }

    virtual
    void doHandleDrawCircleInfo( const int,
                                 const CircleInfoT & )
      { }

    virtual
    void doHandleDrawLineInfo( const int,
                               const LineInfoT & )
      { }

    virtual
    void doHandlePlayerType( const PlayerTypeT & );

    virtual
    void doHandlePlayerParam( const PlayerParamT & );

    virtual
    void doHandleServerParam( const ServerParamT & );

    virtual
    void doHandleEOF();


    // utility

    bool createOutputFile( const int cycle );
    void printHeader();

    void printShowV4( const ShowInfoT & show );
    void printShowV3( const ShowInfoT & show );
    void printShowV2( const ShowInfoT & show );
    void printShowOld( const ShowInfoT & show );
};

/*--------------------------------------------------------------------*/
bool
RCGSplitter::parseCmdLine( int argc,
                           char ** argv )
{
#ifdef HAVE_BOOST_PROGRAM_OPTIONS
    namespace po = boost::program_options;

    po::options_description visibles( "Allowed options:" );

    visibles.add_options()
        ( "help,h",
          "generates this message." )
        ( "verbose",
          po::bool_switch( &M_verbose )->default_value( false ),
          "verbose output mode." )
        ( "span-cycle,c",
          po::value< int >( &M_span_cycle )->default_value( 12000, "12000"  ),
          "set a split span cycle value" )
        ( "segment-start,s",
          po::value< int >( &M_segment_start_cycle )->default_value( -1, "-1"  ),
          "set a segment start cycle value. (negative value means the first cycle in the input file)" )
        ( "segment-end,e",
          po::value< int >( &M_segment_end_cycle )->default_value( -1, "-1"  ),
          "set a segment end cycle value. (negative value means the end cycle in the input file)" )
        ;

    po::options_description invisibles( "Invisibles" );
    invisibles.add_options()
        ( "file",
          po::value< std::string >( &M_filepath )->default_value( "" ),
          "set the path to Game Log file(.rcg) to be loaded."  )
        ;

    po::options_description all_desc( "All options:" );
    all_desc.add( visibles ).add( invisibles );

    po::positional_options_description pdesc;
    pdesc.add( "file", 1 ); // allowed only one rcg file

    bool help = false;
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
    }
    catch ( std::exception & e )
    {
        std::cerr << e.what() << std::endl;
        help = true;
    }

    if ( help
         || M_filepath.empty() )
    {
#ifdef HAVE_LIBZ
        std::cout << "Usage: rcgsplit [options ... ] [<GameLogFile>[.gz]]\n";
#else
        std::cout << "Usage: rcgsplit [options ... ] [<GameLogFile>]\n";
#endif
        std::cout << visibles << std::endl;
        return false;
    }
    return true;
#else // HAVE_BOOST_PROGRAM_OPTIONS
    return false
#endif
}

/*--------------------------------------------------------------------*/
void
RCGSplitter::doHandleLogVersion( int ver )
{
    M_version = ver;

    if ( M_verbose )
    {
        std::cout << "Game Log Version = " << ver << std::endl;
    }
}

/*--------------------------------------------------------------------*/
void
RCGSplitter::doHandleShowInfo( const ShowInfoT & show )
{
    M_time = show.time_;

    if ( M_version == rcss::rcg::REC_VERSION_4 )
    {
        printShowV4( show );
    }
    else if ( M_version == rcss::rcg::REC_VERSION_3 )
    {
        printShowV3( show );
    }
    else if ( M_version == rcss::rcg::REC_VERSION_2 )
    {
        printShowV2( show );
    }
    else
    {
        printShowOld( show );
    }
}

/*--------------------------------------------------------------------*/
void
RCGSplitter::printShowV4( const ShowInfoT & show )
{
    static const char * s_playmode_strings[] = PLAYMODE_STRINGS;

    static PlayMode s_playmode = PM_Null;
    static TeamT s_teams[2];

    bool new_file = createOutputFile( show.time_ );

    if ( ! M_fout.is_open() )
    {
        return;
    }

    std::ostream & os = M_fout;

    if ( new_file
         || s_playmode != M_playmode )
    {
        s_playmode = M_playmode;

        os << "(playmode " << M_time
           << " " << s_playmode_strings[s_playmode]
           << ")\n";
    }

    if  ( new_file
          || ! s_teams[0].equals( M_team_l )
          || ! s_teams[1].equals( M_team_r ) )
    {
        s_teams[0] = M_team_l;
        s_teams[1] = M_team_r;

        os << "(team " << M_time
           << ' ' << ( M_team_l.name_.empty() ? "null" : M_team_l.name_.c_str() )
           << ' ' << ( M_team_r.name_.empty() ? "null" : M_team_r.name_.c_str() )
           << ' ' << M_team_l.score_
           << ' ' << M_team_r.score_
           << ' ' << M_team_l.pen_score_
           << ' ' << M_team_r.pen_score_
           << ' ' << M_team_l.pen_miss_
           << ' ' << M_team_r.pen_miss_
           << ")\n";
    }


    os << "(show " << M_time;

    // ball
    os << "((b)"
       << ' ' << show.ball_.x_
       << ' ' << show.ball_.y_
       << ' ' << show.ball_.vx_
       << ' ' << show.ball_.vy_
       << ')';

    // players
    for ( int i = 0; i < MAX_PLAYER*2; ++i )
    {
        const PlayerT & p = show.player_[i];

        os << " ((" << p.side_ << ' ' << p.unum_ << ')';
        os << ' ' << p.type_;
        os << ' ' << std::hex << std::showbase << p.state_ << std::dec << std::noshowbase;

        os << ' ' << p.x_ << ' ' << p.y_
           << ' ' << p.vx_ << ' ' << p.vy_
           << ' ' << p.body_ << ' ' << p.neck_;
        if ( p.point_x_ != SHOWINFO_SCALE2F
             && p.point_y_ != SHOWINFO_SCALE2F )
        {
            os << ' ' << p.point_x_ << ' ' << p.point_y_;
        }


        os << " (v " << p.view_quality_ << ' ' << p.view_width_ << ')';

        os << " (s " << p.stamina_ << ' ' << p.effort_ << ' ' << p.recovery_ << ')';

        if ( p.focus_side_ != 'n' )
        {
            os << " (f" << p.focus_side_ << ' ' << p.focus_unum_ << ')';
        }

        os << " (c"
           << ' ' << p.kick_count_
           << ' ' << p.dash_count_
           << ' ' << p.turn_count_
           << ' ' << p.catch_count_
           << ' ' << p.move_count_
           << ' ' << p.turn_neck_count_
           << ' ' << p.change_view_count_
           << ' ' << p.say_count_
           << ' ' << p.tackle_count_
           << ' ' << p.pointto_count_
           << ' ' << p.attentionto_count_
           << ')';
        os << ')';
    }

    os << ")\n";
}

/*--------------------------------------------------------------------*/
void
RCGSplitter::printShowV3( const ShowInfoT & show )
{
    static PlayMode s_playmode = PM_Null;
    static TeamT s_teams[2];

    bool new_file = createOutputFile( show.time_ );

    if ( ! M_fout.is_open() )
    {
        return;
    }

    std::ostream & os = M_fout;

    Int16 mode;

    if ( new_file
         || s_playmode != M_playmode )
    {
        char pm = static_cast< char >( M_playmode );
        s_playmode = M_playmode;

        mode = htons( PM_MODE );
        os.write( reinterpret_cast< const char * >( &mode ),
                  sizeof( Int16 ) );
        os.write( reinterpret_cast< const char * >( &pm ),
                  sizeof( char ) );
    }

    if  ( new_file
          || ! s_teams[0].equals( M_team_l )
          || ! s_teams[1].equals( M_team_r ) )
    {
        team_t teams[2];
        convert( M_team_l, teams[0] );
        convert( M_team_r, teams[1] );
        s_teams[0] = M_team_l;
        s_teams[1] = M_team_r;

        mode = htons( TEAM_MODE );
        os.write( reinterpret_cast< const char * >( &mode ),
                  sizeof( mode ) );
        os.write( reinterpret_cast< const char * >( teams ),
                  sizeof( team_t ) * 2 );
    }

    short_showinfo_t2 new_show;

    convert( show, new_show );

    mode = htons( SHOW_MODE );
    os.write( reinterpret_cast< const char * >( &mode ),
              sizeof( Int16 ) );
    os.write( reinterpret_cast< const char * >( &new_show ),
              sizeof( short_showinfo_t2 ) );

}

/*--------------------------------------------------------------------*/
void
RCGSplitter::printShowV2( const ShowInfoT & show )
{
    createOutputFile( show.time_ );

    if ( ! M_fout.is_open() )
    {
        return;
    }

    std::ostream & os = M_fout;

    showinfo_t new_show;

    convert( static_cast< char >( M_playmode ),
             M_team_l,
             M_team_r,
             show,
             new_show );

    Int16 mode = htons( SHOW_MODE );
    os.write( reinterpret_cast< const char * >( &mode ),
              sizeof( Int16 ) );
    os.write( reinterpret_cast< const char * >( &new_show ),
              sizeof( showinfo_t ) );
}

/*--------------------------------------------------------------------*/
void
RCGSplitter::printShowOld( const ShowInfoT & show )
{
    createOutputFile( show.time_ );

    if ( ! M_fout.is_open() )
    {
        return;
    }

    std::ostream & os = M_fout;

    dispinfo_t disp;

    disp.mode = htons( SHOW_MODE );

    convert( static_cast< char >( M_playmode ),
             M_team_l,
             M_team_r,
             show,
             disp.body.show );

    os.write( reinterpret_cast< const char * >( &disp ),
              sizeof( dispinfo_t ) );
}

/*--------------------------------------------------------------------*/
void
RCGSplitter::doHandleMsgInfo( const int time,
                              const int board,
                              const std::string & msg )
{
    M_time = time;
    createOutputFile( M_time );

    if ( ! M_fout.is_open() )
    {
        return;
    }

    if ( M_version == REC_VERSION_4 )
    {
        M_fout << "(msg " << M_time
               << " " << board
               << " \"" << msg << "\")\n";
    }
    else if ( M_version == REC_VERSION_3
              || M_version == REC_VERSION_2 )
    {
        Int16 mode = htons( MSG_MODE );
        Int16 tmp_board = htons( static_cast< Int16 >( board ) );

        M_fout.write( reinterpret_cast< const char * >( &mode ),
                      sizeof( mode ) );
        M_fout.write( reinterpret_cast< const char * >( &tmp_board ),
                      sizeof( Int16 ) );
        Int16 nlen = htons( static_cast< short >( msg.length() + 1 ) );
        M_fout.write( reinterpret_cast< const char * >( &nlen ),
                      sizeof( Int16 ) );
        M_fout.write( msg.c_str(), msg.length() + 1 );
    }
    else
    {
        dispinfo_t disp;

        disp.mode = htons( MSG_MODE );

        disp.body.msg.board = htons( static_cast< Int16 >( board ) );
        std::memset( disp.body.msg.message, 0,
                     sizeof( disp.body.msg.message ) );
        std::strncpy( disp.body.msg.message,
                      msg.c_str(),
                      std::min( sizeof( disp.body.msg.message ) - 1,
                                msg.length() ) );
        M_fout.write( reinterpret_cast< const char * >( &disp ),
                      sizeof( dispinfo_t ) );
    }
}

/*--------------------------------------------------------------------*/
void
RCGSplitter::doHandleEOF()
{
    if ( M_fout.is_open() )
    {
        M_fout.flush();
        M_fout.close();
    }

    if ( M_verbose )
    {
        std::cout << "End." << std::endl;
    }
}

/*--------------------------------------------------------------------*/
void
RCGSplitter::doHandlePlayMode( const int time,
                               const PlayMode pm )
{
    M_time = time;
    M_playmode = pm;
}

/*--------------------------------------------------------------------*/
void
RCGSplitter::doHandleTeamInfo( const int time,
                               const TeamT & team_l,
                               const TeamT & team_r )
{
    M_time = time;
    M_team_l = team_l;
    M_team_r = team_r;
}

/*--------------------------------------------------------------------*/
void
RCGSplitter::doHandlePlayerType( const PlayerTypeT & param )
{
    M_player_types.push_back( param );
}

/*--------------------------------------------------------------------*/
void
RCGSplitter::doHandlePlayerParam( const PlayerParamT & param )
{
    M_player_param = param;
}

/*--------------------------------------------------------------------*/
void
RCGSplitter::doHandleServerParam( const ServerParamT & param )
{
    M_server_param = param;
}

/*--------------------------------------------------------------------*/
bool
RCGSplitter::createOutputFile( const int cycle )
{
    if ( M_segment_start_cycle > 0
         && cycle < M_segment_start_cycle )
    {
        M_fout.flush();
        M_fout.close();
        return false;
    }

    if ( M_segment_end_cycle > 0
         && M_segment_end_cycle < cycle )
    {
        M_fout.flush();
        M_fout.close();
        return false;
    }

    if ( M_start_cycle == 0
         || cycle >= M_start_cycle + M_span_cycle )
    {
        M_fout.flush();
        M_fout.close();

        M_start_cycle = cycle;

        char filename[256];
        snprintf( filename, 256, "%08d-%08d.rcg",
                  M_start_cycle, M_start_cycle + M_span_cycle - 1 );
        M_fout.open( filename,
                     std::ios_base::out | std::ios_base::binary );
        if ( M_verbose )
        {
            std::cout << "new file [" << filename << "]" << std::endl;
        }

        printHeader();

        return true;
    }

    return false;
}

/*--------------------------------------------------------------------*/
void
RCGSplitter::printHeader()
{
    if ( ! M_fout.is_open() )
    {
        return;
    }

    if ( M_version == REC_OLD_VERSION )
    {
        return;
    }

    if ( M_version == REC_VERSION_4 )
    {
        M_fout << "ULG4\n";
    }
    else
    {
        char header[5];
        header[0] = 'U';
        header[1] = 'L';
        header[2] = 'G';
        header[3] = static_cast< char >( M_version );

        M_fout.write( header, 4 );
    }

    if ( M_version == REC_VERSION_4 )
    {
        M_server_param.print( M_fout  ) << '\n';
        M_player_param.print( M_fout ) << '\n';
        for ( std::vector< PlayerTypeT >::iterator it = M_player_types.begin();
              it != M_player_types.end();
              ++it )
        {
            it->print( M_fout ) << '\n';
        }
    }
    else if ( M_version == REC_VERSION_3 )
    {
        Int16 mode;

        {
            server_params_t param;
            convert( M_server_param, param );
            mode = htons( PARAM_MODE );
            M_fout.write( reinterpret_cast< const char * >( &mode ),
                          sizeof( mode ) );
            M_fout.write( reinterpret_cast< const char * >( &param ),
                          sizeof( server_params_t ) );
        }

        {
            player_params_t param;
            convert( M_player_param, param );
            mode = htons( PPARAM_MODE );
            M_fout.write( reinterpret_cast< const char * >( &mode ),
                          sizeof( mode ) );
            M_fout.write( reinterpret_cast< const char * >( &param ),
                          sizeof( player_params_t ) );
        }

        mode = htons( PT_MODE );
        for ( std::vector< PlayerTypeT >::iterator it = M_player_types.begin();
              it != M_player_types.end();
              ++it )
        {
            player_type_t param;
            convert( *it, param );
            M_fout.write( reinterpret_cast< const char * >( &mode ),
                          sizeof( mode ) );
            M_fout.write( reinterpret_cast< const char * >( &param ),
                          sizeof( player_type_t ) );
        }
    }
}

}
}

/*--------------------------------------------------------------------*/

int
main( int argc, char ** argv )
{
    rcss::rcg::RCGSplitter splitter;

    if ( ! splitter.parseCmdLine( argc, argv ) )
    {
        return 1;
    }

#ifdef HAVE_LIBZ
    rcss::gzifstream fin( splitter.filepath().c_str() );
#else
    std::ifstream fin( splitter.filepath().c_str() );
#endif

    rcss::rcg::Parser parser( splitter );
    int count = 0;
    while ( parser.parse( fin ) )
    {
        if ( ++count % 500 == 0 )
        {
            std::fprintf( stdout, "parsing... %d\r",
                          splitter.currentTime() );
            std::fflush( stdout );
        }
    }

    return 0;
}
