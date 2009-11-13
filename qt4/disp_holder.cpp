// -*-c++-*-

/*!
  \file disp_holder.cpp
  \brief display data holder class Source File.
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "disp_holder.h"

#include <rcsslogplayer/util.h>
#include <rcsslogplayer/parser.h>

#include <sstream>
#include <cmath>
#include <cstring>
#include <cstdio>

#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif
#ifdef HAVE_WINDOWS_H
#include <windows.h>
#endif

/*-------------------------------------------------------------------*/
/*!

 */
DispHolder::DispHolder()
    : M_log_version( 0 )
{

}

/*-------------------------------------------------------------------*/
/*!

 */
DispHolder::~DispHolder()
{

}

/*-------------------------------------------------------------------*/
/*!

 */
void
DispHolder::clear()
{
    M_playmode = rcss::rcg::PM_Null;
    M_teams[0].clear();
    M_teams[1].clear();

    M_score_changed_index.clear();
    M_penalty_scores_left.clear();
    M_penalty_scores_right.clear();

    M_last_disp.reset();
    M_dispinfo_cont.clear();

    M_server_param = rcss::rcg::ServerParamT();
    M_player_param = rcss::rcg::PlayerParamT();
    M_player_types.clear();

    M_point_map.clear();
    M_line_map.clear();
    M_circle_map.clear();

    M_team_graphic_left.clear();
    M_team_graphic_right.clear();
}

/*-------------------------------------------------------------------*/
/*!

 */
DispConstPtr
DispHolder::getDispInfo( const std::size_t idx ) const
{
    if ( M_dispinfo_cont.size() <= idx )
    {
        return DispConstPtr(); // null pointer
    }

    return M_dispinfo_cont[idx];
}

namespace {

struct TimeCmp {
    bool operator()( const DispPtr & lhs,
                     const int time ) const
      {
          return lhs->show_.time_ < time;
      }
};

}

/*-------------------------------------------------------------------*/
/*!

 */
std::size_t
DispHolder::getIndexOf( const int time ) const
{
    std::vector< DispPtr >::const_iterator it
        = std::lower_bound( M_dispinfo_cont.begin(),
                            M_dispinfo_cont.end(),
                            time,
                            TimeCmp() );
    if ( it == M_dispinfo_cont.end() )
    {
        return 0;
    }

    return std::distance( M_dispinfo_cont.begin(), it );
}

/*-------------------------------------------------------------------*/
/*!

 */
const
rcss::rcg::PlayerTypeT &
DispHolder::playerType( const int id ) const
{
    std::map< int, rcss::rcg::PlayerTypeT >::const_iterator it
        = M_player_types.find( id );

    if ( it == M_player_types.end() )
    {
        return M_default_player_type;
    }

    return it->second;
}

/*-------------------------------------------------------------------*/
/*!

 */
bool
DispHolder::addDispInfo1( const rcss::rcg::dispinfo_t & disp )
{
    if ( M_dispinfo_cont.size() >= 65535 )
    {
        std::cerr << "over the maximum number of showinfo."
                  << std::endl;
        return true;
    }

    switch ( ntohs( disp.mode ) ) {
    case rcss::rcg::NO_INFO:
        break;
    case rcss::rcg::SHOW_MODE:
        {
            DispPtr new_disp( new rcss::rcg::DispInfoT );

            M_playmode = static_cast< rcss::rcg::PlayMode >( disp.body.show.pmode );
            rcss::rcg::convert( disp.body.show.team[0], M_teams[0] );
            rcss::rcg::convert( disp.body.show.team[1], M_teams[1] );

            new_disp->pmode_ = M_playmode;
            new_disp->team_[0] = M_teams[0];
            new_disp->team_[1] = M_teams[1];
            rcss::rcg::convert( disp.body.show, new_disp->show_ );

            M_last_disp = new_disp;
            if ( new_disp->show_.time_ > 0
                 || M_dispinfo_cont.empty() )
            {
                M_dispinfo_cont.push_back( new_disp );
            }
            else
            {
                M_dispinfo_cont.back() =  new_disp;
            }
        }
        break;
    case rcss::rcg::MSG_MODE:
        doHandleMsgInfo( 0,
                         ntohs( disp.body.msg.board ),
                         disp.body.msg.message );
        break;
    case rcss::rcg::DRAW_MODE:
        // TODO
        break;
    default:
        return false;
    }

    return true;
}

/*-------------------------------------------------------------------*/
/*!

 */
