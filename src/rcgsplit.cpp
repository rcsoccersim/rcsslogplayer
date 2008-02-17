// -*-c++-*-

/***************************************************************************
                                rcgsplit.cpp
                         rcg splitter source file
                             -------------------
    begin                : 2007-05-11
    copyright            : (C) 2007 by The RoboCup Soccer Server
                           Maintenance Group.
    email                : sserver-admin@lists.sourceforge.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU LGPL as published by the Free Software  *
 *   Foundation; either version 2 of the License, or (at your option) any  *
 *   later version.                                                        *
 *                                                                         *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "rcgparser.hpp"
#include "rcgdatahandler.hpp"

#ifdef HAVE_LIBZ
#include <rcssbase/gzip/gzfstream.hpp>
#endif

#include <boost/program_options.hpp>

#include <string>
#include <list>
#include <fstream>
#include <cmath>

#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif


class RCGSplitter
    : public rcss::RCGDataHandler {
private:
    // options
    std::string M_filepath;
    bool M_verbose;
    int M_span_cycle;
    int M_segment_start_cycle;
    int M_segment_end_cycle;

    // game log data
    int M_version;

    server_params_t M_server_param;
    player_params_t M_player_param;
    std::list< player_type_t > M_player_types;

    std::string M_server_param_string;
    std::string M_player_param_string;
    std::list< std::string > M_player_type_strings;

    char M_playmode;
    team_t M_team_l;
    team_t M_team_r;

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
      {}

    bool parseCmdLine( int argc,
                       char ** argv );

    const
    std::string & filepath() const
      {
          return M_filepath;
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
    void doHandleDispInfo( std::streampos,
                           const dispinfo_t & );

    virtual
    void doHandleShowInfo( std::streampos,
                           const showinfo_t & );

    virtual
    void doHandleShowInfo( std::streampos,
                           const short_showinfo_t2 & );

    virtual
    void doHandleMsgInfo( std::streampos,
                          Int16,
                          const std::string & );

    virtual
    void doHandlePlayMode( std::streampos,
                           char );

    virtual
    void doHandleTeamInfo( std::streampos,
                           const team_t &,
                           const team_t & );

    virtual
    void doHandleServerParams( std::streampos,
                               const server_params_t & );

    virtual
    void doHandlePlayerParams( std::streampos,
                               const player_params_t & );

    virtual
    void doHandlePlayerType( std::streampos,
                             const player_type_t & );

    virtual
    void doHandleEOF();


    // version 4
    virtual
    void doHandleShowBegin( const int time );
    virtual
    void doHandleShowEnd();

    virtual
    void doHandleBall( const int time,
                       const BallT & ball );
    virtual
    void doHandlePlayer( const int time,
                         const PlayerT & player );
    virtual
    void doHandleMsg( const int time,
                      const int board,
                      const char * msg );
    virtual
    void doHandlePlayMode( const int time,
                           const PlayMode pm );
    virtual
    void doHandleTeam( const int time,
                       const TeamT & team_l,
                       const TeamT & team_r );
    virtual
    void doHandleServerParams( const std::map< std::string, std::string > & param_map );
    virtual
    void doHandlePlayerParams( const std::map< std::string, std::string > & param_map );
    virtual
    void doHandlePlayerType( const std::map< std::string, std::string > & param_map );


    // utility

    bool createOutputFile( const int cycle );
    void writeHeader();
};


/*--------------------------------------------------------------------*/
bool
RCGSplitter::parseCmdLine( int argc,
                           char ** argv )
{
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
RCGSplitter::doHandleDispInfo( std::streampos,
                               const dispinfo_t & disp )
{
    Int16 mode = ntohs( disp.mode );

    if ( mode == SHOW_MODE )
    {
        int cycle = ntohs( disp.body.show.time );

        createOutputFile( cycle );
    }

    if ( ! M_fout.is_open() )
    {
        return;
    }

    M_fout.write( reinterpret_cast< const char * >( &disp ),
                  sizeof( dispinfo_t ) );

}

/*--------------------------------------------------------------------*/
void
RCGSplitter::doHandleShowInfo( std::streampos,
                               const showinfo_t & show )
{
    int cycle = ntohs( show.time );

    createOutputFile( cycle );

    if ( ! M_fout.is_open() )
    {
        return;
    }

    Int16 mode = htons( SHOW_MODE );

    M_fout.write( reinterpret_cast< const char * >( &mode ),
                  sizeof( mode ) );
    M_fout.write( reinterpret_cast< const char * >( &show ),
                  sizeof( show ) );
}

/*--------------------------------------------------------------------*/
void
RCGSplitter::doHandleShowInfo( std::streampos,
                               const short_showinfo_t2 & show )
{
    int cycle = ntohs( show.time );

    bool new_file = createOutputFile( cycle );

    if ( ! M_fout.is_open() )
    {
        return;
    }

    static char s_playmode = static_cast< char >( 0 );
    static team_t s_teams[2] = { { "", 0 },
                                 { "", 0 } };

    Int16 mode;

    if ( new_file
         || s_playmode != M_playmode )
    {
        s_playmode = M_playmode;

        mode = htons( PM_MODE );
        M_fout.write( reinterpret_cast< const char * >( &mode ),
                      sizeof( mode ) );
        M_fout.write( reinterpret_cast< const char * >( &s_playmode ),
                      sizeof( char ) );
    }

    if  ( new_file
          || std::strlen( s_teams[0].name ) != std::strlen( M_team_l.name )
          || std::strcmp( s_teams[0].name, M_team_l.name )
          || s_teams[0].score != M_team_l.score
          || std::strlen( s_teams[1].name ) != std::strlen( M_team_r.name )
          || std::strcmp( s_teams[1].name, M_team_r.name )
          || s_teams[1].score != M_team_r.score
          )
    {
        std::strncpy( s_teams[0].name, M_team_l.name, 16 );
        s_teams[0].score = M_team_l.score;
        std::strncpy( s_teams[1].name, M_team_r.name, 16 );
        s_teams[1].score = M_team_r.score;

        mode = htons( TEAM_MODE );
        M_fout.write( reinterpret_cast< const char* >( &mode ),
                      sizeof( mode ) );
        M_fout.write( reinterpret_cast< const char* >( s_teams ),
                      sizeof( team_t ) * 2 );
    }


    mode = htons( SHOW_MODE );
    M_fout.write( reinterpret_cast< const char * >( &mode ),
                  sizeof( mode ) );
    M_fout.write( reinterpret_cast< const char * >( &show ),
                  sizeof( show ) );

}

/*--------------------------------------------------------------------*/
void
RCGSplitter::doHandleMsgInfo( std::streampos,
                              Int16 board,
                              const std::string & msg )
{
    if ( ! M_fout.is_open() )
    {
        return;
    }

    Int16 mode = htons( MSG_MODE );

    M_fout.write( reinterpret_cast< const char * >( &mode ),
                  sizeof( mode ) );
    M_fout.write( reinterpret_cast< const char * >( &board ),
                  sizeof( board ) );
    short nlen = htons( static_cast< short >( msg.length() + 1 ) );
    M_fout.write( reinterpret_cast< const char * >( &nlen ),
                  sizeof( nlen ) );

    M_fout.write( msg.c_str(), msg.length() + 1 );
}

/*--------------------------------------------------------------------*/
void
RCGSplitter::doHandlePlayMode( std::streampos,
                               char pm )
{
    M_playmode = pm;
}

/*--------------------------------------------------------------------*/
void
RCGSplitter::doHandleTeamInfo( std::streampos,
                               const team_t & team_l,
                               const team_t & team_r )
{
    std::memcpy( M_team_l.name, team_l.name, 16 );
    M_team_l.score = team_l.score;

    std::memcpy( M_team_r.name, team_r.name, 16 );
    M_team_r.score = team_r.score;
}

/*--------------------------------------------------------------------*/
void
RCGSplitter::doHandleServerParams( std::streampos,
                                   const server_params_t & param )
{
    M_server_param = param;
}

/*--------------------------------------------------------------------*/
void
RCGSplitter::doHandlePlayerParams( std::streampos,
                                   const player_params_t & param )
{
    M_player_param = param;
}

/*--------------------------------------------------------------------*/
void
RCGSplitter::doHandlePlayerType( std::streampos,
                                 const player_type_t & type )
{
    M_player_types.push_back( type );
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
RCGSplitter::doHandleShowBegin( const int time )
{
    static const char * playmode_strings[] = PLAYMODE_STRINGS;

    bool new_file = createOutputFile( time );

    if ( ! M_fout.is_open() )
    {
        return;
    }

    static char s_playmode = static_cast< char >( 0 );
    static team_t s_teams[2] = { { "", 0 },
                                 { "", 0 } };

    if ( new_file
         || s_playmode != M_playmode )
    {
        s_playmode = M_playmode;

        M_fout << "(playmode " << time << ' '
               << playmode_strings[M_playmode] << ")\n";
    }

    if  ( new_file
          || std::strlen( s_teams[0].name ) != std::strlen( M_team_l.name )
          || std::strcmp( s_teams[0].name, M_team_l.name )
          || s_teams[0].score != M_team_l.score
          || std::strlen( s_teams[1].name ) != std::strlen( M_team_r.name )
          || std::strcmp( s_teams[1].name, M_team_r.name )
          || s_teams[1].score != M_team_r.score
          )
    {
        std::snprintf( s_teams[0].name, 16, "%s", M_team_l.name );
        s_teams[0].score = M_team_l.score;
        std::snprintf( s_teams[1].name, 16, "%s", M_team_r.name );
        s_teams[1].score = M_team_r.score;


        M_fout << "(team "
               << M_team_l.name << ' ' << M_team_r.name << ' '
               << ntohs( M_team_l.score ) << ' ' << ntohs( M_team_r.score )
               << ")\n";
    }


    M_fout << "(show " << time;
}

/*--------------------------------------------------------------------*/
void
RCGSplitter::doHandleShowEnd()
{
    if ( ! M_fout.is_open() )
    {
        return;
    }

    M_fout << ")\n";
}

/*--------------------------------------------------------------------*/
void
RCGSplitter::doHandleBall( const int time,
                           const BallT & ball )
{
    if ( ! M_fout.is_open() )
    {
        return;
    }

    M_fout << " ((b) "
           << ball.x << ' ' << ball.y << ' '
           << ball.vx << ' ' << ball.vy << ')';
}

/*--------------------------------------------------------------------*/
void
RCGSplitter::doHandlePlayer( const int time,
                             const PlayerT & player )
{
    if ( ! M_fout.is_open() )
    {
        return;
    }

    M_fout << " ((" << player.side << ' ' << player.unum << ')'
           << player.type << ' '
           << std::hex << std::showbase
           << player.state
           << std::dec << std::noshowbase;
    M_fout << ' ' << player.x
           << ' ' << player.y
           << ' ' << player.vx
           << ' ' << player.vy
           << ' ' << player.body
           << ' ' << player.neck;
    M_fout << " (v "
           << ( player.view_quality ? "h " : "l " )
           << player.view_width << ')';
    M_fout << " (s "
           << player.stamina << ' '
           << player.effort << ' '
           << player.recovery << ')';
    M_fout << " (c "
           << player.n_kick << ' '
           << player.n_dash << ' '
           << player.n_turn << ' '
           << player.n_catch << ' '
           << player.n_move << ' '
           << player.n_turn_neck << ' '
           << player.n_change_view << ' '
           << player.n_say << ' '
           << 0 << ' ' // tackle
           << 0 << ' ' // pointto
           << 0 << ')'; // attentionto
    M_fout << ')';
}

/*--------------------------------------------------------------------*/
void
RCGSplitter::doHandleMsg( const int time,
                          const int board,
                          const char * msg )
{
    if ( ! M_fout.is_open() )
    {
        return;
    }

    M_fout << "(msg " << time
           << ' ' << board
           << " \"" << msg << "\")\n" ;
}

/*--------------------------------------------------------------------*/
void
RCGSplitter::doHandlePlayMode( const int time,
                               const PlayMode pm )
{
    M_playmode = static_cast< char >( pm );
}

/*--------------------------------------------------------------------*/
void
RCGSplitter::doHandleTeam( const int time,
                           const TeamT & team_l,
                           const TeamT & team_r )
{
    std::snprintf( M_team_l.name, 16, "%s", team_l.name.c_str() );
    M_team_l.score = htons( team_l.score );

    std::snprintf( M_team_r.name, 16, "%s", team_r.name.c_str() );
    M_team_r.score = htons( team_r.score );
}

/*--------------------------------------------------------------------*/
void
RCGSplitter::doHandleServerParams( const std::map< std::string, std::string > & param_map )
{
    M_server_param_string = "(server_param ";
    for ( std::map< std::string, std::string >::const_iterator it = param_map.begin();
          it != param_map.end();
          ++it )
    {
        M_server_param_string += '(';
        M_server_param_string += it->first;
        M_server_param_string += ' ';
        M_server_param_string += it->second;
        M_server_param_string += ')';
    }
    M_server_param_string += ')';
}

/*--------------------------------------------------------------------*/
void
RCGSplitter::doHandlePlayerParams( const std::map< std::string, std::string > & param_map )
{
    M_player_param_string = "(player_param ";
    for ( std::map< std::string, std::string >::const_iterator it = param_map.begin();
          it != param_map.end();
          ++it )
    {
        M_player_param_string += '(';
        M_player_param_string += it->first;
        M_player_param_string += ' ';
        M_player_param_string += it->second;
        M_player_param_string += ')';
    }
    M_player_param_string += ')';
}

/*--------------------------------------------------------------------*/
void
RCGSplitter::doHandlePlayerType( const std::map< std::string, std::string > & param_map )
{
    std::string param_string = "(player_type ";
    for ( std::map< std::string, std::string >::const_iterator it = param_map.begin();
          it != param_map.end();
          ++it )
    {
        param_string += '(';
        param_string += it->first;
        param_string += ' ';
        param_string += it->second;
        param_string += ')';
    }
    param_string += ')';

    M_player_type_strings.push_back( param_string );
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
        std::snprintf( filename, 255, "%d-%d.rcg",
                       M_start_cycle, M_start_cycle + M_span_cycle - 1 );
        M_fout.open( filename,
                         std::ios_base::out | std::ios_base::binary );
        if ( M_verbose )
        {
            std::cout << "new file [" << filename << "]" << std::endl;
        }

        writeHeader();

        return true;
    }

    return false;
}

/*--------------------------------------------------------------------*/
void
RCGSplitter::writeHeader()
{
    if ( ! M_fout.is_open() )
    {
        return;
    }

    if ( M_version != REC_OLD_VERSION )
    {
        char header[5];
        header[0] = 'U';
        header[1] = 'L';
        header[2] = 'G';
        header[3] = static_cast< char >( M_version );

        if ( M_version >= REC_VERSION_4 )
        {
            header[3] = static_cast< char >( static_cast< int >( '0' ) + M_version );
        }

        M_fout.write( header, 4 );

        if ( M_version == REC_VERSION_4 )
        {
            M_fout << '\n';
            M_fout << M_server_param_string << '\n';
            M_fout << M_player_param_string << '\n';
            for ( std::list< std::string >::iterator it = M_player_type_strings.begin();
                  it != M_player_type_strings.end();
                  ++it )
            {
                M_fout << *it << '\n';
            }
        }
        else if ( M_version == REC_VERSION_3 )
        {
            Int16 mode;

            mode = htons( PARAM_MODE );
            M_fout.write( reinterpret_cast< const char * >( &mode ),
                          sizeof( mode ) );
            M_fout.write( reinterpret_cast< const char * >( &M_server_param ),
                          sizeof( server_params_t ) );

            mode = htons( PPARAM_MODE );
            M_fout.write( reinterpret_cast< const char * >( &mode ),
                          sizeof( mode ) );
            M_fout.write( reinterpret_cast< const char * >( &M_player_param ),
                          sizeof( player_params_t ) );

            mode = htons( PT_MODE );
            for ( std::list< player_type_t >::iterator it = M_player_types.begin();
                  it != M_player_types.end();
                  ++it )
            {
                M_fout.write( reinterpret_cast< const char * >( &mode ),
                              sizeof( mode ) );
                M_fout.write( reinterpret_cast< const char * >( &(*it) ),
                              sizeof( player_type_t ) );
            }
        }
    }

}

/*--------------------------------------------------------------------*/

int
main( int argc, char ** argv )
{
    RCGSplitter splitter;

    if ( ! splitter.parseCmdLine( argc, argv ) )
    {
        return 1;
    }

#ifdef HAVE_LIBZ
    rcss::gz::gzifstream fin( splitter.filepath().c_str() );
#else
    std::ifstream fin( splitter.filepath().c_str() );
#endif

    rcss::RCGParser parser( splitter );
    try
    {
        while ( parser.parse( fin ) )
        {
            // do nothing
        }
    }
    catch ( const std::string & e )
    {
        std::cerr << e << std::endl;
    }
    catch ( ... )
    {
        std::cerr << "Unknown error occured\n";
    }
}
