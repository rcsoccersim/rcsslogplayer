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

using namespace rcss::rcg;


class RCGSerializer {
public:
    virtual
    ~RCGSerializer()
      { }

    virtual
    std::ostream & serializeHeader( std::ostream & os ) = 0;
    virtual
    std::ostream & serializePlayMode( std::ostream & os,
                                      PlayMode pm ) = 0;
    virtual
    std::ostream & serializeTeams( std::ostream & os,
                                   const TeamT & team_l,
                                   const TeamT & team_r ) = 0;
    virtual
    std::ostream & serializeShow( std::ostream & os,
                                  const ShowInfoT & show ) = 0;
    virtual
    std::ostream & serializeMsg( std::ostream & os,
                                 const int time,
                                 const int board,
                                 const std::string & msg ) = 0;
    virtual
    std::ostream & serializeServerParam( std::ostream & os,
                                         const ServerParamT & param ) = 0;
    virtual
    std::ostream & serializePlayerParam( std::ostream & os,
                                         const PlayerParamT & param ) = 0;
    virtual
    std::ostream & serializePlayerType( std::ostream & os,
                                        const PlayerTypeT & type ) = 0;
};

class RCGSerializerV1
    : public RCGSerializer {
protected:
    PlayMode M_playmode;
    TeamT M_team_l;
    TeamT M_team_r;
public:
    RCGSerializerV1()
        : M_playmode( PM_Null )
      { }

    virtual
    std::ostream & serializeHeader( std::ostream & os )
      {
          return os;
      }

    virtual
    std::ostream & serializePlayMode( std::ostream & os,
                                      PlayMode pm )
      {
          M_playmode = pm;
          return os;
      }
    virtual
    std::ostream & serializeTeams( std::ostream & os,
                                   const TeamT & team_l,
                                   const TeamT & team_r )
      {
          M_team_l = team_l;
          M_team_r = team_r;
          return os;
      }
    virtual
    std::ostream & serializeShow( std::ostream & os,
                                  const ShowInfoT & show );
    virtual
    std::ostream & serializeMsg( std::ostream & os,
                                 const int time,
                                 const int board,
                                 const std::string & msg );
    virtual
    std::ostream & serializeServerParam( std::ostream & os,
                                         const ServerParamT & )
      {
          return os;
      }
    virtual
    std::ostream & serializePlayerParam( std::ostream & os,
                                         const PlayerParamT & )
      {
          return os;
      }
    virtual
    std::ostream & serializePlayerType( std::ostream & os,
                                        const PlayerTypeT & )
      {
          return os;
      }
};

class RCGSerializerV2
    : public RCGSerializerV1 {
public:
    virtual
    std::ostream & serializeHeader( std::ostream & os );
    //virtual
    //std::ostream & serializePlayMode( std::ostream & os,
    //                                  PlayMode pm );
    //virtual
    //std::ostream & serializeTeams( std::ostream & os,
    //                               const TeamT & team_l,
    //                               const TeamT & team_r );
    virtual
    std::ostream & serializeShow( std::ostream & os,
                                  const ShowInfoT & show );
    virtual
    std::ostream & serializeMsg( std::ostream & os,
                                 const int time,
                                 const int board,
                                 const std::string & msg );
    //virtual
    //std::ostream & serializeServerParam( std::ostream & os,
    //                                     const ServerParamT & param );
    //virtual
    //std::ostream & serializePlayerParam( std::ostream & os,
    //                                     const PlayerParamT & param );
    //virtual
    //std::ostream & serializePlayerType( std::ostream & os,
    //                                    const PlayerTypeT & type );
};