bool
DispHolder::addDispInfo2( const rcss::rcg::dispinfo_t2 & disp )
{
    if ( M_dispinfo_cont.size() >= 65535 )
    {
        std::cerr << "over the maximum number of showinfo."
                  << std::endl;
        return true;
    }


    switch ( ntohs( disp.mode ) ) {
    case rcss::rcg::NO_INFO:
        break;
    case rcss::rcg::SHOW_MODE:
        {
            DispPtr new_disp( new rcss::rcg::DispInfoT );

            M_playmode = static_cast< rcss::rcg::PlayMode >( disp.body.show.pmode );
            rcss::rcg::convert( disp.body.show.team[0], M_teams[0] );
            rcss::rcg::convert( disp.body.show.team[1], M_teams[1] );

            new_disp->pmode_ = M_playmode;
            new_disp->team_[0] = M_teams[0];
            new_disp->team_[1] = M_teams[1];
            rcss::rcg::convert( disp.body.show, new_disp->show_ );

            M_last_disp = new_disp;
            if ( new_disp->show_.time_ > 0
                 || M_dispinfo_cont.empty() )
            {
                M_dispinfo_cont.push_back( new_disp );
            }
            else
            {
                M_dispinfo_cont.back() =  new_disp;
            }
        }
        break;

    case rcss::rcg::MSG_MODE:
        doHandleMsgInfo( 0,
                         ntohs( disp.body.msg.board ),
                         disp.body.msg.message );
        break;
    case rcss::rcg::PARAM_MODE:
        {
            rcss::rcg::ServerParamT new_params;
            rcss::rcg::convert( disp.body.sparams, new_params );

            doHandleServerParam( new_params );
        }
        break;
    case rcss::rcg::PPARAM_MODE:
        {
            rcss::rcg::PlayerParamT new_params;
            rcss::rcg::convert( disp.body.pparams, new_params );

            doHandlePlayerParam( new_params );
        }
        break;
    case rcss::rcg::PT_MODE:
        {
            rcss::rcg::PlayerTypeT new_type;
            convert( disp.body.ptinfo, new_type );

            doHandlePlayerType( new_type );
        }
        break;
    case rcss::rcg::DRAW_MODE:
    case rcss::rcg::BLANK_MODE:
    case rcss::rcg::PM_MODE:
    case rcss::rcg::TEAM_MODE:
        break;
    default:
        return false;
    }

    return true;
}

/*-------------------------------------------------------------------*/
/*!

 */
bool
DispHolder::addDispInfo3( const char * msg )
{
    if ( M_dispinfo_cont.size() >= 65535 )
    {
        std::cerr << "over the maximum number of showinfo."
                  << std::endl;
        return true;
    }

    rcss::rcg::Parser parser( *this );

    return parser.parseLine( -1, msg );
}

/*-------------------------------------------------------------------*/
/*!

 */
void
DispHolder::doHandleLogVersion( int ver )
{
    M_log_version = ver;
}

/*-------------------------------------------------------------------*/
/*!

 */
int
DispHolder::doGetLogVersion() const
{
    return M_log_version;
}

/*-------------------------------------------------------------------*/
/*!

 */
void
DispHolder::doHandleShowInfo( const rcss::rcg::ShowInfoT & show )
{
    if ( M_dispinfo_cont.size() >= 65535 )
    {
        std::cerr << "over the maximum number of showinfo."
                  << std::endl;
        return;
    }

    DispPtr disp( new rcss::rcg::DispInfoT );

    disp->pmode_ = M_playmode;
    disp->team_[0] = M_teams[0];
    disp->team_[1] = M_teams[1];
    disp->show_ = show;

    M_last_disp = disp;

    if ( M_playmode == rcss::rcg::PM_BeforeKickOff )
    {
        if ( M_dispinfo_cont.empty()
             || M_dispinfo_cont.back()->pmode_ != rcss::rcg::PM_BeforeKickOff )
        {
            M_dispinfo_cont.push_back( disp );
        }
        else
        {
            M_dispinfo_cont.back() = disp;
        }
    }
    else if ( M_playmode == rcss::rcg::PM_TimeOver )
    {
        if ( M_dispinfo_cont.empty()
             || M_dispinfo_cont.back()->pmode_ != rcss::rcg::PM_TimeOver )
        {
            M_dispinfo_cont.push_back( disp );
        }
        else
        {
            M_dispinfo_cont.back() = disp;
        }
    }
    else
    {
        M_dispinfo_cont.push_back( disp );
    }
}

/*-------------------------------------------------------------------*/
/*!

 */
void
DispHolder::doHandleMsgInfo( const int,
                             const int,
                             const std::string & msg )
{
    if ( ! msg.compare( 0, std::strlen( "(team_graphic_" ), "(team_graphic_" ) )
    {
        analyzeTeamGraphic( msg );
        return;
    }
    else if ( ! msg.compare( 0, std::strlen( "(change_player_type" ), "(change_player_type" ) )
    {
        return;
    }

    //     std::cerr << "handle message: time=" << time << " board=" << board
    //               << " [" << msg << "]" << std::endl;
}

