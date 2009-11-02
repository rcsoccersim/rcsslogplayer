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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rcsslogplayer/parser.h>
#include <rcsslogplayer/handler.h>

#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif
#ifdef HAVE_WINDOWS_H
#include <windows.h>
#endif

#include <iostream>
#include <limits>
#include <cstring>

static int MAX_TIME = std::numeric_limits< short >::max();

void
gsub( std::string & orig,
      const std::string & remove,
      const std::string & replacement )
{
    for ( std::string::size_type i = orig.find( remove );
          i != std::string::npos;
          i = orig.find( remove, i + 1 ) )
    {
        orig.replace( i, remove.size(),
                      replacement );
    }
}

void
XMLEscape( std::string & orig )
{
    gsub( orig, "&", "&amp;" );
    gsub( orig, "<", "&lt;" );
    gsub( orig, ">", "&gt;" );
    gsub( orig, "'", "&apos;" );
    gsub( orig, "\"", "&quot;" );
}
void
UnXMLEscape( std::string & orig )
{
    gsub( orig, "&amp;", "&" );
    gsub( orig, "&lt;", "<" );
    gsub( orig, "&gt;", ">" );
    gsub( orig, "&apos;", "'" );
    gsub( orig, "&quot;" , "\"" );
}

rcss::rcg::PlayMode
play_mode_id( const char * mode )
{
    static const char * playmode_strings[] = PLAYMODE_STRINGS;

    for ( int n = 0; n < rcss::rcg::PM_MAX; ++n )
    {
        if ( ! std::strcmp( playmode_strings[n], mode ) )
        {
            return static_cast< rcss::rcg::PlayMode >( n );
        }
    }
    return rcss::rcg::PM_Null;
}


