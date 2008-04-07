// -*-c++-*-

/*!
  \file handler.h
  \brief Class for handring rcg data
*/

/*
 *Copyright:

 Copyright (C) The RoboCup Soccer Server Maintenance Group.
 Hidehisa AKIYAMA

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

/////////////////////////////////////////////////////////////////////

#ifndef RCSSLOGPLAYER_HANDLER_H
#define RCSSLOGPLAYER_HANDLER_H

#include <rcsslogplayer/types.h>

#include <string>

namespace rcss {
namespace rcg {

/*!
  class Handler
  \brief abstract rcg data handler
 */
class Handler {
protected:
    Handler()
      { }

public:

    virtual
    ~Handler()
      { }

private:
    friend class Parser;

    void handleLogVersion( int ver )
      {
          doHandleLogVersion( ver );
      }

    int getLogVersion() const
      {
          return doGetLogVersion();
      }

    void handleShowInfo( const ShowInfoT & info )
      {
          doHandleShowInfo( info );
      }

    void handleMsgInfo( const int time,
                        const int board,
                        const std::string & msg )
      {
          doHandleMsgInfo( time, board, msg );
      }

    void handlePlayMode( const int time,
                         const PlayMode playmode )
      {
          doHandlePlayMode( time, playmode );
      }

    void handleTeamInfo( const int time,
                         const TeamT & team_l,
                         const TeamT & team_r )
      {
          doHandleTeamInfo( time, team_l, team_r );
      }

    void handleDrawClear( const int time )
      {
          doHandleDrawClear( time );
      }

    void handleDrawPointInfo( const int time,
                              const PointInfoT & p )
      {
          doHandleDrawPointInfo( time, p );
      }

    void handleDrawCircleInfo( const int time,
                               const CircleInfoT & c )
      {
          doHandleDrawCircleInfo( time, c );
      }

    void handleDrawLineInfo( const int time,
                             const LineInfoT & l )
      {
          doHandleDrawLineInfo( time, l );
      }

    void handleServerParam( const ServerParamT & param )
      {
          doHandleServerParam( param );
      }

    void handlePlayerParam( const PlayerParamT & param )
      {
          doHandlePlayerParam( param );
      }

    void handlePlayerType( const PlayerTypeT & type )
      {
          doHandlePlayerType( type );
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
    void doHandleShowInfo( const ShowInfoT & )
      { }

    virtual
    void doHandleMsgInfo( const int,
                          const int,
                          const std::string & )
      { }

    virtual
    void doHandlePlayMode( const int,
                           const PlayMode )
      { }

    virtual
    void doHandleTeamInfo( const int,
                           const TeamT &,
                           const TeamT & )
      { }

    virtual
    void doHandleDrawClear( const int )
      { }

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
    void doHandleServerParam( const ServerParamT & )
      { }

    virtual
    void doHandlePlayerParam( const PlayerParamT & )
      { }

    virtual
    void doHandlePlayerType( const PlayerTypeT & )
      { }

    virtual
    void doHandleEOF()
      { }

};

}
}

#endif
