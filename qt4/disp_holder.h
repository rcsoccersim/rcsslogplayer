// -*-c++-*-

/*!
  \file view_holder.h
  \brief view data holder class Header File.
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
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this code; see the file COPYING.	If not, write to
 the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.

 *EndCopyright:
 */

/////////////////////////////////////////////////////////////////////

#ifndef RCSSLOGPLAYER_DISP_HOLDER_H
#define RCSSLOGPLAYER_DISP_HOLDER_H

#include "team_graphic.h"

#include <rcsslogplayer/types.h>
#include <rcsslogplayer/handler.h>

#include <boost/shared_ptr.hpp>

#include <map>
#include <vector>
#include <iostream>

typedef boost::shared_ptr< rcss::rcg::DispInfoT > DispPtr;
typedef boost::shared_ptr< const rcss::rcg::DispInfoT > DispConstPtr;

class DispHolder
    : public rcss::rcg::Handler {
private:

    int M_log_version;
    rcss::rcg::PlayMode M_playmode; //!< last handled playmode
    rcss::rcg::TeamT M_teams[2]; //!< last handled team info
    DispPtr M_last_disp;

    std::vector< DispPtr > M_dispinfo_cont;

    rcss::rcg::ServerParamT M_server_param;
    rcss::rcg::PlayerParamT M_player_param;
    rcss::rcg::PlayerTypeT M_default_player_type;
    std::map< int, rcss::rcg::PlayerTypeT > M_player_types;

    //! the index of score change
    std::vector< std::size_t > M_score_changed_index;

    // the record of penalty score/miss, first: time, second: playmode
    std::vector< std::pair< int, rcss::rcg::PlayMode > > M_penalty_scores_left;
    std::vector< std::pair< int, rcss::rcg::PlayMode > > M_penalty_scores_right;

    std::multimap< int, rcss::rcg::PointInfoT > M_point_map;
    std::multimap< int, rcss::rcg::LineInfoT > M_line_map;
    std::multimap< int, rcss::rcg::CircleInfoT > M_circle_map;

    // team graphic holder
    TeamGraphic M_team_graphic_left;
    TeamGraphic M_team_graphic_right;

    // not used
    DispHolder( const DispHolder & );
    const DispHolder & operator=( const DispHolder & );
public:

    DispHolder();
    ~DispHolder();

    void clear();

    int logVersion() const
      {
          return M_log_version;
      }

    DispConstPtr getDispInfo( const std::size_t idx ) const;
    std::size_t getIndexOf( const int time ) const;

    DispConstPtr lastDispInfo() const
      {
          return M_last_disp;
      }

    const
    std::vector< DispPtr > & dispInfoCont() const
      {
          return M_dispinfo_cont;
      }

    const
    rcss::rcg::ServerParamT & serverParam() const
      {
          return M_server_param;
      }

    const
    rcss::rcg::PlayerParamT & playerParam() const
      {
          return M_player_param;
      }

    const
    std::map< int, rcss::rcg::PlayerTypeT > & playerTypes() const
      {
          return M_player_types;
      }

    const
    rcss::rcg::PlayerTypeT & playerType( const int id ) const;

    const
    std::vector< std::size_t > & scoreChangedIndex() const
      {
          return M_score_changed_index;
      }

    const
    std::vector< std::pair< int, rcss::rcg::PlayMode > > & penaltyScoresLeft() const
      {
          return M_penalty_scores_left;
      }

    const
    std::vector< std::pair< int, rcss::rcg::PlayMode > > & penaltyScoresRight() const
      {
          return M_penalty_scores_right;
      }


    const
    std::multimap< int, rcss::rcg::PointInfoT > & pointMap() const
      {
          return M_point_map;
      }

    const
    std::multimap< int, rcss::rcg::CircleInfoT > & circleMap() const
      {
          return M_circle_map;
      }

    const
    std::multimap< int, rcss::rcg::LineInfoT > & lineMap() const
      {
          return M_line_map;
      }


    const
    TeamGraphic & teamGraphicLeft() const
      {
          return M_team_graphic_left;
      }

    const
    TeamGraphic & teamGraphicRight() const
      {
          return M_team_graphic_right;
      }


    bool addDispInfo1( const rcss::rcg::dispinfo_t & disp );
    bool addDispInfo2( const rcss::rcg::dispinfo_t2 & disp );
    bool addDispInfo3( const char * msg );

private:
    virtual
    void doHandleLogVersion( int ver );
    virtual
    int doGetLogVersion() const;
    virtual
    void doHandleShowInfo( const rcss::rcg::ShowInfoT & );
    virtual
    void doHandleMsgInfo( const int,
                          const int,
                          const std::string & );
    virtual
    void doHandlePlayMode( const int,
                           const rcss::rcg::PlayMode );
    virtual
    void doHandleTeamInfo( const int,
                           const rcss::rcg::TeamT &,
                           const rcss::rcg::TeamT & );
    virtual
    void doHandleDrawClear( const int );
    virtual
    void doHandleDrawPointInfo( const int,
                                const rcss::rcg::PointInfoT & );
    virtual
    void doHandleDrawLineInfo( const int,
                               const rcss::rcg::LineInfoT & );
    virtual
    void doHandleDrawCircleInfo( const int,
                                 const rcss::rcg::CircleInfoT & );
    virtual
    void doHandleServerParam( const rcss::rcg::ServerParamT & );
    virtual
    void doHandlePlayerParam( const rcss::rcg::PlayerParamT & );
    virtual
    void doHandlePlayerType( const rcss::rcg::PlayerTypeT & );
    virtual
    void doHandleEOF();

private:
    void analyzeTeamGraphic( const std::string & msg );

};

#endif
