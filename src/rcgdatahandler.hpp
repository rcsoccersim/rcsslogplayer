// -*-c++-*-

/***************************************************************************
                                rcgdatahandler.hpp
                      Class for handling rcg data
                             -------------------
    begin                : 20-FEB-2003
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

#ifndef RCGDATAHANDLER_HPP
#define RCGDATAHANDLER_HPP

#include "types.h"


namespace rcss {

class RCGDataHandler {
public:
    RCGDataHandler() {}

    virtual
    ~RCGDataHandler() {}

private:
    friend class RCGParser;

    void handleLogVersion( int ver )
      {
          doHandleLogVersion( ver );
      }

    int getLogVersion() const
      {
          return doGetLogVersion();
      }

    void handleDispInfo( std::streampos pos,
                         const dispinfo_t & info )
      {
          doHandleDispInfo( pos, info );
      }

    void handleShowInfo( std::streampos pos,
                         const showinfo_t & info )
      {
          doHandleShowInfo( pos, info );
      }

    void handleShowInfo( std::streampos pos,
                         const short_showinfo_t2 & info )
      {
          doHandleShowInfo( pos, info );
      }

    void handleMsgInfo( std::streampos pos,
                        short board,
                        const std::string & msg )
      {
          doHandleMsgInfo( pos, board, msg );
      }

    void handlePlayMode( std::streampos pos,
                         char playmode )
      {
          doHandlePlayMode( pos, playmode );
      }

    void handleTeamInfo( std::streampos pos,
                         const team_t & team_info_l,
                         const team_t & team_info_r )
      {
          doHandleTeamInfo( pos, team_info_l, team_info_r );
      }

    void handleServerParams( std::streampos pos,
                             const server_params_t & params )
      {
          doHandleServerParams( pos, params );
      }

    void handlePlayerParams( std::streampos pos,
                             const player_params_t & params )
      {
          doHandlePlayerParams( pos, params );
      }

    void handlePlayerType( std::streampos pos,
                           const player_type_t & type )
      {
          doHandlePlayerType( pos, type );
      }

    void handleEOF()
      {
          doHandleEOF();
      }

protected:
    virtual
    void doHandleLogVersion( int ver ) = 0;

    virtual
    int doGetLogVersion() const = 0;

    virtual
    void doHandleDispInfo( std::streampos,
                           const dispinfo_t & )
      { }

    virtual
    void doHandleShowInfo( std::streampos,
                           const showinfo_t & )
      { }

    virtual
    void doHandleShowInfo( std::streampos,
                           const short_showinfo_t2 & )
      { }

    virtual
    void doHandleMsgInfo( std::streampos,
                          short,
                          const std::string & )
      { }

    virtual
    void doHandlePlayMode( std::streampos,
                           char )
      { }

    virtual
    void doHandleTeamInfo( std::streampos,
                           const team_t &,
                           const team_t & )
      { }

    virtual
    void doHandleServerParams( std::streampos,
                               const server_params_t & )
      { }

    virtual
    void doHandlePlayerParams( std::streampos,
                               const player_params_t & )
      { }

    virtual
    void doHandlePlayerType( std::streampos,
                             const player_type_t & )
      { }

    virtual
    void doHandleEOF()
      { }
};

}

#endif
