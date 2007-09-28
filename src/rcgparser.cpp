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

#include "rcgparser.hpp"
#include "rcgdatahandler.hpp"

#include <netinet/in.h>

namespace rcss
{
    RCGParser::RCGParser( RCGDataHandler& handler )
        : m_handler( handler )
    {}


    bool
    RCGParser::parseBegin( std::istream& strm )
    {
        char buf[4];
        strm.read( buf, 4 );
        if( strm.good() )
        {
            if( buf[ 0 ] == 'U'
                && buf[ 1 ] == 'L'
                && buf[ 2 ] == 'G' )
            {
                m_handler.handleLogVersion( buf[ 3 ] );
            }
            else
            {
                strm.seekg( 0 );
                m_handler.handleLogVersion( 1 );
            }
            return true;
        }
        else
            return strmErr( strm );
    }

    bool
    RCGParser::doParse( std::istream& strm )
    {
        if( strm.tellg() == std::streampos( 0 ) && !parseBegin( strm ) )
            return false;
        
        return parseNext( strm );
    }

    bool
    RCGParser::parseDispInfo( std::istream& strm )
    {
        std::streampos pos = strm.tellg();
        dispinfo_t info;
        strm.read( (char*)&info, sizeof( dispinfo_t ) );
        if( strm.good() )
        {
            m_handler.handleDispInfo( pos, info );
            return true;
        }
        else
            return strmErr( strm );
    }

    bool
    RCGParser::parseMode( std::istream& strm )
    {
        std::streampos pos = strm.tellg();
        short mode;
        strm.read( (char*)&mode, sizeof( mode ) );
        if( strm.good() )
            return parseItem( strm, ntohs( mode ), pos );
        else
            return strmErr( strm );
    }

    bool
    RCGParser::parseNext( std::istream& strm )
    {
        switch( m_handler.getLogVersion() )
        {
            case REC_OLD_VERSION:
                return parseDispInfo( strm );
                
            default:
                return parseMode( strm );
        }
    }

    bool
    RCGParser::parseItem( std::istream& strm, short mode, std::streampos pos )
    {
        switch( mode )
           {
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
    RCGParser::parseShowInfo( std::istream& strm, std::streampos pos )
    {
        if( m_handler.getLogVersion() == REC_VERSION_3 )
        {
			short_showinfo_t2 info;
            strm.read( (char*)&info, sizeof( short_showinfo_t2 ) );
            if( strm.good() )
            { 
                m_handler.handleShowInfo( pos, info );
                return true; 
            }
            else
                return strmErr( strm );   
        }
        else
        {
            showinfo_t info;
            strm.read( (char *)&info, sizeof( showinfo_t ) );
            if( strm.good() )
            { 
                m_handler.handleShowInfo( pos, info );
                return true; 
            }
            else
                return strmErr( strm );
        }
    }

    bool
    RCGParser::parseMsgInfo( std::istream& strm, std::streampos pos )
    {
        short board;
        strm.read( (char*)&board, sizeof( short ) );
        if( strm.good() )
        {
            short len;
            strm.read( (char*)&len, sizeof( short ) );
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
                    m_handler.handleMsgInfo( pos, board, msgstr );
                    return true;
                }
                else
                    delete[] msg;
            }
        }
        return strmErr( strm );
    }	

    bool
    RCGParser::parsePlayMode( std::istream& strm, std::streampos pos )
    {
        char playmode;
        strm.read( &playmode, sizeof( char ) );
        if( strm.good() )
        {
            m_handler.handlePlayMode( pos, playmode );
            return true;
        }
        else
            return strmErr( strm );
    }

    bool
    RCGParser::parseTeamInfo( std::istream& strm, std::streampos pos )
    {
        team_t team_info[ 2 ];
        strm.read( (char*)team_info, sizeof( team_t ) * 2 );
        if( strm.good() )
        {
            m_handler.handleTeamInfo( pos, team_info[ 0 ], team_info[ 1 ] );
            return true;
        }
        else
            return strmErr( strm );
    }		  
		
    bool
    RCGParser::parseServerParams( std::istream& strm, std::streampos pos )
    {
        server_params_t params;
        strm.read( (char*)&params, sizeof( params ) );
        if( strm.good() )
        {
            m_handler.handleServerParams( pos, params );
            return true;
        }
        else
            return strmErr( strm );
    }

    bool
    RCGParser::parsePlayerParams( std::istream& strm, std::streampos pos )
    {
        player_params_t params;
        strm.read( (char*)&params, sizeof( params ) );
        if( strm.good() )
        {
            m_handler.handlePlayerParams( pos, params );
            return true;
        }
        else
            return strmErr( strm );
    }

    bool
    RCGParser::parsePlayerType( std::istream& strm, std::streampos pos )
    {
        player_type_t type;
        strm.read( (char*)&type, sizeof( type ) );
        if( strm.good() )
        {
            m_handler.handlePlayerType( pos, type );
            return true;
        }
        else
        {
            return strmErr( strm );
        }
    }

    bool
    RCGParser::strmErr( std::istream& strm )
    {
        if( strm.eof() )
            m_handler.handleEOF();
        return false;   
    }
}