/*-------------------------------------------------------------------*/
/*!

 */
void
DispHolder::doHandlePlayMode( const int time,
                              const rcss::rcg::PlayMode pmode )
{
    if ( M_playmode != pmode )
    {
        if ( pmode == rcss::rcg::PM_PenaltyScore_Left
             || pmode == rcss::rcg::PM_PenaltyMiss_Left )
        {
            //             int cycle = ( M_last_disp
            //                           ? M_last_disp->show_.time_
            //                           : 0 );
            M_penalty_scores_left.push_back( std::make_pair( time, pmode ) );
        }
        else if ( pmode == rcss::rcg::PM_PenaltyScore_Right
                  || pmode == rcss::rcg::PM_PenaltyMiss_Right )
        {
            //             int cycle = ( M_last_disp
            //                           ? M_last_disp->show_.time_
            //                           : 0 );
            M_penalty_scores_right.push_back( std::make_pair( time, pmode ) );
        }
    }

    M_playmode = pmode;
}

/*-------------------------------------------------------------------*/
/*!

 */
void
DispHolder::doHandleTeamInfo( const int,
                              const rcss::rcg::TeamT & team_l,
                              const rcss::rcg::TeamT & team_r )
{
    if ( M_teams[0].score_ != team_l.score_
         || M_teams[1].score_ != team_r.score_ )
    {
        M_score_changed_index.push_back( M_dispinfo_cont.size() );
    }

    M_teams[0] = team_l;
    M_teams[1] = team_r;
}

/*-------------------------------------------------------------------*/
/*!

 */
void
DispHolder::doHandleDrawClear( const int time )
{
    M_point_map.erase( time );
    M_circle_map.erase( time );
    M_line_map.erase( time );
}

/*-------------------------------------------------------------------*/
/*!

 */
void
DispHolder::doHandleDrawPointInfo( const int time,
                                   const rcss::rcg::PointInfoT & point )
{
    M_point_map.insert( std::pair< int, rcss::rcg::PointInfoT >( time, point ) );
}

/*-------------------------------------------------------------------*/
/*!

 */
void
DispHolder::doHandleDrawCircleInfo( const int time,
                                    const rcss::rcg::CircleInfoT & circle )
{
    M_circle_map.insert( std::pair< int, rcss::rcg::CircleInfoT >( time, circle ) );
}

/*-------------------------------------------------------------------*/
/*!

 */
void
DispHolder::doHandleDrawLineInfo( const int time,
                                  const rcss::rcg::LineInfoT & line )
{
    M_line_map.insert( std::pair< int, rcss::rcg::LineInfoT >( time, line ) );
}

/*-------------------------------------------------------------------*/
/*!

 */
void
DispHolder::doHandleServerParam( const rcss::rcg::ServerParamT & param )
{
    M_server_param = param;
}

/*-------------------------------------------------------------------*/
/*!

 */
void
DispHolder::doHandlePlayerParam( const rcss::rcg::PlayerParamT & param )
{
    M_player_param = param;
}

/*-------------------------------------------------------------------*/
/*!

 */
void
DispHolder::doHandlePlayerType( const rcss::rcg::PlayerTypeT & param )
{
    M_player_types.insert( std::pair< int, rcss::rcg::PlayerTypeT >( param.id_, param ) );
}

/*-------------------------------------------------------------------*/
/*!

 */
void
DispHolder::doHandleEOF()
{

}

/*-------------------------------------------------------------------*/
/*!

 */
void
DispHolder::analyzeTeamGraphic( const std::string & msg )
{
    char side = 'n';
    int x = -1, y = -1;

    if ( std::sscanf( msg.c_str(),
                      "(team_graphic_%c ( %d %d ",
                      &side, &x, &y ) != 3
         || ( side != 'l' && side != 'r' )
         || x < 0
         || y < 0 )
    {
        std::cerr << "Illegal team_graphic message ["
                  << msg << "]" << std::endl;
        return;
    }

    if ( side == 'l' )
    {
        if ( ! M_team_graphic_left.parse( msg.c_str() ) )
        {
            std::cerr << "Illegal team_graphic message ["
                      << msg << "]" << std::endl;
        }
        return;
    }

    if ( side == 'r' )
    {
        if ( ! M_team_graphic_right.parse( msg.c_str() ) )
        {
            std::cerr << "Illegal team_graphic message ["
                      << msg << "]" << std::endl;
        }
        return;
    }
}
