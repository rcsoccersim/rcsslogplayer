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

#include <cmath>

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
    if ( ntohs( disp.mode ) != rcss::rcg::SHOW_MODE )
    {
        return true;
    }

    if ( M_dispinfo_cont.size() >= 65535 )
    {
        std::cerr << "over the maximum number of showinfo."
                  << std::endl;
        return true;
    }

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
    return true;
}

/*-------------------------------------------------------------------*/
/*!

 */
bool
DispHolder::addDispInfo2( const rcss::rcg::dispinfo_t2 & disp )
{
    if ( ntohs( disp.mode ) != rcss::rcg::SHOW_MODE )
    {
        return true;
    }

    if ( M_dispinfo_cont.size() >= 65535 )
    {
        std::cerr << "over the maximum number of showinfo."
                  << std::endl;
        return true;
    }

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
    return true;
}

/*-------------------------------------------------------------------*/
/*!

 */
bool
DispHolder::addDispInfo3( const char * msg )
{
    const char * buf = msg;

    if ( std::strncmp( buf, "(show ", 6 ) != 0 )
    {
        return true;
    }

    if ( M_dispinfo_cont.size() >= 65535 )
    {
        std::cerr << "over the maximum number of showinfo."
                  << std::endl;
        return true;
    }


    DispPtr disp( new rcss::rcg::DispInfoT );
    rcss::rcg::ShowInfoT & show = disp->show_;

    int n_read = 0;
    char * next;

    // time
    int time = 0;
    if ( std::sscanf( buf, " ( show %d %n ",
                      &time, &n_read ) != 1 )
    {
        return false;
    }
    buf += n_read;

    show.time_ = static_cast< rcss::rcg::UInt32 >( time );

    while ( *buf == ' ' ) ++buf;
    // playmode
    if ( ! std::strncmp( buf, "(pm", 3 ) )
    {
        int pm = 0;
        if ( std::sscanf( buf,
                          " ( pm %d ) %n ",
                          &pm, &n_read ) == 1 )
        {
            buf += n_read;
            M_playmode = static_cast< rcss::rcg::PlayMode >( pm );
            disp->pmode_ = M_playmode;
        }
    }

    // team
    if ( ! std::strncmp( buf, "(tm", 3 ) )
    {
        char name_l[32], name_r[32];
        int score_l = 0, score_r = 0;
        int pen_score_l = 0, pen_miss_l = 0, pen_score_r = 0, pen_miss_r = 0;

        int n = std::sscanf( buf,
                             " ( tm %31s %31s %d %d %d %d %d %d ",
                             name_l, name_r,
                             &score_l, &score_r,
                             &pen_score_l, &pen_miss_l,
                             &pen_score_r, &pen_miss_r );

        if ( n != 4 && n != 8 )
        {
            std::cerr << "error: n=" << n
                      << " Illegal team info. \"" << buf << "\"" << std::endl;;
            return false;
        }
        while ( *buf != ')' && *buf != '\0' ) ++buf;
        while ( *buf == ')' && *buf != '\0' ) ++buf;

        if ( ! std::strcmp( name_l, "null" ) ) std::memset( name_l, 0, 4 );
        if ( ! std::strcmp( name_r, "null" ) ) std::memset( name_r, 0, 4 );

        M_teams[0] = rcss::rcg::TeamT( name_l, score_l, pen_score_l, pen_miss_l );
        M_teams[1] = rcss::rcg::TeamT( name_r, score_r, pen_score_r, pen_miss_r );

        disp->team_[0] = M_teams[0];
        disp->team_[1] = M_teams[1];
    }

    // ball
    {
        // ((b) x y vx vy)
        while ( *buf == ' ' ) ++buf;
        while ( *buf != '\0' && *buf != ')' ) ++buf;
        while ( *buf == ')' ) ++buf;

        rcss::rcg::BallT & ball = show.ball_;
        ball.x_ = std::strtof( buf, &next ); buf = next;
        ball.y_ = std::strtof( buf, &next ); buf = next;
        ball.vx_ = std::strtof( buf, &next ); buf = next;
        ball.vy_ = std::strtof( buf, &next ); buf = next;
        while ( *buf == ')' ) ++buf;
        while ( *buf == ' ' ) ++buf;

        if ( ball.vy_ == HUGE_VALF )
        {
            std::cerr << "error: "
                      << " Illegal ball info. "
                      << " \"" << buf << "\""
                      << std::endl;;
            return false;
        }
    }

    // players
    // ((side unum) type state x y vx vy body neck [pointx pointy] (v h 90) (s 4000 1 1)[(f side unum)])
    //              (c 1 1 1 1 1 1 1 1 1 1 1))
    for ( int i = 0; i < rcss::rcg::MAX_PLAYER * 2; ++i )
    {
        if ( *buf == '\0' || *buf == ')' ) break;

        // ((side unum)
        while ( *buf == ' ' ) ++buf;
        while ( *buf == '(' ) ++buf;
        char side = *buf;
        if ( side != 'l' && side != 'r' )
        {
            std::cerr << "error: "
                      << " Illegal player side. " << side << ' ' << i
                      << " \"" << buf << "\""
                      << std::endl;;
            return false;
        }

        ++buf;
        long unum = std::strtol( buf, &next, 10 ); buf = next;
        if ( unum < 0 || rcss::rcg::MAX_PLAYER < unum )
        {
            std::cerr << "error: "
                      << " Illegal player unum. " << side << ' ' << i
                      << " \"" << buf << "\""
                      << std::endl;;
            return false;
        }

        while ( *buf == ')' ) ++buf;

        const int idx = ( side == 'l' ? unum - 1 : unum - 1 + rcss::rcg::MAX_PLAYER );

        rcss::rcg::PlayerT & p = show.player_[idx];
        p.side_ = side;
        p.unum_ = static_cast< rcss::rcg::Int16 >( unum );

        // x y vx vy body neck
        p.type_ = static_cast< rcss::rcg::Int16 >( std::strtol( buf, &next, 10 ) ); buf = next;
        p.state_ = static_cast< rcss::rcg::Int32 >( std::strtol( buf, &next, 16 ) ); buf = next;
        p.x_ = std::strtof( buf, &next ); buf = next;
        p.y_ = std::strtof( buf, &next ); buf = next;
        p.vx_ = std::strtof( buf, &next ); buf = next;
        p.vy_ = std::strtof( buf, &next ); buf = next;
        p.body_ = std::strtof( buf, &next ); buf = next;
        p.neck_ = std::strtof( buf, &next ); buf = next;
        while ( *buf == ' ' ) ++buf;

        // x y vx vy body neck
        if ( *buf != '\0' && *buf != '(' )
        {
            p.point_x_ = std::strtof( buf, &next ); buf = next;
            p.point_y_ = std::strtof( buf, &next ); buf = next;
        }

        // (v quality width)
        while ( *buf != '\0' && *buf != 'v' ) ++buf;
        ++buf; // skip 'v'
        while ( *buf == ' ' ) ++buf;
        p.view_quality_ = *buf; ++buf;
        p.view_width_ = std::strtof( buf, &next ); buf = next;

        // (s stamina effort recovery)
        while ( *buf != '\0' && *buf != 's' ) ++buf;
        ++buf; // skip 's' //while ( *buf != '\0' && *buf != ' ' ) ++buf;
        p.stamina_ = std::strtof( buf, &next ); buf = next;
        p.effort_ = std::strtof( buf, &next ); buf = next;
        p.recovery_ = std::strtof( buf, &next ); buf = next;
        while ( *buf != '\0' && *buf != ')' ) ++buf;
        while ( *buf == ')' ) ++buf;

        while ( *buf != '\0' && *buf != '(' ) ++buf;

        // (f side unum)
        if ( *(buf + 1) == 'f' )
        {
            while ( *buf != '\0' && *buf != ' ' ) ++buf;
            while ( *buf == ' ' ) ++buf;
            p.focus_side_ = *buf; ++buf;
            p.focus_unum_ = static_cast< rcss::rcg::Int16 >( std::strtol( buf, &next, 10 ) ); buf = next;
            while ( *buf == ' ' ) ++buf;
            while ( *buf == ')' ) ++buf;
            while ( *buf == ' ' ) ++buf;
        }

        // (c kick dash turn catch move tneck cview say tackle pointto atttention)
        while ( *buf == '(' ) ++buf;
        ++buf; // skip 'c' //while ( *buf != '\0' && *buf != ' ' ) ++buf;
        p.kick_count_ = static_cast< rcss::rcg::UInt16 >( std::strtol( buf, &next, 10 ) ); buf = next;
        p.dash_count_ = static_cast< rcss::rcg::UInt16 >( std::strtol( buf, &next, 10 ) ); buf = next;
        p.turn_count_ = static_cast< rcss::rcg::UInt16 >( std::strtol( buf, &next, 10 ) ); buf = next;
        p.catch_count_ = static_cast< rcss::rcg::UInt16 >( std::strtol( buf, &next, 10 ) ); buf = next;
        p.move_count_ = static_cast< rcss::rcg::UInt16 >( std::strtol( buf, &next, 10 ) ); buf = next;
        p.turn_neck_count_ = static_cast< rcss::rcg::UInt16 >( std::strtol( buf, &next, 10 ) ); buf = next;
        p.change_view_count_ = static_cast< rcss::rcg::UInt16 >( std::strtol( buf, &next, 10 ) ); buf = next;
        p.say_count_ = static_cast< rcss::rcg::UInt16 >( std::strtol( buf, &next, 10 ) ); buf = next;
        p.tackle_count_ = static_cast< rcss::rcg::UInt16 >( std::strtol( buf, &next, 10 ) ); buf = next;
        p.pointto_count_ = static_cast< rcss::rcg::UInt16 >( std::strtol( buf, &next, 10 ) ); buf = next;
        p.attentionto_count_ = static_cast< rcss::rcg::UInt16 >( std::strtol( buf, &next, 10 ) ); buf = next;
        while ( *buf == ')' ) ++buf;
        while ( *buf == ' ' ) ++buf;

        if ( *buf == '\0'
             && i != rcss::rcg::MAX_PLAYER*2 - 1 )
        {
            std::cerr << "error: "
                      << " Illegal player info. " << side << ' ' << unum
                      << " \"" << buf << "\""
                      << std::endl;;
            return false;
        }
    }

    M_last_disp = disp;
    if ( disp->show_.time_ > 0
         || M_dispinfo_cont.empty() )
    {
        M_dispinfo_cont.push_back( disp );
    }
    else
    {
        M_dispinfo_cont.back() =  disp;
    }
    return true;
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
    M_dispinfo_cont.push_back( disp );
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
