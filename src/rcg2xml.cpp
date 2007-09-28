// -*-c++-*-

/***************************************************************************
                                rcg2xml.cpp
                         Converts rcg fil;es to xml files
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


#include "rcgparser.hpp"
#include "rcgdatahandler.hpp"
#include <netinet/in.h>

#include <limits>

static int MAX_TIME = std::numeric_limits<short>::max();

void
gsub( std::string& orig,
      const std::string& remove,
      const std::string& replacement )
{
    for( std::string::size_type i = orig.find( remove );
         i != std::string::npos;
         i = orig.find( remove, i + 1 ) )
    {
        orig.replace( i, remove.size(),
                      replacement );
    }
}

void
XMLEscape( std::string& orig )
{
    gsub( orig, "&", "&amp;" );
    gsub( orig, "<", "&lt;" );
    gsub( orig, ">", "&gt;" );
    gsub( orig, "'", "&apos;" );
    gsub( orig, "\"", "&quot;" );
}
void
UnXMLEscape( std::string& orig )
{
    gsub( orig, "&amp;", "&" );
    gsub( orig, "&lt;", "<" );
    gsub( orig, "&gt;", ">" );
    gsub( orig, "&apos;", "'" );
    gsub( orig, "&quot;" , "\"" );
}


namespace rcss
{
    class XMLWriter
        : public RCGDataHandler
    {
    public:
        XMLWriter()
            : m_time( 0 )
        {}


    private:
        void
        doHandleLogVersion( int ver )
        {
            m_ver = ver;
            std::cout << "<?xml version=\"1.0\"?>\n"
                      << "<RCG xmlns:xsi=\""
                      << "http://www.w3.org/2001/XMLSchema-instance"
                      << "\" xsi:noNamespaceSchemaLocation=\""
                      << "http://sserver.sf.net/xml-schema/rcg/rcg-0.1.xsd"
                      << "\" version=\"" << ver << "\">\n";
        }

        int
        doGetLogVersion() const
        { return m_ver; }

        void
        doHandleEOF()
        {
            std::cout << "</RCG>\n";
        }

        template< typename VALUE_TYPE >
        void
        printParam( const char* name, VALUE_TYPE value )
        {
            std::cout << "<Param name=\"" << name << "\">" << value << "</Param>\n";
        }

        void
        printParam( const char* name, const char* value )
        {
            std::string name_str( value );
            std::string value_str( value );
            XMLEscape( name_str );
            XMLEscape( value_str );
            std::cout << "<Param name=\"" << name_str << "\">" << value_str << "</Param>\n";
        }

        void
        doHandleServerParams( std::streampos,
                              const server_params_t& param )
        {
            std::cout << "<ServerParam>\n";
            printParam( "gwidth", toDouble( param.gwidth ) );
            printParam( "inertia_moment", toDouble( param.inertia_moment ) );
            printParam( "psize", toDouble( param.psize ) );
            printParam( "pdecay", toDouble( param.pdecay ) );
            printParam( "prand", toDouble( param.prand ) );
            printParam( "pweight", toDouble( param.pweight ) );
            printParam( "pspeed_max", toDouble( param.pspeed_max ) );
            printParam( "paccel_max", toDouble( param.paccel_max ) );
            printParam( "stamina_max", toDouble( param.stamina_max ) );
            printParam( "stamina_inc", toDouble( param.stamina_inc ) );
            printParam( "recover_init", toDouble( param.recover_init ) );
            printParam( "recover_dthr", toDouble( param.recover_dthr ) );
            printParam( "recover_min", toDouble( param.recover_min ) );
            printParam( "recover_dec", toDouble( param.recover_dec ) );
            printParam( "effort_init", toDouble( param.effort_init ) );
            printParam( "effort_dthr", toDouble( param.effort_dthr ) );
            printParam( "effort_min", toDouble( param.effort_min ) );
            printParam( "effort_dec", toDouble( param.effort_dec ) );
            printParam( "effort_ithr", toDouble( param.effort_ithr ) );
            printParam( "effort_inc", toDouble( param.effort_inc ) );
            printParam( "kick_rand", toDouble( param.kick_rand ) );
            printParam( "team_actuator_noise", toShort( param.team_actuator_noise ) );
            printParam( "prand_factor_l", toDouble( param.prand_factor_l ) );
            printParam( "prand_factor_r", toDouble( param.prand_factor_r ) );
            printParam( "kick_rand_factor_l", toDouble( param.kick_rand_factor_l ) );
            printParam( "kick_rand_factor_r", toDouble( param.kick_rand_factor_r ) );
            printParam( "bsize", toDouble( param.bsize ) );
            printParam( "bdecay", toDouble( param.bdecay ) );
            printParam( "brand", toDouble( param.brand ) );
            printParam( "bweight", toDouble( param.bweight ) );
            printParam( "bspeed_max", toDouble( param.bspeed_max ) );
            printParam( "baccel_max", toDouble( param.baccel_max ) );
            printParam( "dprate", toDouble( param.dprate ) );
            printParam( "kprate", toDouble( param.kprate ) );
            printParam( "kmargin", toDouble( param.kmargin ) );
            printParam( "ctlradius", toDouble( param.ctlradius ) );
            printParam( "ctlradius_width", toDouble( param.ctlradius_width ) );
            printParam( "maxp", toDouble( param.maxp ) );
            printParam( "minp", toDouble( param.minp ) );
            printParam( "maxm", toDouble( param.maxm ) );
            printParam( "minm", toDouble( param.minm ) );
            printParam( "maxnm", toDouble( param.maxnm ) );
            printParam( "minnm", toDouble( param.minnm ) );
            printParam( "maxn", toDouble( param.maxn ) );
            printParam( "minn", toDouble( param.minn ) );
            printParam( "visangle", toDouble( param.visangle ) );
            printParam( "visdist", toDouble( param.visdist ) );
            printParam( "windir", toDouble( param.windir ) );
            printParam( "winforce", toDouble( param.winforce ) );
            printParam( "winang", toDouble( param.winang ) );
            printParam( "winrand", toDouble( param.winrand ) );
            printParam( "kickable_area", toDouble( param.kickable_area ) );
            printParam( "catch_area_l", toDouble( param.catch_area_l ) );
            printParam( "catch_area_w", toDouble( param.catch_area_w ) );
            printParam( "catch_prob", toDouble( param.catch_prob ) );
            printParam( "goalie_max_moves", toShort( param.goalie_max_moves ) );
            printParam( "ckmargin", toDouble( param.ckmargin ) );
            printParam( "offside_area", toDouble( param.offside_area ) );
            printParam( "win_no", toShort( param.win_no ) );
            printParam( "win_random", toShort( param.win_random ) );
            printParam( "say_cnt_max", toShort( param.say_cnt_max ) );
            printParam( "SayCoachMsgSize", toShort( param.SayCoachMsgSize ) );
            printParam( "clang_win_size", toShort( param.clang_win_size ) );
            printParam( "clang_define_win", toShort( param.clang_define_win ) );
            printParam( "clang_meta_win", toShort( param.clang_meta_win ) );
            printParam( "clang_advice_win", toShort( param.clang_advice_win ) );
            printParam( "clang_info_win", toShort( param.clang_info_win ) );
            printParam( "clang_mess_delay", toShort( param.clang_mess_delay ) );
            printParam( "clang_mess_per_cycle", toShort( param.clang_mess_per_cycle ) );
            printParam( "half_time", toShort( param.half_time ) );
            printParam( "sim_st", toShort( param.sim_st ) );
            printParam( "send_st", toShort( param.send_st ) );
            printParam( "recv_st", toShort( param.recv_st ) );
            printParam( "sb_step", toShort( param.sb_step ) );
            printParam( "lcm_st", toShort( param.lcm_st ) );
            printParam( "SayMsgSize", toShort( param.M_say_msg_size ) );
            printParam( "hear_max", toShort( param.M_hear_max ) );
            printParam( "hear_inc", toShort( param.M_hear_inc ) );
            printParam( "hear_decay", toShort( param.M_hear_decay ) );
            printParam( "cban_cycle", toShort( param.cban_cycle ) );
            printParam( "slow_down_factor", toShort( param.slow_down_factor ) );
            printParam( "useoffside", toShort( param.useoffside ) );
            printParam( "kickoffoffside", toShort( param.kickoffoffside ) );
            printParam( "offside_kick_margin", toDouble( param.offside_kick_margin ) );
            printParam( "audio_dist", toDouble( param.audio_dist ) );
            printParam( "dist_qstep", toDouble( param.dist_qstep ) );
            printParam( "land_qstep", toDouble( param.land_qstep ) );
            printParam( "dir_qstep", toDouble( param.dir_qstep ) );
            printParam( "dist_qstep_l", toDouble( param.dist_qstep_l ) );
            printParam( "dist_qstep_r", toDouble( param.dist_qstep_r ) );
            printParam( "land_qstep_l", toDouble( param.land_qstep_l ) );
            printParam( "land_qstep_r", toDouble( param.land_qstep_r ) );
            printParam( "dir_qstep_l", toDouble( param.dir_qstep_l ) );
            printParam( "dir_qstep_r", toDouble( param.dir_qstep_r ) );
            printParam( "CoachMode", toShort( param.CoachMode ) );
            printParam( "CwRMode", toShort( param.CwRMode ) );
            printParam( "old_hear", toShort( param.old_hear ) );
            printParam( "sv_st", toShort( param.sv_st ) );

            printParam( "synch_mode", toShort( param.synch_mode ) );//pfr:SYNCH
            printParam( "synch_offset", toShort( param.synch_offset ) );//pfr:SYNCH
            printParam( "synch_micro_sleep", toShort( param.synch_micro_sleep ) );//pfr:SYNCH

            printParam( "start_goal_l", toShort( param.start_goal_l ) );
            printParam( "start_goal_r", toShort( param.start_goal_r ) );

            printParam( "fullstate_l", toShort( param.fullstate_l ) );
            printParam( "fullstate_r", toShort( param.fullstate_r ) );

            printParam( "drop_time", toShort( param.drop_time ) );

            printParam( "slowness_on_top_for_left_team", toDouble( param.slowness_on_top_for_left_team ) );
            printParam( "slowness_on_top_for_right_team", toDouble( param.slowness_on_top_for_left_team ) );

            printParam( "ka_length", toDouble( param.ka_length ) );
            printParam( "ka_width", toDouble( param.ka_width ) );

            printParam( "point_to_ban", toShort( param.point_to_ban ) );
            printParam( "point_to_duration", toShort( param.point_to_duration ) );

//              printParam( "pen_before_setup_wait", ( ( (double)(long)ntohs( param.pen_before_setup_wait ) ) / SHOWINFO_SCALE2 ) );
//              printParam( "pen_setup_wait", ( ( (double)(long)ntohs( param.pen_setup_wait ) ) / SHOWINFO_SCALE2 ) );
//              printParam( "pen_ready_wait", ( ( (double)(long)ntohs( param.pen_ready_wait ) ) / SHOWINFO_SCALE2 ) );
//              printParam( "pen_taken_wait", ( ( (double)(long)ntohs( param.pen_taken_wait ) ) / SHOWINFO_SCALE2 ) );
//              printParam( "pen_nr_kicks", ( ( (double)(long)ntohs( param.pen_nr_kicks ) ) / SHOWINFO_SCALE2 ) );
//              printParam( "pen_max_extra_kicks", ( ( (double)(long)ntohs( param.pen_max_extra_kicks ) ) / SHOWINFO_SCALE2 ) );
//              printParam( "pen_dist_x", toDouble( param.pen_dist_x ) );
//              printParam( "pen_random_winner", ( ( (double)(long)ntohs( param.pen_random_winner ) ) / SHOWINFO_SCALE2 ) );
//              printParam( "pen_max_goalie_dist_x", toDouble( param.pen_max_goalie_dist_x ) );
//              printParam( "pen_allow_mult_kicks", ( ( (double)(long)ntohs( param.pen_allow_mult_kicks ) ) / SHOWINFO_SCALE2 ) );

            std::cout << "</ServerParam>\n";
        }

        void
        doHandlePlayerParams( std::streampos,
                              const player_params_t& param )
        {
            std::cout << "<PlayerParam>\n";
            printParam( "player_types", toShort( param.player_types ) );
            printParam( "subs_max", toShort( param.subs_max ) );
            printParam( "pt_max", toShort( param.pt_max ) );

            printParam( "player_speed_max_delta_min", toDouble( param.player_speed_max_delta_min ) );
            printParam( "player_speed_max_delta_max", toDouble( param.player_speed_max_delta_max ) );
            printParam( "stamina_inc_max_delta_factor", toDouble( param.stamina_inc_max_delta_factor ) );

            printParam( "player_decay_delta_min", toDouble( param.player_decay_delta_min ) );
            printParam( "player_decay_delta_max", toDouble( param.player_decay_delta_max ) );
            printParam( "inertia_moment_delta_factor", toDouble( param.inertia_moment_delta_factor ) );

            printParam( "dash_power_rate_delta_min", toDouble( param.dash_power_rate_delta_min ) );
            printParam( "dash_power_rate_delta_max", toDouble( param.dash_power_rate_delta_max ) );
            printParam( "player_size_delta_factor", toDouble( param.player_size_delta_factor ) );

            printParam( "kickable_margin_delta_min", toDouble( param.kickable_margin_delta_min ) );
            printParam( "kickable_margin_delta_max", toDouble( param.kickable_margin_delta_max ) );
            printParam( "kick_rand_delta_factor", toDouble( param.kick_rand_delta_factor ) );

            printParam( "extra_stamina_delta_min", toDouble( param.extra_stamina_delta_min ) );
            printParam( "extra_stamina_delta_max", toDouble( param.extra_stamina_delta_max ) );
            printParam( "effort_max_delta_factor", toDouble( param.effort_max_delta_factor ) );
            printParam( "effort_min_delta_factor", toDouble( param.effort_min_delta_factor ) );
            printParam( "random_seed", toLong( param.random_seed ) );;

            printParam( "new_dash_power_rate_delta_min", toDouble( param.new_dash_power_rate_delta_min ) );
            printParam( "new_dash_power_rate_delta_max", toDouble( param.new_dash_power_rate_delta_max ) );
            printParam( "new_stamina_inc_max_delta_factor", toDouble( param.new_stamina_inc_max_delta_factor ) );
            std::cout << "</PlayerParam>\n";
        }

        void
        doHandlePlayerType( std::streampos,
                            const player_type_t& type )
        {
            std::cout << "<PlayerType id=\"" << (long)ntohs( type.id ) << "\">\n";
            printParam( "player_speed_max", toDouble( type.player_speed_max ) );
            printParam( "stamina_inc_max", toDouble( type.stamina_inc_max ) );
            printParam( "player_decay", toDouble( type.player_decay ) );
            printParam( "inertia_moment", toDouble( type.inertia_moment ) );
            printParam( "dash_power_rate", toDouble( type.dash_power_rate ) );
            printParam( "player_size", toDouble( type.player_size ) );
            printParam( "kickable_margin", toDouble( type.kickable_margin ) );
            printParam( "kick_rand", toDouble( type.kick_rand ) );
            printParam( "extra_stamina", toDouble( type.extra_stamina ) );
            printParam( "effort_max", toDouble( type.effort_max ) );
            printParam( "effort_min", toDouble( type.effort_min ) );
            std::cout << "</PlayerType>\n";
        }

        void
        doHandleShowInfo( std::streampos,
                          const short_showinfo_t2& info )
        {
            std::cout << "<ShowInfo time=\"" << ntohs( info.time ) << "\">\n";
            print( info.ball );
            for( int i = 0; i < MAX_PLAYER * 2; ++i )
            {
                print( i, info.pos[ i ] );
            }
            std::cout << "</ShowInfo>\n";
            m_time = ntohs( info.time );
        }

        void
        print( const ball_t& ball )
        {
            std::cout << "<Ball>\n";
            printPos( toDouble( ball.x ),  toDouble( ball.y ) );
            printVel( toDouble( ball.deltax ), toDouble( ball.deltay ) );
            std::cout << "</Ball>\n";
        }

        void
        printBall( const pos_t& ball )
        {
            std::cout << "<Ball>\n";
            printPos( toDoubleOld( ball.x ),  toDoubleOld( ball.y ) );
            std::cout << "</Ball>\n";
        }

        void
        print( int idx, const player_t& player )
        {
            if( toShort( player.mode ) )
            {
                int unum = idx % MAX_PLAYER;
                char side = ( unum == idx ? 'l' : 'r' );
                std::cout << "<Player side=\"" << side << "\"";
                std::cout << " unum=\"" << unum << "\"";
                int type = toShort( player.type );
                if( type )
                    std::cout << " type=\"" << type << "\"";
                int mode = toShort( player.mode );
                if( mode != 1 )
                    std::cout << " mode=\"" << mode << "\"";
                std::cout << ">\n";
                printPos( toDouble( player.x ),  toDouble( player.y ) );
                printVel( toDouble( player.deltax ), toDouble( player.deltay ) );
                printAngles( toDouble( player.body_angle ), toDouble( player.head_angle ) );
                printView( toDouble( player.view_width ), toShort( player.view_quality ) );
                printStamina( toDouble( player.stamina ), toDouble( player.effort ), toDouble( player.recovery ) );
                printCounts( toShort( player.kick_count ),
                             toShort( player.dash_count ),
                             toShort( player.turn_count ),
                             toShort( player.say_count ),
                             toShort( player.tneck_count ),
                             toShort( player.catch_count ),
                             toShort( player.move_count ),
                             toShort( player.chg_view_count ) );
                std::cout << "</Player>\n";
            }
        }

        void
        printPlayer( int idx, const pos_t& player )
        {
            if( toShort( player.enable ) )
            {
                int unum = idx % MAX_PLAYER;
                char side = ( unum == idx ? 'l' : 'r' );
                std::cout << "<Player side=\"" << side << "\"";
                std::cout << " unum=\"" << unum << "\"";
                int mode = toShort( player.enable );
                if( mode != 1 )
                    std::cout << " mode=\"" << mode << "\"";
                printPos( toDoubleOld( player.x ), toDoubleOld( player.y ) );
                printAngles( toDoubleOld( player.angle ) );
                std::cout << "</Player>\n";
            }
        }

        void
        printPos( double x, double y )
        {
            std::cout << "<X>" << x << "</X><Y>" << y << "</Y>\n";
        }

        void
        printVel( double x, double y )
        {
            std::cout << "<VelX>" << x << "</VelX><VelY>" << y << "</VelY>\n";
        }

        void
        printAngles( double body, double head )
        {
            printAngles( body );
            std::cout << "<HeadAng>" << head << "</HeadAng>\n";
        }

        void
        printAngles( double body )
        {
            if( m_time < MAX_TIME )
            {
            std::cout << "<BodyAng>" << body << "</BodyAng>\n";
}        }

        void
        printView( double width, short qual )
        {
            std::cout << "<ViewWidth>" << width << "</ViewWidth>\n";
            std::cout << "<ViewQual>" << ( qual ? "high" : "low" ) << "</ViewQual>\n";
        }

        void
        printStamina( double stamina, double effort, double recovery )
        {
            std::cout << "<Stamina>" << stamina << "</Stamina>\n";
            std::cout << "<Effort>" << effort << "</Effort>\n";
            std::cout << "<Recovery>" << recovery << "</Recovery>\n";
        }

        void
        printCounts( short kick,
                     short dash,
                     short turn,
                     short say,
                     short tneck,
                     short katch,
                     short move,
                     short chg_view )
        {
            std::cout << "<Count>\n";
            std::cout << "<Kick>" << kick << "</Kick>\n";
            std::cout << "<Dash>" << dash << "</Dash>\n";
            std::cout << "<Turn>" << turn << "</Turn>\n";
            std::cout << "<Say>" << say << "</Say>\n";
            std::cout << "<TurnNeck>" << tneck << "</TurnNeck>\n";
            std::cout << "<Catch>" << katch << "</Catch>\n";
            std::cout << "<Move>" << move << "</Move>\n";
            std::cout << "<ChgView>" << chg_view << "</ChgView>\n";
            std::cout << "</Count>\n";
        }

        static
        double
        toDouble( long value )
        {
            return ( (double)(long)ntohl( value ) ) / SHOWINFO_SCALE2;
        }

        static
        short
        toShort( short value )
        {
            return (short)ntohs( value );
        }

        static
        double
        toDoubleOld( short value )
        { return ( (double)(short)ntohs( value ) ) / SHOWINFO_SCALE; }

        static
        long
        toLong( long value )
        {
            return (long)ntohl( value );
        }

        void
        printPlayMode( char mode )
        {
            static const char* pm_strings[] = PLAYMODE_STRINGS;
            std::cout << "<PlayMode>";
            if( mode < PM_MAX )
            {
                std::cout << pm_strings[ (int)mode ];
            }
            else
                std::cout << (int)mode;
            std::cout << "</PlayMode>\n";
        }

        void
        printTeam( char side, team_t team )
        {
            std::cout << "<Team side=\"" << side << "\">";
            std::string name( team.name );
            XMLEscape( name );
            std::cout << "<Name>" << name << "</Name>";
            team.score = toShort( team.score );
            if( team.score )
                std::cout << "<Score>" << team.score << "</Score>";
            std::cout << "</Team>\n";
        }

        void
        doHandleShowInfo( std::streampos pos,
                          const showinfo_t& info )
        {
            printPlayMode( info.pmode );
            printTeam( 'l', info.team[ 0 ] );
            printTeam( 'r', info.team[ 1 ] );
            std::cout << "<ShowInfo time=\"" << ntohs( info.time ) << "\">\n";
            printBall( info.pos[ 0 ] );
            for( int i = 0; i < MAX_PLAYER * 2; ++i )
            {
                printPlayer( i, info.pos[ i + 1 ] );
            }
            m_time = ntohs( info.time );

            std::cout << "</ShowInfo>\n";
        }

        void
        doHandleMsgInfo( std::streampos,
                         short board,
                         const std::string& msg )
        {
            // format the string
            std::string msgcopy( msg );
            XMLEscape( msgcopy );
            std::cout << "<MsgInfo";
            board = ntohs( board );
            if( board != 1 )
                std::cout << " board=\"" << board << "\"";
            std::cout << ">" << msgcopy << "</MsgInfo>\n";
        }

        void
        doHandlePlayMode( std::streampos,
                              char pmode )
        {
            printPlayMode( pmode );
        }

        void
        doHandleTeamInfo( std::streampos,
                          const team_t& left,
                          const team_t& right )
        {
            printTeam( 'l', left );
            printTeam( 'r', right );
        }

        int m_ver;
        int m_time;
    };
}


int main()
{
    rcss::XMLWriter writer;
    rcss::RCGParser parser( writer );
    try
    {
        while( parser.parse( std::cin ) )
        {
            // do nothing
        }
    }
    catch( const std::string& e )
    {
        std::cerr << e << std::endl;
    }
    catch( ... )
    {
        std::cerr << "Unknown error occured\n";
    }
}
