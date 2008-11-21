// -*-c++-*-

/*!
  \file rcg3to4.cpp
  \brief rcg converter from version 3 to version 4
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
#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <cmath>

namespace {
inline
float
quantize( const float & val,
          const float & prec )
{
    return rintf( val / prec ) * prec;
}
}

namespace rcss {
namespace rcg {


class RCG3to4
    : public Handler {
private:
    // options
    std::string M_input; //!< input file path
    std::string M_output; //!< input file path
    bool M_verbose;

    int M_version;
    int M_time;
    PlayMode M_playmode;
    TeamT M_team_l;
    TeamT M_team_r;

    // output file info
    std::ofstream M_fout;

public:
    RCG3to4()
        : M_version( 0 )
        , M_time( -1 )
        , M_playmode( PM_Null )
      { }

    bool parseCmdLine( int argc,
                       char ** argv );

    const
    std::string & input() const
      {
          return M_input;
      }

    const
    std::string & output() const
      {
          return M_output;
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

};

/*--------------------------------------------------------------------*/
bool
RCG3to4::parseCmdLine( int argc,
                       char ** argv )
{
#ifdef HAVE_BOOST_PROGRAM_OPTIONS
    namespace po = boost::program_options;

    po::options_description visibles( "Allowed options" );

    visibles.add_options()
        ( "help,h",
          "print this message." )
        ( "verbose",
          po::bool_switch( &M_verbose )->default_value( false ),
          "verbose mode." )
        ( "output,o",
          po::value< std::string >( &M_output )->default_value( "" ),
          "set the path to Game Log file(.rcg) to be output."  )
        ;

    po::options_description invisibles( "Invisibles" );
    invisibles.add_options()
        ( "input",
          po::value< std::string >( &M_input )->default_value( "" ),
          "set the path to Game Log file(.rcg) to be opened."  )
        ;

    po::options_description all_desc( "All options:" );
    all_desc.add( visibles ).add( invisibles );

    po::positional_options_description pdesc;
    pdesc.add( "input", 1 ); // allowed only one rcg file

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
         || M_input.empty()
         || M_output.empty() )
    {
#ifdef HAVE_LIBZ
        std::cout << "Usage: rcg3to4 [options ... ] [<GameLogFile>[.gz]]\n";
#else
        std::cout << "Usage: rcg3to4 [options ... ] [<GameLogFile>]\n";
#endif
        std::cout << visibles << std::endl;
        return false;
    }

    M_fout.open( M_output.c_str() );

    if ( ! M_fout.is_open() )
    {
        std::cerr << "rcg3to4: failed to opent the output file ." << M_output
                  << std::endl;
        return false;
    }
    return true;
#else // HAVE_BOOST_PROGRAM_OPTIONS
    std::cerr << "rcg3to4: boost::program_options is not available."
              << std::endl;
    return false;
#endif

}

/*--------------------------------------------------------------------*/
void
RCG3to4::doHandleLogVersion( int ver )
{
    M_version = ver;

    if ( ver != REC_VERSION_3 )
    {
        std::cerr << "rcg3to4: Illegal game log version ." << ver
                  << std::endl;
        M_fout.close();
        std::exit( 1 );
        return;
    }

    if ( M_verbose )
    {
        std::cout << "rcg3to4: input game log version = " << ver
                  << std::endl;
    }

    M_fout << "ULG4\n";
}

