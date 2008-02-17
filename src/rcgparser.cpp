// -*-c++-*-

/***************************************************************************
                                 rcgparser.cpp
                         Class for parsing rcg files
                             -------------------
    begin                : 18-FEB-2003
    copyright            : (C) 2003 by The RoboCup Soccer Server
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

#include <iostream>
#include <string>

#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif
#ifdef HAVE_WINDOWS_H
#include <windows.h>
#endif

namespace rcss {

RCGParser::RCGParser( RCGDataHandler & handler )
    : M_handler( handler )
{

}


bool
RCGParser::doParse( std::istream & strm )
{
    if ( strm.tellg() == std::streampos( 0 )
         && ! parseBegin( strm ) )
    {
        return false;
    }

    if ( M_handler.getLogVersion() >= REC_VERSION_4 )
    {
        return parseLines( strm );
    }

    return parseNext( strm );
}

bool
RCGParser::parseBegin( std::istream & strm )
{
    char buf[4];
    strm.read( buf, 4 );
    if ( strm.good() )
    {
        if( buf[ 0 ] == 'U'
            && buf[ 1 ] == 'L'
            && buf[ 2 ] == 'G' )
        {
            int ver = static_cast< int >( buf[3] );
            if ( ver != 2 && ver != 3 )
            {
                ver -= static_cast< int >( '0' );
            }
            std::cerr << "RCGParser::parseBegin version = " << ver
                      << std::endl;

            M_handler.handleLogVersion( ver );
        }
        else
        {
            std::cerr << "RCGParser::parseBegin version = " << 1
                      << std::endl;

            strm.seekg( 0 );
            M_handler.handleLogVersion( 1 );
        }
        return true;
    }
    else
    {
        return strmErr( strm );
    }
}

bool
RCGParser::parseNext( std::istream & strm )
{
    if ( M_handler.getLogVersion() == REC_OLD_VERSION )
    {
        return parseDispInfo( strm );
    }
    else
    {
        return parseMode( strm );
    }
}

bool
RCGParser::parseDispInfo( std::istream & strm )
{
    std::streampos pos = strm.tellg();
    dispinfo_t info;
    strm.read( reinterpret_cast< char * >( &info ), sizeof( dispinfo_t ) );
    if ( strm.good() )
    {
        M_handler.handleDispInfo( pos, info );
        return true;
    }
    else
    {
        return strmErr( strm );
    }
}

bool
RCGParser::parseMode( std::istream & strm )
{
    std::streampos pos = strm.tellg();
    short mode;
    strm.read( reinterpret_cast< char * >( &mode ), sizeof( mode ) );
    if ( strm.good() )
    {
        return parseItem( strm, ntohs( mode ), pos );
    }
    else
    {
        return strmErr( strm );
    }
}

bool
RCGParser::parseItem( std::istream & strm,
                      short mode,
                      std::streampos pos )
{
    switch( mode ) {
    case NO_INFO:
        return true;
    case SHOW_MODE:
        return parseShowInfo( strm, pos );
    case MSG_MODE:
        return parseMsgInfo( strm, pos );
    case PM_MODE:
        return parsePlayMode( strm, pos );
    case TEAM_MODE:
        return parseTeamInfo( strm, pos );
    case PARAM_MODE:
        return parseServerParams( strm, pos );
    case PPARAM_MODE:
        return parsePlayerParams( strm, pos );
    case PT_MODE:
        return parsePlayerType( strm, pos );
    default:
        throw std::string( "Unknown mode\n" );
    }
}

bool
RCGParser::parseShowInfo( std::istream & strm,
                          std::streampos pos )
{
    if ( M_handler.getLogVersion() == REC_VERSION_3 )
    {
        short_showinfo_t2 info;
        strm.read( reinterpret_cast< char * >( &info ), sizeof( short_showinfo_t2 ) );
        if ( strm.good() )
        {
            M_handler.handleShowInfo( pos, info );
            return true;
        }
        else
        {
            return strmErr( strm );
        }
    }
    else
    {
        showinfo_t info;
        strm.read( reinterpret_cast< char * >( &info ), sizeof( showinfo_t ) );
        if ( strm.good() )
        {
            M_handler.handleShowInfo( pos, info );
            return true;
        }
        else
        {
            return strmErr( strm );
        }
    }
}

bool
RCGParser::parseMsgInfo( std::istream & strm,
                         std::streampos pos )
{
    short board;
    strm.read( reinterpret_cast< char * >( &board ), sizeof( short ) );
    if( strm.good() )
    {
        short len;
        strm.read( reinterpret_cast< char * >(  &len ), sizeof( short ) );
        if( strm.good() )
        {
            len = ntohs( len );
            char* msg = new char[ len ];
            strm.read( msg, len );
            if( strm.good() )
            {
                if( msg[ len - 1 ] == 0 )
                    len = strlen( msg );
                // copy the message to a string and delete the
                // original.  That was if handleMsgInfo throws,
                // the memory won't be lost.
                std::string msgstr( msg );
                delete[] msg;
                M_handler.handleMsgInfo( pos, board, msgstr );
                return true;
            }
            else
                delete[] msg;
        }
    }
    return strmErr( strm );
}

bool
RCGParser::parsePlayMode( std::istream & strm,
                          std::streampos pos )
{
    char playmode;
    strm.read( &playmode, sizeof( char ) );
    if ( strm.good() )
    {
        M_handler.handlePlayMode( pos, playmode );
        return true;
    }
    else
    {
        return strmErr( strm );
    }
}

bool
RCGParser::parseTeamInfo( std::istream & strm,
                          std::streampos pos )
{
    team_t team_info[ 2 ];
    strm.read( reinterpret_cast< char * >( team_info ), sizeof( team_t ) * 2 );
    if ( strm.good() )
    {
        M_handler.handleTeamInfo( pos, team_info[ 0 ], team_info[ 1 ] );
        return true;
    }
    else
    {
        return strmErr( strm );
    }
}

bool
RCGParser::parseServerParams( std::istream & strm,
                              std::streampos pos )
{
    server_params_t params;
    strm.read( reinterpret_cast< char * >( &params ), sizeof( params ) );
    if ( strm.good() )
    {
        M_handler.handleServerParams( pos, params );
        return true;
    }
    else
    {
        return strmErr( strm );
    }
}

bool
RCGParser::parsePlayerParams( std::istream & strm,
                              std::streampos pos )
{
    player_params_t params;
    strm.read( reinterpret_cast< char * >( &params ), sizeof( params ) );
    if( strm.good() )
    {
        M_handler.handlePlayerParams( pos, params );
        return true;
    }
    else
    {
        return strmErr( strm );
    }
}

bool
RCGParser::parsePlayerType( std::istream & strm,
                            std::streampos pos )
{
    player_type_t type;
    strm.read( reinterpret_cast< char * >( &type ), sizeof( type ) );
    if ( strm.good() )
    {
        M_handler.handlePlayerType( pos, type );
        return true;
    }
    else
    {
        return strmErr( strm );
    }
}

bool
RCGParser::strmErr( std::istream & strm )
{
    if ( strm.eof() )
    {
        M_handler.handleEOF();
    }
    return false;
}


bool
RCGParser::parseLines( std::istream & strm )
{
    int n_line = 0;

    std::string line;
    line.reserve( 8192 );

    while ( std::getline( strm, line ) )
    {
        ++n_line;
        if ( line.empty() ) continue;

        if ( line.compare( 0, 6, "(show " ) == 0 )
        {
            parseShowLine( n_line, line );
        }
        else if ( line.compare( 0, 5, "(msg " ) == 0 )
        {
            parseMsgLine( n_line, line );
        }
        else if ( line.compare( 0, 10, "(playmode " ) == 0 )
        {
            parsePlayModeLine( n_line, line );
        }
        else if ( line.compare( 0, 6, "(team " ) == 0 )
        {
            parseTeamLine( n_line, line );
        }
        else if ( line.compare( 0, 13, "(player_type " ) == 0 )
        {
            parsePlayerTypeLine( n_line, line );
        }
        else if ( line.compare( 0, 14, "(server_param " ) == 0 )
        {
            parseServerParamLine( n_line, line );
        }
        else if ( line.compare( 0, 14, "(player_param " ) == 0 )
        {
            parsePlayerParamLine( n_line, line );
        }
        else
        {
            std::cerr << __FILE__ << ':' << __LINE__ << ": error: "
                      << "RCGParser: Unknown info at line " << n_line
                      << "\"" << line << "\""
                      << std::endl;
        }

        if ( n_line >= 50 ) break;
    }

    M_handler.handleEOF();

    return false;

}

bool
RCGParser::parseShowLine( const int n_line,
                          const std::string & line )
{
    const char * buf = line.c_str();

    int n_read = 0;

    // time
    int time = 0;

    if ( std::sscanf( buf, " ( show %d %n ",
                      &time, &n_read ) != 1 )
    {
        std::cerr << n_line << ": error: "
                  << "Illegal time info \"" << line << "\"" << std::endl;
        return false;
    }
    buf += n_read;

    M_handler.handleShowBegin( time );

    // ball
    {
        BallT ball;
        if ( std::sscanf( buf, " ( ( b ) %lf %lf %lf %lf ) %n ",
                          &ball.x, &ball.y, &ball.vx, &ball.vy,
                          &n_read ) != 4 )
        {
            std::cerr << n_line << ": error: "
                      << "Illegal ball info \"" << line << "\"" << std::endl;
            return false;
        }
        buf += n_read;

        M_handler.handleBall( time, ball );
    }

    // players
    for ( int i = 0; i < MAX_PLAYER * 2; ++i )
    {
        if ( *buf == ')' ) break;

        PlayerT p;

        if ( std::sscanf( buf,
                          " ( ( %c %d ) %d %lx %lf %lf %lf %lf %lf %lf %n ",
                          &p.side, &p.unum, &p.type, &p.state,
                          &p.x, &p.y, &p.vx, &p.vy, &p.body, &p.neck,
                          &n_read ) != 10 )
        {
            std::cerr << n_line << ": error: "
                      << " Illegal player " << i << " \"" << line << "\""
                      << std::endl;;
            return false;
        }
        buf += n_read;

        if ( std::sscanf( buf,
                          " %lf %lf %n ",
                          &p.arm_dist, &p.arm_head,
                          &n_read ) == 2 )
        {
            buf += n_read;
        }

        char view_quality = 'h';
        if ( std::sscanf( buf,
                          " ( v %c %lf ) %n ",
                          &view_quality, &p.view_width,
                          &n_read ) != 2 )
        {
            std::cerr << n_line << ": error: "
                      << "Illegal player " << i << " view \"" << line << "\"" << std::endl;;
            return false;
        }
        buf += n_read;
        p.view_quality = ( view_quality == 'h' ? true : false );

        if ( std::sscanf( buf,
                          " ( s %lf %lf %lf ) %n ",
                          &p.stamina, &p.effort, &p.recovery,
                          &n_read ) != 3 )
        {
            std::cerr << n_line << ": error: "
                      << "Illegal player " << i << " stamina \"" << line << "\"" << std::endl;;
            return false;
        }
        buf += n_read;

        if ( std::sscanf( buf,
                          " ( f %c %d ) %n ",
                          &p.focus_side, &p.focus_unum,
                          &n_read ) == 2 )
        {
            buf += n_read;
        }

        if ( std::sscanf( buf,
                          " ( c %d %d %d %d %d %d %d %d %d %d %d ) ) %n ",
                          &p.n_kick, &p.n_dash, &p.n_turn, &p.n_catch, &p.n_move,
                          &p.n_turn_neck, &p.n_change_view, &p.n_say, &p.n_tackle,
                          &p.n_pointto, &p.n_attentionto,
                          &n_read ) != 11 )
        {
            std::cerr << n_line << ": error: "
                      << "Illegal player " << i << " count \"" << line << "\"" << std::endl;;
            return false;
        }
        buf += n_read;

        M_handler.handlePlayer( time, p );
    }

    M_handler.handleShowEnd();

    return true;
}

bool
RCGParser::parseMsgLine( const int n_line,
                         const std::string & line )
{
    int time = 0;
    int board = 0;
    char msg[8192];

    if ( std::sscanf( line.c_str(),
                      " ( msg %d %d \"%8191[^\"]\" ) ",
                      &time, &board, msg ) != 3 )
    {
        std::cerr << n_line << ": error: "
                  << "Illegal msg line. \"" << line << "\"" << std::endl;;
        return false;
    }

    M_handler.handleMsg( time, board, msg );

    return true;
}

bool
RCGParser::parsePlayModeLine( const int n_line,
                              const std::string & line )
{
    static const char * playmode_strings[] = PLAYMODE_STRINGS;

    int time = 0;
    char pm_string[32];

    if ( std::sscanf( line.c_str(),
                      " ( playmode %d %31[^)] ) ",
                      &time, pm_string ) != 2 )
    {
        std::cerr << n_line << ": error: "
                  << "Illegal playmode line. \"" << line << "\"" << std::endl;;
        return false;
    }

    PlayMode pm = PM_Null;
    for ( int n = 0; n < PM_MAX; ++n )
    {
        if ( ! std::strcmp( playmode_strings[n], pm_string ) )
        {
            pm = static_cast< PlayMode >( n );
            break;
        }
    }

    M_handler.handlePlayMode( time, pm );

    return true;
}

bool
RCGParser::parseTeamLine( const int n_line,
                          const std::string & line )
{
    int time = 0;
    char name_l[32], name_r[32];
    int score_l = 0, score_r = 0;
    int pen_score_l = 0, pen_miss_l = 0, pen_score_r = 0, pen_miss_r = 0;


    int n = std::sscanf( line.c_str(),
                         " ( team %d %31s %31s %d %d %d %d %d %d ",
                         &time,
                         name_l, name_r,
                         &score_l, &score_r,
                         &pen_score_l, &pen_miss_l,
                         &pen_score_r, &pen_miss_r );
    if ( n != 5 && n != 9 )
    {
        std::cerr << n_line << ": error: "
                  << "Illegal team line. \"" << line << "\"" << std::endl;;
        return false;
    }

    TeamT team_l( name_l, score_l, pen_score_l, pen_miss_l );
    TeamT team_r( name_r, score_r, pen_score_r, pen_miss_r );

    M_handler.handleTeam( time, team_l, team_r );

    return true;
}


bool
RCGParser::parseServerParamLine( const int n_line,
                                 const std::string & line )
{
    std::string name;
    ParamMap param_map;

    if ( ! parseParamLine( line, name, param_map )
         || name != "server_param" )
    {
        std::cerr << n_line << ": error: "
                  << "Illegal server_param line. \"" << line << "\"" << std::endl;;
        return false;
    }

    M_handler.handleServerParams( param_map );

    return true;
}

bool
RCGParser::parsePlayerParamLine( const int n_line,
                                 const std::string & line )
{
    std::string name;
    ParamMap param_map;

    if ( ! parseParamLine( line, name, param_map )
         || name != "player_param" )
    {
        std::cerr << n_line << ": error: "
                  << "Illegal player_param line. \"" << line << "\"" << std::endl;;
        return false;
    }

    M_handler.handlePlayerParams( param_map );

    return true;
}

bool
RCGParser::parsePlayerTypeLine( const int n_line,
                                const std::string & line )
{
    std::string name;
    ParamMap param_map;

    if ( ! parseParamLine( line, name, param_map )
         || name != "player_type" )
    {
        std::cerr << n_line << ": error: "
                  << "Illegal player_type line. \"" << line << "\"" << std::endl;;
        return false;
    }

    M_handler.handlePlayerType( param_map );

    return true;
}


bool
RCGParser::parseParamLine( const std::string & line,
                           std::string & param_name,
                           ParamMap & param_map )
{
    int n_read = 0;
    {

        char buf[32];
        if ( std::sscanf( line.c_str(), " ( %31s %n ", buf, &n_read ) != 1 )
        {
            std::cerr << "Failed to the parse message id."
                      << std::endl;
            return false;
        }

        param_name = buf;
    }

    for ( std::string::size_type pos = line.find_first_of( '(', n_read );
          pos != std::string::npos;
          pos = line.find_first_of( '(', pos ) )
    {
        std::string::size_type end_pos = line.find_first_of( ' ', pos );
        if ( end_pos == std::string::npos )
        {
            std::cerr << "Failed to find parameter name."
                      << std::endl;
            return false;
        }

        pos += 1;
        const std::string name_str( line, pos, end_pos - pos );

        pos = end_pos;
        // search end paren or double quatation
        end_pos = line.find_first_of( ")\"", end_pos ); //"
        if ( end_pos == std::string::npos )
        {
            std::cerr << "Failed to parse parameter value for [" << name_str << "] "
                      << std::endl;
            return false;
        }

        // found quated value
        if ( line[end_pos] == '\"' )
        {
            pos = end_pos;
            end_pos = line.find_first_of( '\"', end_pos + 1 ); //"
            if ( end_pos == std::string::npos )
            {
                std::cerr << "Failed to parse the quated value for [" << name_str << "] "
                          << std::endl;
                return false;
            }
            end_pos += 1; // skip double quatation
        }
        else
        {
            pos += 1; // skip white space
        }

        std::string value_str( line, pos, end_pos - pos );
        //value_str = cleanString( value_str );

        param_map.insert( ParamMap::value_type( name_str, value_str ) );

        pos = end_pos;
    }

    return true;
}


}