class RCGSerializerV3
    : public RCGSerializerV2 {
public:
    virtual
    std::ostream & serializeHeader( std::ostream & os );
    virtual
    std::ostream & serializePlayMode( std::ostream & os,
                                      PlayMode pm );
    virtual
    std::ostream & serializeTeams( std::ostream & os,
                                   const TeamT & team_l,
                                   const TeamT & team_r );
    virtual
    std::ostream & serializeShow( std::ostream & os,
                                  const ShowInfoT & show );
    //virtual
    //std::ostream & serializeMsg( std::ostream & os,
    //                             const int time,
    //                             const int board,
    //                             const std::string & msg );
    virtual
    std::ostream & serializeServerParam( std::ostream & os,
                                         const ServerParamT & param );
    virtual
    std::ostream & serializePlayerParam( std::ostream & os,
                                         const PlayerParamT & param );
    virtual
    std::ostream & serializePlayerType( std::ostream & os,
                                        const PlayerTypeT & type );
};

class RCGSerializerV4
    : public RCGSerializerV3 {
protected:
    int M_time;
public:
    RCGSerializerV4()
        : RCGSerializerV3(),
          M_time( 0 )
      { }

    virtual
    std::ostream & serializeHeader( std::ostream & os );
    virtual
    std::ostream & serializePlayMode( std::ostream & os,
                                      PlayMode pm );
    virtual
    std::ostream & serializeTeams( std::ostream & os,
                                   const TeamT & team_l,
                                   const TeamT & team_r );
    virtual
    std::ostream & serializeShow( std::ostream & os,
                                  const ShowInfoT & show );
    virtual
    std::ostream & serializeMsg( std::ostream & os,
                                 const int time,
                                 const int board,
                                 const std::string & msg );
    virtual
    std::ostream & serializeServerParam( std::ostream & os,
                                         const ServerParamT & param );
    virtual
    std::ostream & serializePlayerParam( std::ostream & os,
                                         const PlayerParamT & param );
    virtual
    std::ostream & serializePlayerType( std::ostream & os,
                                        const PlayerTypeT & type );
};

class RCGSerializerV5
    : public RCGSerializerV4 {
public:
    virtual
    std::ostream & serializeHeader( std::ostream & os );
    //virtual
    //std::ostream & serializePlayMode( std::ostream & os,
    //                                 PlayMode pm );
    //virtual
    //std::ostream & serializeTeams( std::ostream & os,
    //                               const TeamT & team_l,
    //                               const TeamT & team_r );
    virtual
    std::ostream & serializeShow( std::ostream & os,
                                  const ShowInfoT & show );
    //virtual
    //std::ostream & serializeMsg( std::ostream & os,
    //                             const int time,
    //                             const int board,
    //                             const std::string & msg );
    //virtual
    //std::ostream & serializeServerParam( std::ostream & os,
    //                                     const ServerParamT & param );
    //virtual
    //std::ostream & serializePlayerParam( std::ostream & os,
    //                                     const PlayerParamT & param );
    //virtual
    //std::ostream & serializePlayerType( std::ostream & os,
    //                                    const PlayerTypeT & type );
};