/*--------------------------------------------------------------------*/
void
RCG3to4::doHandleShowInfo( const ShowInfoT & show )
{
    static const char * s_playmode_strings[] = PLAYMODE_STRINGS;

    static PlayMode s_playmode = PM_Null;
    static TeamT s_teams[2];

    if ( ! M_fout.is_open() )
    {
        return;
    }

    M_time = show.time_;

    if ( M_verbose )
    {
        std::cout << "rcg3to4: handle show info. time = " << M_time
                  << std::endl;
    }

    std::ostream & os = M_fout;

    if ( s_playmode != M_playmode )
    {
        s_playmode = M_playmode;

        os << "(playmode " << M_time
           << " " << s_playmode_strings[s_playmode]
           << ")\n";
    }

    if  ( ! s_teams[0].equals( M_team_l )
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
    os << " ((b)"
       << ' ' << quantize( show.ball_.x_, 0.0001 )
       << ' ' << quantize( show.ball_.y_, 0.0001 )
       << ' ' << quantize( show.ball_.vx_, 0.0001 )
       << ' ' << quantize( show.ball_.vy_, 0.0001 )
       << ')';

    // players
    for ( int i = 0; i < MAX_PLAYER*2; ++i )
    {
        const PlayerT & p = show.player_[i];

        os << " ((" << p.side_ << ' ' << p.unum_ << ')';
        os << ' ' << p.type_;
        os << ' ' << std::hex << std::showbase << p.state_ << std::dec << std::noshowbase;

        os << ' ' << quantize( p.x_, 0.0001 )
           << ' ' << quantize( p.y_, 0.0001 )
           << ' ' << quantize( p.vx_, 0.0001 )
           << ' ' << quantize( p.vy_, 0.0001 )
           << ' ' << quantize( p.body_, 0.001 )
           << ' ' << quantize( p.neck_, 0.001 );
        if ( p.isPointing() )
        {
            os << ' ' << quantize( p.point_x_, 0.0001 )
               << ' ' << quantize( p.point_y_, 0.0001 );
        }

        os << " (v " << p.view_quality_
           << ' ' << quantize( p.view_width_, 0.001 ) << ')';

        os << " (s " << quantize( p.stamina_, 0.0001 )
           << ' ' << quantize( p.effort_, 0.0001 )
           << ' ' << quantize( p.recovery_, 0.0001 )
           << ')';

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
RCG3to4::doHandleMsgInfo( const int time,
                          const int board,
                          const std::string & msg )
{
    if ( ! M_fout.is_open() )
    {
        return;
    }

    if ( M_verbose )
    {
        std::cout << "rcg3to4: handle msg info. time = " << M_time
                  << std::endl;
    }

    M_time = time;
    M_fout << "(msg " << M_time
           << " " << board
           << " \"" << msg << "\")\n";
}

/*--------------------------------------------------------------------*/
void
RCG3to4::doHandleEOF()
{
    if ( ! M_fout.is_open() )
    {
        return;
    }

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
RCG3to4::doHandlePlayMode( const int time,
                           const PlayMode pm )
{
    M_time = time;
    M_playmode = pm;
}

/*--------------------------------------------------------------------*/
void
RCG3to4::doHandleTeamInfo( const int time,
                           const TeamT & team_l,
                           const TeamT & team_r )
{
    M_time = time;
    M_team_l = team_l;
    M_team_r = team_r;
}

/*--------------------------------------------------------------------*/
void
RCG3to4::doHandlePlayerType( const PlayerTypeT & param )
{
    if ( ! M_fout.is_open() )
    {
        return;
    }

    param.print( M_fout ) << '\n';
}

/*--------------------------------------------------------------------*/
void
RCG3to4::doHandlePlayerParam( const PlayerParamT & param )
{
    if ( ! M_fout.is_open() )
    {
        return;
    }

    param.print( M_fout ) << '\n';
}

/*--------------------------------------------------------------------*/
void
RCG3to4::doHandleServerParam( const ServerParamT & param )
{
    if ( ! M_fout.is_open() )
    {
        return;
    }

    param.print( M_fout ) << '\n';
}

}
}

/*--------------------------------------------------------------------*/

int
main( int argc, char ** argv )
{
    rcss::rcg::RCG3to4 converter;

    if ( ! converter.parseCmdLine( argc, argv ) )
    {
        return 1;
    }

#ifdef HAVE_LIBZ
    rcss::gzifstream fin( converter.input().c_str() );
#else
    std::ifstream fin( converter.input().c_str() );
#endif

    if ( ! fin.is_open() )
    {
        std::cerr << "rcg3to4: failed to opent the input file ."
                  << converter.input()
                  << std::endl;
        return 1;
    }

    rcss::rcg::Parser parser( converter );
    int count = -1;
    while ( parser.parse( fin ) )
    {
        if ( ++count % 512 == 0 )
        {
            std::fprintf( stdout, "parsing... %d\r", count );
            std::fflush( stdout );
        }
    }

    return 0;
}
