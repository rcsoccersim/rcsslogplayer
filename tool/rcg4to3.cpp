// -*-c++-*-

/*!
  \file rcg4to3.cpp
  \brief rcg converter from version 4 to version 3
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

#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif
#ifdef HAVE_WINDOWS_H
#include <windows.h>
#endif

namespace rcss {
namespace rcg {


class RCG4to3
    : public Handler {
private:
    // options
    std::string M_input; //!< input file path
    std::string M_output; //!< output file path
    bool M_verbose;

    int M_version;
    int M_time;
    PlayMode M_playmode;
    TeamT M_team_l;
    TeamT M_team_r;

    // output file info
    std::ofstream M_fout;

public:
    RCG4to3()
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
RCG4to3::parseCmdLine( int argc,
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
        std::cout << "Usage: rcg4to3 [options ... ] <GameLogFile>[.gz]\n";
#else
        std::cout << "Usage: rcg4to3 [options ... ] <GameLogFile>\n";
#endif
        std::cout << visibles << std::endl;
        return false;
    }

    M_fout.open( M_output.c_str() );

    if ( ! M_fout.is_open() )
    {
        std::cerr << "rcg4to3: failed to opent the output file ." << M_output
                  << std::endl;
        return false;
    }

    return true;
#else // HAVE_BOOST_PROGRAM_OPTIONS
    std::cerr << "rcg4to3: boost::program_options is not available."
              << std::endl;
    return false;
#endif
}

/*--------------------------------------------------------------------*/
void
RCG4to3::doHandleLogVersion( int ver )
{
    M_version = ver;

    if ( ver != REC_VERSION_4
         && ver != REC_VERSION_5 )
    {
        std::cerr << "rcg4to3: Illegal game log version ." << ver
                  << std::endl;
        M_fout.close();
        std::exit( 1 );
        return;
    }

    if ( M_verbose )
    {
        std::cout << "rcg4to3: input game log version = " << ver
                  << std::endl;
    }

    char header[5];
    header[0] = 'U';
    header[1] = 'L';
    header[2] = 'G';
    header[3] = static_cast< char >( 3 );

    M_fout.write( header, 4 );
}

/*--------------------------------------------------------------------*/
void
RCG4to3::doHandleShowInfo( const ShowInfoT & show )
{
    static PlayMode s_playmode = PM_Null;
    static TeamT s_teams[2];

    if ( ! M_fout.is_open() )
    {
        return;
    }

    if ( M_verbose )
    {
        std::cout << "rcg4to3: handle show info. time = " << M_time
                  << std::endl;
    }

    M_time = show.time_;

    std::ostream & os = M_fout;

    Int16 mode;

    if ( s_playmode != M_playmode )
    {
        char pm = static_cast< char >( M_playmode );
        s_playmode = M_playmode;

        mode = htons( PM_MODE );
        os.write( reinterpret_cast< const char * >( &mode ),
                  sizeof( Int16 ) );
        os.write( reinterpret_cast< const char * >( &pm ),
                  sizeof( char ) );
    }

    if  ( ! s_teams[0].equals( M_team_l )
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
RCG4to3::doHandleMsgInfo( const int time,
                          const int board,
                          const std::string & msg )
{
    if ( ! M_fout.is_open() )
    {
        return;
    }

    if ( M_verbose )
    {
        std::cout << "rcg4to3: handle msg info. time = " << M_time
                  << std::endl;
    }

    M_time = time;

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

/*--------------------------------------------------------------------*/
void
RCG4to3::doHandleEOF()
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
RCG4to3::doHandlePlayMode( const int time,
                           const PlayMode pm )
{
    M_time = time;
    M_playmode = pm;
}

/*--------------------------------------------------------------------*/
void
RCG4to3::doHandleTeamInfo( const int time,
                           const TeamT & team_l,
                           const TeamT & team_r )
{
    M_time = time;
    M_team_l = team_l;
    M_team_r = team_r;
}

/*--------------------------------------------------------------------*/
void
RCG4to3::doHandlePlayerType( const PlayerTypeT & param )
{
    if ( ! M_fout.is_open() )
    {
        return;
    }

    Int16 mode = htons( PT_MODE );
    player_type_t p;
    convert( param, p );

    M_fout.write( reinterpret_cast< const char * >( &mode ),
                  sizeof( Int16 ) );
    M_fout.write( reinterpret_cast< const char * >( &p ),
                  sizeof( player_type_t ) );
}

/*--------------------------------------------------------------------*/
void
RCG4to3::doHandlePlayerParam( const PlayerParamT & param )
{
    if ( ! M_fout.is_open() )
    {
        return;
    }

    Int16 mode = htons( PPARAM_MODE );
    player_params_t p;
    convert( param, p );

    M_fout.write( reinterpret_cast< const char * >( &mode ),
                  sizeof( Int16 ) );
    M_fout.write( reinterpret_cast< const char * >( &p ),
                  sizeof( player_params_t ) );
}

/*--------------------------------------------------------------------*/
void
RCG4to3::doHandleServerParam( const ServerParamT & param )
{
    if ( ! M_fout.is_open() )
    {
        return;
    }

    Int16 mode = htons( PARAM_MODE );
    server_params_t p;
    convert( param, p );

    M_fout.write( reinterpret_cast< const char * >( &mode ),
                  sizeof( Int16 ) );
    M_fout.write( reinterpret_cast< const char * >( &p ),
                  sizeof( server_params_t ) );
}

}
}

/*--------------------------------------------------------------------*/

int
main( int argc, char ** argv )
{
    rcss::rcg::RCG4to3 converter;

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
        std::cerr << "rcg4to3: failed to opent the input file ."
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