namespace rcss {
namespace rcg {

class XMLWriter
    : public rcg::Handler {
private:
    int M_version;
    int M_time;

public:
    XMLWriter()
        : M_time( 0 )
      { }

private:
    void doHandleLogVersion( int ver )
      {
          M_version = ver;
          std::cout << "<?xml version=\"1.0\"?>\n"
                    << "<RCG xmlns:xsi=\""
                    << "http://www.w3.org/2001/XMLSchema-instance"
                    << "\" xsi:noNamespaceSchemaLocation=\""
                    << "http://sserver.sf.net/xml-schema/rcg/rcg-0.1.xsd"
                    << "\" version=\"" << ver << "\">\n";
      }

    int doGetLogVersion() const
      {
          return M_version;
      }

    void doHandleEOF()
      {
          std::cout << "</RCG>\n";
      }

    template< typename VALUE_TYPE >
    void printParam( const char * name,
                     VALUE_TYPE value )
      {
          std::cout << "<Param name=\"" << name << "\">" << value << "</Param>\n";
      }

    void printParam( const char * name,
                     const char * value )
      {
          std::string name_str( name );
          std::string value_str( value );
          XMLEscape( name_str );
          XMLEscape( value_str );
          std::cout << "<Param name=\"" << name_str << "\">" << value_str << "</Param>\n";
      }

    void printParam( const char * name,
                     const std::string & value )
      {
          std::string name_str( name );
          std::string value_str( value );
          XMLEscape( name_str );
          XMLEscape( value_str );
          std::cout << "<Param name=\"" << name_str << "\">" << value_str << "</Param>\n";
      }


    void doHandleServerParam( const ServerParamT & param )
      {
          std::cout << "<ServerParam>\n";
          printParam( "goal_width", param.goal_width_ );
          printParam( "inertia_moment", param.inertia_moment_ );
          printParam( "player_size", param.player_size_ );
          printParam( "player_decay", param.player_decay_ );
          printParam( "player_rand", param.player_rand_ );
          printParam( "player_weight", param.player_weight_ );
          printParam( "player_speed_max", param.player_speed_max_ );
          printParam( "player_accel_max", param.player_accel_max_ );
          printParam( "stamina_max", param.stamina_max_ );
          printParam( "stamina_inc_max", param.stamina_inc_max_ );
          printParam( "recover_init", param.recover_init_ );
          printParam( "recover_dec_thr", param.recover_dec_thr_ );
          printParam( "recover_min", param.recover_min_ );
          printParam( "recover_dec", param.recover_dec_ );
          printParam( "effort_init", param.effort_init_ );
          printParam( "effort_dec_thr", param.effort_dec_thr_ );
          printParam( "effort_min", param.effort_min_ );
          printParam( "effort_dec", param.effort_dec_ );
          printParam( "effort_inc_thr", param.effort_inc_thr_ );
          printParam( "effort_inc", param.effort_inc_ );
          printParam( "kick_rand", param.kick_rand_ );
          printParam( "team_actuator_noise", param.team_actuator_noise_ );
          printParam( "prand_factor_l", param.player_rand_factor_l_ );
          printParam( "prand_factor_r", param.player_rand_factor_r_ );
          printParam( "kick_rand_factor_l", param.kick_rand_factor_l_ );
          printParam( "kick_rand_factor_r", param.kick_rand_factor_r_ );
          printParam( "ball_size", param.ball_size_ );
          printParam( "ball_decay", param.ball_decay_ );
          printParam( "ball_rand", param.ball_rand_ );
          printParam( "ball_weight", param.ball_weight_ );
          printParam( "ball_speed_max", param.ball_speed_max_ );
          printParam( "ball_accel_max", param.ball_accel_max_ );
          printParam( "dash_power_rate", param.dash_power_rate_ );
          printParam( "kick_power_rate", param.kick_power_rate_ );
          printParam( "kickable_margin", param.kickable_margin_ );
          printParam( "control_radius", param.control_radius_ );
          //printParam( "control_radius_width", param.control_radius_width_ );
          //printParam( "kickable_area", param.kickable_area_ );
          printParam( "catch_probability", param.catch_probability_ );
          printParam( "catchable_area_l", param.catchable_area_l_ );
          printParam( "catchable_area_w", param.catchable_area_w_ );
          printParam( "goalie_max_moves", param.goalie_max_moves_ );
          printParam( "maxpower", param.max_power_ );
          printParam( "minpower", param.min_power_ );
          printParam( "maxmoment", param.max_moment_ );
          printParam( "minmoment", param.min_moment_ );
          printParam( "maxneckmoment", param.max_neck_moment_ );
          printParam( "minneckmoment", param.min_neck_moment_ );
          printParam( "maxneckang", param.max_neck_angle_ );
          printParam( "minneckang", param.min_neck_angle_ );
          printParam( "visible_angle", param.visible_angle_ );
          printParam( "visible_distance", param.visible_distance_ );
          printParam( "audio_cut_dist", param.audio_cut_dist_ );
          printParam( "quantize_step", param.quantize_step_ );
          printParam( "quantize_step_l", param.landmark_quantize_step_ );
          //printParam( "dir_qstep", param.dir_quantize_step );
          //printParam( "dist_qstep_l", param.dist_quantize_step_l );
          //printParam( "dist_qstep_r", param.dist_quantize_step_r );
          //printParam( "land_qstep_l", param.landmark_dist_quantize_step_l );
          //printParam( "land_qstep_r", param.landmark_dist_quantize_step_r );
          //printParam( "dir_qstep_l", param.dir_quantize_step_l_ );
          //printParam( "dir_qstep_r", param.dir_quantize_step_r );
          printParam( "ckick_margin", param.corner_kick_margin_ );
          printParam( "wind_dir", param.wind_dir_ );
          printParam( "wind_force", param.wind_force_ );
          printParam( "wind_ang", param.wind_angle_ );
          printParam( "wind_rand", param.wind_rand_ );
          printParam( "wind_none", param.wind_none_ );
          printParam( "wind_random", param.wind_random_ );
          printParam( "half_time", param.half_time_ );
          printParam( "drop_ball_time", param.drop_ball_time_ );
          printParam( "port", param.port_ );
          printParam( "coach_port", param.coach_port_ );
          printParam( "olcoach_port", param.online_coach_port_ );
          printParam( "say_coach_cnt_max", param.say_coach_count_max_ );
          printParam( "say_coach_msg_size", param.say_coach_msg_size_ );
          printParam( "simulator_step", param.simulator_step_ );
          printParam( "send_step", param.send_step_ );
          printParam( "recv_step", param.recv_step_ );
          printParam( "sense_body_step", param.sense_body_step_ );
          //printParam( "lcm_step", param.lcm_step_ );
          printParam( "say_msg_size", param.say_msg_size_ );
          printParam( "clang_win_size", param.clang_win_size_ );
          printParam( "clang_define_win", param.clang_define_win_ );
          printParam( "clang_meta_win", param.clang_meta_win_ );
          printParam( "clang_advice_win", param.clang_advice_win_ );
          printParam( "clang_info_win", param.clang_info_win_ );
          printParam( "clang_del_win", param.clang_del_win_ );
          printParam( "clang_rule_win", param.clang_rule_win_ );
          printParam( "clang_mess_delay", param.clang_mess_delay_ );
          printParam( "clang_mess_per_cycle", param.clang_mess_per_cycle_ );
          printParam( "hear_max", param.hear_max_ );
          printParam( "hear_inc", param.hear_inc_ );
          printParam( "hear_decay", param.hear_decay_ );
          printParam( "catch_ban_cycle", param.catch_ban_cycle_ );
          printParam( "coach", param.coach_mode_ );
          printParam( "coach_w_referee", param.coach_with_referee_mode_ );
          printParam( "old_coach_hear", param.old_coach_hear_ );
          printParam( "send_vi_st", param.send_vi_step_ );
          printParam( "use_offside", param.use_offside_ );
          printParam( "offside_kick_margin", param.offside_kick_margin_ );
          printParam( "forbid_kick_off_offside", param.forbid_kick_off_offside_ );
          printParam( "verbose", param.verbose_ );
          printParam( "offside_active_area_size", param.offside_active_area_size_ );
          printParam( "slow_down_factor", param.slow_down_factor_ );
          printParam( "synch_mode", param.synch_mode_ );//pfr:SYNCH
          printParam( "synch_offset", param.synch_offset_ );//pfr:SYNCH
          printParam( "synch_micro_sleep", param.synch_micro_sleep_ );//pfr:SYNCH
          printParam( "start_goal_l", param.start_goal_l_ );
          printParam( "start_goal_r", param.start_goal_r_ );
          printParam( "fullstate_l", param.fullstate_l_ );
          printParam( "fullstate_r", param.fullstate_r_ );
          printParam( "slowness_on_top_for_left_team", param.slowness_on_top_for_left_team_ );
          printParam( "slowness_on_top_for_right_team", param.slowness_on_top_for_left_team_ );
          printParam( "landmark_file", param.landmark_file_ );
          printParam( "send_comms", param.send_comms_ );
          printParam( "text_logging", param.text_logging_ );
          printParam( "game_logging", param.game_logging_ );
          printParam( "game_log_version", param.game_log_version_ );
          printParam( "text_log_dir", param.text_log_dir_ );
          printParam( "game_log_dir", param.game_log_dir_ );
          printParam( "text_log_fixed_name", param.text_log_fixed_name_ );
          printParam( "game_log_fixed_name", param.game_log_fixed_name_ );
          printParam( "text_log_fixed", param.text_log_fixed_ );
          printParam( "game_log_fixed", param.game_log_fixed_ );
          printParam( "text_log_dated", param.text_log_dated_ );
          printParam( "game_log_dated", param.game_log_dated_ );
          printParam( "log_date_format", param.log_date_format_ );
          printParam( "log_times", param.log_times_ );
          printParam( "record_messages", param.record_messages_ );
          printParam( "text_log_compression", param.text_log_compression_ );
          printParam( "game_log_compression", param.game_log_compression_ );
          printParam( "profile", param.profile_ );
          printParam( "point_to_ban", param.point_to_ban_ );
          printParam( "point_to_duration", param.point_to_duration_ );
          printParam( "tackle_dist", param.tackle_dist_ );
          printParam( "tackle_back_dist", param.tackle_back_dist_ );
          printParam( "tackle_width", param.tackle_width_ );
          printParam( "tackle_exponent", param.tackle_exponent_ );
          printParam( "tackle_cycles", param.tackle_cycles_ );
          printParam( "tackle_power_rate", param.tackle_power_rate_ );
          printParam( "freeform_wait_period", param.freeform_wait_period_ );
          printParam( "freeform_send_period", param.freeform_send_period_ );
          printParam( "free_kick_faults", param.free_kick_faults_ );
          printParam( "back_passes", param.back_passes_ );
          printParam( "proper_goal_kicks", param.proper_goal_kicks_ );
          printParam( "stopped_ball_vel", param.stopped_ball_vel_ );
          printParam( "max_goal_kicks", param.max_goal_kicks_ );
          printParam( "auto_mode", param.auto_mode_ );
          printParam( "kick_off_wait", param.kick_off_wait_ );
          printParam( "connect_wait", param.connect_wait_ );
          printParam( "game_over_wait", param.game_over_wait_ );
          printParam( "team_l_start", param.team_l_start_ );
          printParam( "team_r_start", param.team_r_start_ );
          printParam( "keepaway", param.keepaway_mode_ );
          printParam( "keepaway_length", param.keepaway_length_ );
          printParam( "keepaway_width", param.keepaway_width_ );
          printParam( "keepaway_logging", param.keepaway_logging_ );
          printParam( "keepaway_log_dir", param.keepaway_log_dir_ );
          printParam( "keepaway_log_fixed_name", param.keepaway_log_fixed_name_ );
          printParam( "keepaway_log_fixed", param.keepaway_log_fixed_ );
          printParam( "keepaway_log_dated", param.keepaway_log_dated_ );
          printParam( "keepaway_start", param.keepaway_start_ );
          printParam( "nr_normal_halfs", param.nr_normal_halfs_ );
          printParam( "nr_extra_halfs", param.nr_extra_halfs_ );
          printParam( "penalty_shoot_outs", param.penalty_shoot_outs_ );
          printParam( "pen_before_setup_wait", param.pen_before_setup_wait_ );
          printParam( "pen_setup_wait", param.pen_setup_wait_ );
          printParam( "pen_ready_wait", param.pen_ready_wait_ );
          printParam( "pen_taken_wait", param.pen_taken_wait_ );
          printParam( "pen_nr_kicks", param.pen_nr_kicks_ );
          printParam( "pen_max_extra_kicks", param.pen_max_extra_kicks_ );
          printParam( "pen_dist_x", param.pen_dist_x_ );
          printParam( "pen_random_winner", param.pen_random_winner_ );
          printParam( "pen_max_goalie_dist_x", param.pen_max_goalie_dist_x_ );
          printParam( "pen_allow_mult_kicks", param.pen_allow_mult_kicks_ );
          printParam( "pen_coach_moves_players", param.pen_coach_moves_players_ );
          printParam( "ball_stuck_area", param.ball_stuck_area_ );
          printParam( "coach_msg_file", param.coach_msg_file_ );
          printParam( "max_tackle_power", param.max_tackle_power_ );
          printParam( "max_back_tackle_power", param.max_back_tackle_power_ );
          printParam( "player_speed_max_min", param.player_speed_max_min_ );
          printParam( "extra_stamina", param.extra_stamina_ );
          printParam( "synch_see_offset", param.synch_see_offset_ );
          printParam( "max_monitors", param.max_monitors_ );
          // 14.0.0
          printParam( "tackle_rand_factor", param.tackle_rand_factor_ );
          printParam( "foul_detect_probability", param.foul_detect_probability_ );
          printParam( "foul_exponent", param.foul_exponent_ );
          printParam( "foul_cycles", param.foul_cycles_ );
          printParam( "golden_goal", param.golden_goal_ );
          //printParam( "min_catch_probability", param.min_catch_probability_ );
          //printParam( "reliable_catch_area_l", param.reliable_catch_area_l_ );
          std::cout << "</ServerParam>\n";
      }

    void doHandlePlayerParam( const PlayerParamT & param )
      {
          std::cout << "<PlayerParam>\n";
          printParam( "player_types", param.player_types_ );
          printParam( "subs_max", param.subs_max_ );
          printParam( "pt_max", param.pt_max_ );

          printParam( "allow_mult_default_type", param.player_speed_max_delta_min_ );

          printParam( "player_speed_max_delta_min", param.player_speed_max_delta_min_ );
          printParam( "player_speed_max_delta_max", param.player_speed_max_delta_max_ );
          printParam( "stamina_inc_max_delta_factor", param.stamina_inc_max_delta_factor_ );

          printParam( "player_decay_delta_min", param.player_decay_delta_min_ );
          printParam( "player_decay_delta_max", param.player_decay_delta_max_ );
          printParam( "inertia_moment_delta_factor", param.inertia_moment_delta_factor_ );

          printParam( "dash_power_rate_delta_min", param.dash_power_rate_delta_min_ );
          printParam( "dash_power_rate_delta_max", param.dash_power_rate_delta_max_ );
          printParam( "player_size_delta_factor", param.player_size_delta_factor_ );

          printParam( "kickable_margin_delta_min", param.kickable_margin_delta_min_ );
          printParam( "kickable_margin_delta_max", param.kickable_margin_delta_max_ );
          printParam( "kick_rand_delta_factor", param.kick_rand_delta_factor_ );

          printParam( "extra_stamina_delta_min", param.extra_stamina_delta_min_ );
          printParam( "extra_stamina_delta_max", param.extra_stamina_delta_max_ );
          printParam( "effort_max_delta_factor", param.effort_max_delta_factor_ );
          printParam( "effort_min_delta_factor", param.effort_min_delta_factor_ );
          printParam( "random_seed", param.random_seed_ );

          printParam( "new_dash_power_rate_delta_min", param.new_dash_power_rate_delta_min_ );
          printParam( "new_dash_power_rate_delta_max", param.new_dash_power_rate_delta_max_ );
          printParam( "new_stamina_inc_max_delta_factor", param.new_stamina_inc_max_delta_factor_ );
          // 14.0.0
          printParam( "kick_power_rate_delta_min", param.kick_power_rate_delta_min_ );
          printParam( "kick_power_rate_delta_max", param.kick_power_rate_delta_max_ );
          printParam( "foul_detect_probability_delta_factor", param.foul_detect_probability_delta_factor_ );
          printParam( "catchable_area_l_stretch_min", param.catchable_area_l_stretch_min_ );
          printParam( "catchable_area_l_stretch_max", param.catchable_area_l_stretch_max_ );
          std::cout << "</PlayerParam>\n";
      }

    void doHandlePlayerType( const PlayerTypeT & type )
      {
          std::cout << "<PlayerType id=\"" << type.id_ << "\">\n";
          printParam( "player_speed_max", type.player_speed_max_ );
          printParam( "stamina_inc_max", type.stamina_inc_max_ );
          printParam( "player_decay", type.player_decay_ );
          printParam( "inertia_moment", type.inertia_moment_ );
          printParam( "dash_power_rate", type.dash_power_rate_ );
          printParam( "player_size", type.player_size_ );
          printParam( "kickable_margin", type.kickable_margin_ );
          printParam( "kick_rand", type.kick_rand_ );
          printParam( "extra_stamina", type.extra_stamina_ );
          printParam( "effort_max", type.effort_max_ );
          printParam( "effort_min", type.effort_min_ );
          // 14.0.0
          printParam( "kick_power_rate", type.kick_power_rate_ );
          printParam( "foul_detect_probability", type.foul_detect_probability_ );
          printParam( "catchable_area_l_stretch", type.catchable_area_l_stretch_ );
          std::cout << "</PlayerType>\n";
      }

    void doHandleShowInfo( const ShowInfoT & info )
      {
          std::cout << "<ShowInfo time=\"" << info.time_ << "\">\n";
          print( info.ball_ );
          for( int i = 0; i < MAX_PLAYER * 2; ++i )
          {
              print( info.player_[i] );
          }
          std::cout << "</ShowInfo>\n";
          M_time = info.time_;
      }

    void print( const BallT & ball )
      {
          std::cout << "<Ball>\n";
          printPos( ball.x_,  ball.y_ );
          printVel( ball.vx_, ball.vy_ );
          std::cout << "</Ball>\n";
      }

    void print( const PlayerT & player )
      {
          if ( player.state_ )
          {
              std::cout << "<Player side=\"" << player.side_ << "\"";
              std::cout << " unum=\"" << player.unum_ << "\"";
              std::cout << " type=\"" << player.type_ << "\"";
              if ( player.state_ != 1 ) std::cout << " mode=\"" << player.state_ << "\"";
              std::cout << ">\n";
              printPos( player.x_,  player.y_ );
              printVel( player.vx_, player.vy_ );
              printAngles( player.body_, player.neck_ );
              printView( player.view_width_, player.view_quality_ == 'h' );
              printStamina( player.stamina_, player.effort_, player.recovery_ );
              printCounts( player.kick_count_,
                           player.dash_count_,
                           player.turn_count_,
                           player.say_count_,
                           player.turn_neck_count_,
                           player.catch_count_,
                           player.move_count_,
                           player.change_view_count_ );
              std::cout << "</Player>\n";
          }
      }

    void printPos( double x,
                   double y )
      {
          std::cout << "<X>" << x << "</X><Y>" << y << "</Y>\n";
      }

    void printVel( double x,
                   double y )
      {
          std::cout << "<VelX>" << x << "</VelX><VelY>" << y << "</VelY>\n";
      }

    void printAngles( double body,
                      double head )
      {
          printAngles( body );
          std::cout << "<HeadAng>" << head << "</HeadAng>\n";
      }

    void printAngles( double body )
      {
          if ( M_time < MAX_TIME )
          {
              std::cout << "<BodyAng>" << body << "</BodyAng>\n";
          }
      }

    void printView( double width,
                    short qual )
      {
          std::cout << "<ViewWidth>" << width << "</ViewWidth>\n";
          std::cout << "<ViewQual>" << ( qual ? "high" : "low" ) << "</ViewQual>\n";
      }

    void printStamina( double stamina,
                       double effort,
                       double recovery )
      {
          std::cout << "<Stamina>" << stamina << "</Stamina>\n";
          std::cout << "<Effort>" << effort << "</Effort>\n";
          std::cout << "<Recovery>" << recovery << "</Recovery>\n";
      }

    void printCounts( UInt16 kick,
                      UInt16 dash,
                      UInt16 turn,
                      UInt16 say,
                      UInt16 tneck,
                      UInt16 katch,
                      UInt16 move,
                      UInt16 chg_view )
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

    void printPlayMode( const PlayMode pmode )
      {
          static const char * pm_strings[] = PLAYMODE_STRINGS;
          std::cout << "<PlayMode>";
          if ( pmode < PM_MAX )
          {
              std::cout << pm_strings[ (int)pmode ];
          }
          else
          {
              std::cout << (int)pmode;
          }
          std::cout << "</PlayMode>\n";
      }

    void printTeam( char side,
                    const TeamT & team )
      {
          std::cout << "<Team side=\"" << side << "\">";
          std::string name( team.name_ );
          XMLEscape( name );
          std::cout << "<Name>" << name << "</Name>";
          if ( team.score_ )
          {
              std::cout << "<Score>" << team.score_ << "</Score>";
          }
          std::cout << "</Team>\n";
      }

    void doHandleMsgInfo( const int,
                          const int board,
                          const std::string & msg )
      {
          // format the string
          std::string msgcopy( msg );
          XMLEscape( msgcopy );
          std::cout << "<MsgInfo";
          if ( board != 1 )
          {
              std::cout << " board=\"" << board << "\"";
          }
          std::cout << ">" << msgcopy << "</MsgInfo>\n";
      }

    void doHandlePlayMode( const int,
                           const PlayMode pmode )
      {
          printPlayMode( pmode );
      }

    void doHandleTeamInfo( const int,
                           const TeamT & left,
                           const TeamT & right )
      {
          printTeam( 'l', left );
          printTeam( 'r', right );
      }


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

};

} // end namespace rcg
} // end namespace rcss


int
main()
{
    rcss::rcg::XMLWriter writer;
    rcss::rcg::Parser parser( writer );

    while ( parser.parse( std::cin ) )
    {

    }

    return 0;
}