class RCGConvert
    : public Handler {
private:
    // options
    std::string M_input_file; //!< input file path
    std::string M_output_file; //!< output file path
    int M_output_version;
    bool M_verbose;


    int M_version; //!< format version of the input log file
    int M_time;
    PlayMode M_playmode;
    TeamT M_team_l;
    TeamT M_team_r;

    std::istream * M_in; //!< input stream
    std::ostream * M_out; //!< output stream

    RCGSerializer * M_serializer;
public:
    RCGConvert()
        : M_version( 0 ),
          M_time( -1 ),
          M_playmode( PM_Null ),
          M_in( static_cast< std::istream * >( 0 ) ),
          M_out( static_cast< std::ostream * >( 0 ) ),
          M_serializer( static_cast< RCGSerializer * >( 0 ) )
      { }

    ~RCGConvert()
      {
          this->close();
      }


    bool parseCmdLine( int argc,
                       char ** argv );

    std::istream * input()
      {
          return M_in;
      }

private:

    bool open( const std::string & input_file,
               const std::string & output_file );
    bool createSerializer();

    void close();

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

/*--------------------------------------------------------------------*/
std::ostream &
RCGSerializerV1::serializeShow( std::ostream & os,
                                const ShowInfoT & show )
{
    dispinfo_t disp;

    disp.mode = htons( SHOW_MODE );
    convert( M_playmode,
             M_team_l, M_team_r,
             show,
             disp.body.show );

    os.write( reinterpret_cast< const char * >( &disp ),
              sizeof( dispinfo_t ) );
    return os;
}

/*--------------------------------------------------------------------*/
std::ostream &
RCGSerializerV1::serializeMsg( std::ostream & os,
                               const int,
                               const int board,
                               const std::string & msg )
{
    dispinfo_t disp;

    disp.mode = htons( MSG_MODE );

    disp.body.msg.board = htons( static_cast< Int16 >( board ) );
    std::memset( disp.body.msg.message, 0, sizeof( disp.body.msg.message ) );
    std::strncpy( disp.body.msg.message,
                  msg.c_str(),
                  std::min( sizeof( disp.body.msg.message ) - 1,
                            msg.length() ) );

    os.write( reinterpret_cast< const char * >( &disp ),
              sizeof( dispinfo_t ) );
    return os;
}


/*--------------------------------------------------------------------*/

/*--------------------------------------------------------------------*/
std::ostream &
RCGSerializerV2::serializeHeader( std::ostream & os )
{
    char header[5];
    header[0] = 'U';
    header[1] = 'L';
    header[2] = 'G';
    header[3] = static_cast< char >( REC_VERSION_2 );

    os.write( header, 4 );
    return os;
}

/*--------------------------------------------------------------------*/
std::ostream &
RCGSerializerV2::serializeShow( std::ostream & os,
                                const ShowInfoT & show )
{
    Int16 mode = htons( SHOW_MODE );
    showinfo_t show1;

    convert( M_playmode,
             M_team_l, M_team_r,
             show,
             show1 );

    os.write( reinterpret_cast< char * >( &mode ),
              sizeof( Int16 ) );
    os.write( reinterpret_cast< const char * >( &show1 ),
              sizeof( showinfo_t ) );
    return os;
}

/*--------------------------------------------------------------------*/
std::ostream &
RCGSerializerV2::serializeMsg( std::ostream & os,
                               const int,
                               const int board,
                               const std::string & msg )
{
    Int16 mode = htons( MSG_MODE );
    os.write( reinterpret_cast< char * >( &mode ),
              sizeof( Int16 ) );

    msginfo_t info;
    info.board = htons( static_cast< Int16 >( board ) );
    std::memset( info.message, 0, sizeof( info.message ) );
    std::strncpy( info.message, msg.c_str(),
                  std::min( sizeof( info.message ) - 1, msg.length() ) );

    os.write( reinterpret_cast< const char * >( &info.board ),
              sizeof( Int16 ) );

    Int16 len = 1;
    while ( info.message[len-1] != '\0'
            && len < 2048 )
    {
        ++len;
    }

    Int16 nlen = htons( len );
    os.write( reinterpret_cast< const char* >( &nlen ),
              sizeof( Int16 ) );

    os.write( reinterpret_cast< const char * >( info.message ),
              len );
    return os;
}


/*--------------------------------------------------------------------*/

/*--------------------------------------------------------------------*/
std::ostream &
RCGSerializerV3::serializeHeader( std::ostream & os )
{
    char header[5];
    header[0] = 'U';
    header[1] = 'L';
    header[2] = 'G';
    header[3] = static_cast< char >( REC_VERSION_3 );

    os.write( header, 4 );
    return os;
}

/*--------------------------------------------------------------------*/
std::ostream &
RCGSerializerV3::serializePlayMode( std::ostream & os,
                                    PlayMode pm )
{
    M_playmode = pm;

    Int16 mode = htons( PM_MODE );
    os.write( reinterpret_cast< char * >( &mode ),
              sizeof( Int16 ) );

    char pmode = static_cast< char >( M_playmode );
    os.write( reinterpret_cast< char * >( &pmode ),
              sizeof( char ) );

    return os;
}

/*--------------------------------------------------------------------*/
std::ostream &
RCGSerializerV3::serializeTeams( std::ostream & os,
                                 const TeamT & team_l,
                                 const TeamT & team_r )
{
    M_team_l = team_l;
    M_team_r = team_r;

    Int16 mode = htons( TEAM_MODE );
    os.write( reinterpret_cast< char * >( &mode ),
              sizeof( Int16 ) );

    team_t tl, tr;
    convert( team_l, tl );
    convert( team_r, tr );
    os.write( reinterpret_cast< const char * >( &tl ),
              sizeof( team_t ) );
    os.write( reinterpret_cast< const char * >( &tr ),
              sizeof( team_t ) );
    return os;
}

/*--------------------------------------------------------------------*/
std::ostream &
RCGSerializerV3::serializeShow( std::ostream & os,
                                const ShowInfoT & show )
{
    Int16 mode = htons( SHOW_MODE );
    os.write( reinterpret_cast< char * >( &mode ),
              sizeof( Int16 ) );

    short_showinfo_t2 short_show2;
    convert( show, short_show2 );
    os.write( reinterpret_cast< const char * >( &short_show2 ),
              sizeof( short_showinfo_t2 ) );

    return os;
}

/*--------------------------------------------------------------------*/
std::ostream &
RCGSerializerV3::serializeServerParam( std::ostream & os,
                                       const ServerParamT & param )
{
    Int16 mode = htons( PARAM_MODE );
    os.write( reinterpret_cast< char * >( &mode ),
              sizeof( Int16 ) );

    server_params_t p;
    convert( param, p );
    os.write( reinterpret_cast< const char * >( &p ),
              sizeof( server_params_t ) );

    return os;
}

/*--------------------------------------------------------------------*/
std::ostream &
RCGSerializerV3::serializePlayerParam( std::ostream & os,
                                       const PlayerParamT & param )
{
    Int16 mode = htons( PPARAM_MODE );
    os.write( reinterpret_cast< char * >( &mode ),
              sizeof( Int16 ) );

    player_params_t p;
    convert( param, p );
    os.write( reinterpret_cast< const char * >( &p ),
              sizeof( player_params_t ) );
    return os;
}

/*--------------------------------------------------------------------*/
std::ostream &
RCGSerializerV3::serializePlayerType( std::ostream & os,
                                      const PlayerTypeT & type )
{
    Int16 mode = htons( PT_MODE );
    os.write( reinterpret_cast< char * >( &mode ),
              sizeof( Int16 ) );

    player_type_t t;
    convert( type, t );
    os.write( reinterpret_cast< const char * >( &t ),
              sizeof( player_type_t ) );

    return os;
}

/*--------------------------------------------------------------------*/

/*--------------------------------------------------------------------*/
std::ostream &
RCGSerializerV4::serializeHeader( std::ostream & os )
{
    os << "ULG4\n";
    return os;
}

/*--------------------------------------------------------------------*/
std::ostream &
RCGSerializerV4::serializePlayMode( std::ostream & os,
                                    PlayMode pm )
{
    static const char * playmode_strings[] = PLAYMODE_STRINGS;

    M_playmode = pm;

    if ( pm < PM_Null || PM_MAX <= pm )
    {
        return os;
    }

    os << "(playmode " << M_time << ' ' << playmode_strings[pm] << ")\n";

    return os;
}

/*--------------------------------------------------------------------*/
std::ostream &
RCGSerializerV4::serializeTeams( std::ostream & os,
                                 const TeamT & team_l,
                                 const TeamT & team_r )
{
    M_team_l = team_l;
    M_team_r = team_r;

    os << "(team " << M_time
       << ' ' << ( team_l.name_.empty() ? "null" : team_l.name_.c_str() )
       << ' ' << ( team_r.name_.empty() ? "null" : team_r.name_.c_str() )
       << ' ' << team_l.score_
       << ' ' << team_r.score_;
    if ( team_l.penaltyTrial() > 0 || team_r.penaltyTrial() > 0 )
    {
        os << ' ' << team_l.pen_score_ << ' ' << team_l.pen_miss_
           << ' ' << team_r.pen_score_ << ' ' << team_r.pen_miss_;
    }
    os << ")\n";

    return os;
}

/*--------------------------------------------------------------------*/
std::ostream &
RCGSerializerV4::serializeShow( std::ostream & os,
                                const ShowInfoT & show )
{
    M_time = show.time_;

    os << "(show " << show.time_;

    // ball

    os << " ((b)"
       << ' ' << show.ball_.x_ << ' ' << show.ball_.y_;
    if ( show.ball_.hasVelocity() )
    {
        os << ' ' << show.ball_.vx_ << ' ' << show.ball_.vy_;
    }
    else
    {
        os << " 0 0";
    }
    os << ')';

    // players

    for ( int i = 0; i < MAX_PLAYER*2; ++i )
    {
        const PlayerT & p = show.player_[i];

        os << " ((" << p.side_ << ' ' << p.unum_ << ')';
        os << ' ' << p.type_;
        os << ' ' << std::hex << std::showbase
           << p.state_
           << std::dec << std::noshowbase;

        os << ' ' << p.x_ << ' ' << p.y_;
        if ( p.hasVelocity() )
        {
            os << ' ' << p.vx_ << ' ' << p.vy_;
        }
        else
        {
            os << " 0 0";
        }
        os << ' ' << p.body_
           << ' ' << ( p.hasNeck() ? p.neck_ : 0.0f );

        if ( p.isPointing() )
        {
            os << ' ' << p.point_x_ << ' ' << p.point_y_;
        }

        if ( p.hasView() )
        {
            os << " (v " << p.view_quality_ << ' ' << p.view_width_ << ')';
        }
        else
        {
            os << " (v h 90)";
        }

        if ( p.hasStamina() )
        {
            os << " (s " << p.stamina_
               << ' ' << p.effort_
               << ' ' << p.recovery_
               << ')';
        }
        else
        {
            os << " (s 4000 1 1)";
        }

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

    return os;
}

/*--------------------------------------------------------------------*/
std::ostream &
RCGSerializerV4::serializeMsg( std::ostream & os,
                               const int time,
                               const int board,
                               const std::string & msg )
{
    M_time = time;

    os << "(msg " << time << ' ' << board
       << " \"" << msg << "\")\n";

    return os;
}

/*--------------------------------------------------------------------*/
std::ostream &
RCGSerializerV4::serializeServerParam( std::ostream & os,
                                       const ServerParamT & param )
{
    return param.print( os ) << '\n';
}


/*--------------------------------------------------------------------*/
std::ostream &
RCGSerializerV4::serializePlayerParam( std::ostream & os,
                                       const PlayerParamT & param )
{
    return param.print( os ) << '\n';
}

/*--------------------------------------------------------------------*/
std::ostream &
RCGSerializerV4::serializePlayerType( std::ostream & os,
                                      const PlayerTypeT & type )
{
    return type.print( os ) << '\n';
}


/*--------------------------------------------------------------------*/

/*--------------------------------------------------------------------*/
std::ostream &
RCGSerializerV5::serializeHeader( std::ostream & os )
{
    os << "ULG5\n";
    return os;
}

/*--------------------------------------------------------------------*/
std::ostream &
RCGSerializerV5::serializeShow( std::ostream & os,
                                const ShowInfoT & show )
{
    M_time = show.time_;

    os << "(show " << show.time_;

    // ball

    os << " ((b)"
       << ' ' << show.ball_.x_ << ' ' << show.ball_.y_;
    if ( show.ball_.hasVelocity() )
    {
        os << ' ' << show.ball_.vx_ << ' ' << show.ball_.vy_;
    }
    else
    {
        os << " 0 0";
    }
    os << ')';

    // players

    for ( int i = 0; i < MAX_PLAYER*2; ++i )
    {
        const PlayerT & p = show.player_[i];

        os << " ((" << p.side_ << ' ' << p.unum_ << ')';
        os << ' ' << p.type_;
        os << ' ' << std::hex << std::showbase
           << p.state_
           << std::dec << std::noshowbase;

        os << ' ' << p.x_ << ' ' << p.y_;
        if ( p.hasVelocity() )
        {
            os << ' ' << p.vx_ << ' ' << p.vy_;
        }
        else
        {
            os << " 0 0";
        }
        os << ' ' << p.body_
           << ' ' << ( p.hasNeck() ? p.neck_ : 0.0f );

        if ( p.isPointing() )
        {
            os << ' ' << p.point_x_ << ' ' << p.point_y_;
        }

        if ( p.hasView() )
        {
            os << " (v " << p.view_quality_ << ' ' << p.view_width_ << ')';
        }
        else
        {
            os << " (v h 90)";
        }

        if ( p.hasStamina() )
        {
            os << " (s " << p.stamina_
               << ' ' << p.effort_
               << ' ' << p.recovery_
               << ' ' << p.stamina_capacity_
               << ')';
        }
        else
        {
            os << " (s 4000 1 1 -1)";
        }

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

    return os;
}


/*--------------------------------------------------------------------*/

/*--------------------------------------------------------------------*/
bool
RCGConvert::parseCmdLine( int argc,
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
        ( "version,v",
          po::value< int >( &M_output_version )->default_value( DEFAULT_REC_VERSION ),
          "set a version number of the output game log.")
        ( "output,o",
          po::value< std::string >( &M_output_file )->default_value( "" ),
          "set a file path of the output game log file(.rcg). '-' means standard output."  )
        ;

    po::options_description invisibles( "Invisibles" );
    invisibles.add_options()
        ( "input",
          po::value< std::string >( &M_input_file )->default_value( "" ),
          "set the input path of the opened Game Log file(.rcg). '-' means standard input."  )
        ;

    po::options_description all_desc( "All options" );
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
    catch ( const std::exception & e )
    {
        std::cerr << e.what() << std::endl;
        help = true;
    }

    if ( help
         || M_input_file.empty()
         || M_output_file.empty() )
    {
#ifdef HAVE_LIBZ
        std::cerr << "Usage: rcgconvert [options ... ] <GameLogFile>[.gz]\n";
#else
        std::cerr << "Usage: rcgconvert [options ... ] <GameLogFile>\n";
#endif
        std::cerr << "  If input file string is '-', the standard input is used as an input.\n\n";
        std::cerr << visibles << std::endl;
        return false;
    }

    if ( M_output_version < REC_OLD_VERSION
         || DEFAULT_REC_VERSION < M_output_version )
    {
        std::cerr << "rcgconvert: unsupported game log version " << M_output_version
                  << " is set as the output version."
                  << std::endl;
        return false;
    }

    if ( ! open( M_input_file, M_output_file ) )
    {
        return false;
    }

    if ( ! createSerializer() )
    {
        return false;
    }

    return true;

#else // HAVE_BOOST_PROGRAM_OPTIONS
    std::cerr << "rcgconvert: boost::program_options is not available."
              << std::endl;
    return false;
#endif
}

/*--------------------------------------------------------------------*/
bool
RCGConvert::open( const std::string & input_file,
                  const std::string & output_file )
{
    // open the input stream
    if ( input_file.empty()
         || input_file == "-" )
    {
        M_in = &std::cin;
        std::cerr << "rcgconvert: input file = stdin" << std::endl;
    }
    else
    {
        if ( input_file.length() > 3
             && input_file.compare( input_file.length() - 3, 3, ".gz" ) == 0 )
        {
#ifndef HAVE_LIBZ
            std::cerr << "No zlib support!" << std::endl;
            return false;
#else
            M_in = new rcss::gzifstream( input_file.c_str() );
#endif
        }
        else
        {
            M_in = new std::ifstream( input_file.c_str() );
        }

        if ( ! *M_in )
        {
            std::cerr << "rcgconvert could not open the input file ." << input_file
                      << std::endl;
            return false;
        }
        std::cerr << "rcgconvert: input file = [" << input_file << ']' << std::endl;
    }

    // open the output stream
    if ( output_file == "-" )
    {
        M_out = &std::cout;
        std::cerr << "rcgconvert: output file = stdout" << std::endl;
    }
    else
    {
        if ( output_file.length() > 3
             && output_file.compare( output_file.length() - 3, 3, ".gz" ) == 0 )
        {
#ifndef HAVE_LIBZ
            std::cerr << "No zlib support!" << std::endl;
            return false;
#else
            M_out = new rcss::gzofstream( output_file.c_str() );
#endif
        }
        else
        {
            M_out = new std::ofstream( output_file.c_str() );
        }

        if ( ! *M_out )
        {
            std::cerr << "rcgconvert could not open the output file ." << output_file
                      << std::endl;
            return false;
        }
        std::cerr << "rcgconvert: output file = [" << output_file << ']' << std::endl;
    }

    return true;
}


/*--------------------------------------------------------------------*/
void
RCGConvert::close()
{
    if ( M_in
         && M_in != &std::cin )
    {
        delete M_in;
        M_in = static_cast< std::istream * >( 0 );
    }

    if ( M_out
         && M_out != &std::cout )
    {
        if ( *M_out )
        {
            M_out->flush();
        }
        delete M_out;
        M_out = static_cast< std::ostream * >( 0 );
    }
}

/*--------------------------------------------------------------------*/
bool
RCGConvert::createSerializer()
{
    // TODO:
    // replaced with factory.

    switch ( M_output_version ) {
    case REC_VERSION_5:
        M_serializer = new RCGSerializerV5();
        break;
    case REC_VERSION_4:
        M_serializer = new RCGSerializerV4();
        break;
    case REC_VERSION_3:
        M_serializer = new RCGSerializerV3();
        break;
    case REC_VERSION_2:
        M_serializer = new RCGSerializerV2();
        break;
    case REC_OLD_VERSION:
        M_serializer = new RCGSerializerV1();
        break;
    default:
        std::cerr << "rcgconvert could not create the serializer for version "
                  << M_output_version << std::endl;
        return false;
    }

    return ( M_serializer != static_cast< RCGSerializer * >( 0 ) );
}

/*--------------------------------------------------------------------*/
void
RCGConvert::doHandleLogVersion( int ver )
{
    if ( ! M_out
         || ! *M_out
         || ! M_serializer )
    {
        return;
    }

    M_version = ver;

    if ( ver < REC_OLD_VERSION
         || DEFAULT_REC_VERSION < ver )
    {
        std::cerr << "rcgconvert: unsupported game log version " << ver
                  << std::endl;
        this->close();
        std::exit( 1 );
        return;
    }

    if ( M_output_version == M_version )
    {
        std::cerr << "rcgconvert: input game log version is same as the specified output version(="
                  << ver << ')' << std::endl;
        this->close();
        std::exit( 1 );
        return;
    }

    std::cerr << "rcgconvert: input game log version = " << ver << '\n';
    std::cerr << "rcgconvert: output game log version = " << M_output_version
              << std::endl;

    M_serializer->serializeHeader( *M_out );
}

/*--------------------------------------------------------------------*/
void
RCGConvert::doHandleShowInfo( const ShowInfoT & show )
{
    static PlayMode s_playmode = PM_Null;
    static TeamT s_teams[2];

    if ( ! M_out
         || ! *M_out
         || ! M_serializer )
    {
        return;
    }

    if ( M_verbose )
    {
        std::cerr << "rcgconvert: handle show info. time = " << M_time
                  << std::endl;
    }

    M_time = show.time_;

    if ( s_playmode != M_playmode )
    {
        if ( M_verbose )
        {
            std::cerr << "rcgconvert: serialize playmode " << M_playmode
                      << std::endl;
        }

        s_playmode = M_playmode;
        M_serializer->serializePlayMode( *M_out, M_playmode );
    }

    if  ( ! s_teams[0].equals( M_team_l )
          || ! s_teams[1].equals( M_team_r ) )
    {
        if ( M_verbose )
        {
            std::cerr << "rcgconvert: serialize teams."<< std::endl;
        }

        s_teams[0] = M_team_l;
        s_teams[1] = M_team_r;
        M_serializer->serializeTeams( *M_out, M_team_l, M_team_r );
    }

    M_serializer->serializeShow( *M_out, show );
}

/*--------------------------------------------------------------------*/
void
RCGConvert::doHandleMsgInfo( const int time,
                             const int board,
                             const std::string & msg )
{
    if ( ! M_out
         || ! *M_out
         || ! M_serializer )
    {
        return;
    }

    if ( M_verbose )
    {
        std::cerr << "rcgconvert: handle msg info. time = " << M_time
                  << std::endl;
    }

    M_time = time;

    M_serializer->serializeMsg( *M_out, time, board, msg );
}

/*--------------------------------------------------------------------*/
void
RCGConvert::doHandleEOF()
{
    if ( M_verbose )
    {
        std::cerr << "End." << std::endl;
    }

    if ( ! M_out
         || ! *M_out )
    {
        return;
    }

    M_out->flush();
}

/*--------------------------------------------------------------------*/
void
RCGConvert::doHandlePlayMode( const int time,
                              const PlayMode pm )
{
    M_time = time;
    M_playmode = pm;
}

/*--------------------------------------------------------------------*/
void
RCGConvert::doHandleTeamInfo( const int time,
                              const TeamT & team_l,
                              const TeamT & team_r )
{
    M_time = time;
    M_team_l = team_l;
    M_team_r = team_r;
}

/*--------------------------------------------------------------------*/
void
RCGConvert::doHandlePlayerType( const PlayerTypeT & param )
{
    if ( ! M_out
         || ! *M_out
         || ! M_serializer )
    {
        return;
    }

    if ( M_verbose )
    {
        std::cerr << "rcgconvert: handle player_type"
                  << std::endl;
    }

    M_serializer->serializePlayerType( *M_out, param );
}

/*--------------------------------------------------------------------*/
void
RCGConvert::doHandlePlayerParam( const PlayerParamT & param )
{
    if ( ! M_out
         || ! *M_out
         || ! M_serializer )
    {
        return;
    }

    if ( M_verbose )
    {
        std::cerr << "rcgconvert: handle player_param"
                  << std::endl;
    }

    M_serializer->serializePlayerParam( *M_out, param );
}

/*--------------------------------------------------------------------*/
void
RCGConvert::doHandleServerParam( const ServerParamT & param )
{
    if ( ! M_out
         || ! *M_out
         || ! M_serializer )
    {
        return;
    }

    if ( M_verbose )
    {
        std::cerr << "rcgconvert: handle server_param"
                  << std::endl;
    }

    M_serializer->serializeServerParam( *M_out, param );
}


/*--------------------------------------------------------------------*/

int
main( int argc, char ** argv )
{
    RCGConvert converter;

    if ( ! converter.parseCmdLine( argc, argv ) )
    {
        return 1;
    }

    std::istream * in = converter.input();
    if ( ! in
         || ! *in )
    {
        std::cerr << "rcgconvert could not open the input stream."
                  << std::endl;
        return 1;
    }

    rcss::rcg::Parser parser( converter );

    int count = -1;
    while ( parser.parse( *in ) )
    {
        if ( ++count % 512 == 0 )
        {
            std::fprintf( stderr, "parsing... %d\r", count );
            std::fflush( stderr );
        }
    }

    return 0;
}
