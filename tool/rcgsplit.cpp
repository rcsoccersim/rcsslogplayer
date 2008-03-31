// -*-c++-*-

/***************************************************************************
                                rcgsplit.cpp
                         rcg splitter source file
                             -------------------
    begin                : 2007-05-11
    copyright            : (C) 2007 by The RoboCup Soccer Server
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
#include <config.h>
#endif

#include <rcsslogplayer/parser.h>
#include <rcsslogplayer/handler.h>
#include <rcsslogplayer/util.h>

#ifdef HAVE_LIBZ
#include <rcssbase/gzip/gzfstream.hpp>
#endif

#include <boost/program_options.hpp>

#include <string>
#include <vector>
#include <fstream>
#include <cmath>

#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif

namespace rcss {
namespace rcg {


class RCGSplitter
    : public Handler {
private:
    // options
    std::string M_filepath;
    bool M_verbose;
    int M_span_cycle;
    int M_segment_start_cycle;
    int M_segment_end_cycle;

    // game log data
    int M_version;

    ServerParamT M_server_param;
    PlayerParamT M_player_param;
    std::vector< PlayerTypeT > M_player_types;

    int M_time;
    PlayMode M_playmode;
    TeamT M_team_l;
    TeamT M_team_r;

    // output file info
    int M_start_cycle;
    std::ofstream M_fout;

public:
    RCGSplitter()
        : M_span_cycle( 12000 ),
          M_segment_start_cycle( -1 ),
          M_segment_end_cycle( -1 ),
          M_version( 0 ),
          M_start_cycle( 0 )
      { }

    bool parseCmdLine( int argc,
                       char ** argv );

    const
    std::string & filepath() const
      {
          return M_filepath;
      }

private:

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
                          const std::string & );

    virtual
    void doHandlePlayMode( const PlayMode );

    virtual
    void doHandleTeamInfo( const TeamT &,
                           const TeamT & );

    virtual
    void doHandlePlayerType( const PlayerTypeT & );

    virtual
    void doHandlePlayerParam( const PlayerParamT & );

    virtual
    void doHandleServerParam( const ServerParamT & );

    virtual
    void doHandleEOF();


    // utility

    bool createOutputFile( const int cycle );
    void writeHeader();

    void printShowV4( std::ostream & os,
                      const ShowInfoT & show );
    void printShowV3( std::ostream & os,
                      const ShowInfoT & show );
    void printShowV2( std::ostream & os,
                      const ShowInfoT & show );
    void printShowOld( std::ostream & os,
                       const ShowInfoT & show );
};

/*--------------------------------------------------------------------*/
template < typename P >
void
print_param( std::ostream & os,
             const char * name,
             const P & value )
{
    os << '(' << name << ' ' << value << ')';
}

/*--------------------------------------------------------------------*/
template <>
void
print_param< std::string >( std::ostream & os,
                            const char * name,
                            const std::string & value )
{
    os << '(' << name << " \"" << value << "\")";
}

/*--------------------------------------------------------------------*/
void
print( std::ostream & os,
       const PlayerTypeT & param )
{
    os << "(player_type ";
    print_param( os, "id", &param.id_ );
    print_param( os, "player_speed_max", param.player_speed_max_ );
    print_param( os, "stamina_inc_max", param.stamina_inc_max_ );
    print_param( os, "player_decay", param.player_decay_ );
    print_param( os, "inertia_moment", param.inertia_moment_ );
    print_param( os, "dash_power_rate", param.dash_power_rate_ );
    print_param( os, "player_size", param.player_size_ );
    print_param( os, "kickable_margin", param.kickable_margin_ );
    print_param( os, "kick_rand", param.kick_rand_ );
    print_param( os, "extra_stamina", param.extra_stamina_ );
    print_param( os, "effort_max", param.effort_max_ );
    print_param( os, "effort_min", param.effort_min_ );
    os << ")\n";
}

/*--------------------------------------------------------------------*/
void
print( std::ostream & os,
       const PlayerParamT & param )
{
    os << "(player_param ";
    print_param( os, "player_types", param.player_types_ );
    print_param( os, "subs_max", param.subs_max_ );
    print_param( os, "pt_max", param.pt_max_ );
    print_param( os, "allow_mult_default_type", param.allow_mult_default_type_ );
    print_param( os, "player_speed_max_delta_min", param.player_speed_max_delta_min_ );
    print_param( os, "player_speed_max_delta_max", param.player_speed_max_delta_max_ );
    print_param( os, "stamina_inc_max_delta_factor", param.stamina_inc_max_delta_factor_ );
    print_param( os, "player_decay_delta_min", param.player_decay_delta_min_ );
    print_param( os, "player_decay_delta_max", param.player_decay_delta_max_ );
    print_param( os, "inertia_moment_delta_factor", param.inertia_moment_delta_factor_ );
    print_param( os, "dash_power_rate_delta_min", param.dash_power_rate_delta_min_ );
    print_param( os, "dash_power_rate_delta_max", param.dash_power_rate_delta_max_ );
    print_param( os, "player_size_delta_factor", param.player_size_delta_factor_ );
    print_param( os, "kickable_margin_delta_min", param.kickable_margin_delta_min_ );
    print_param( os, "kickable_margin_delta_max", param.kickable_margin_delta_max_ );
    print_param( os, "kick_rand_delta_factor", param.kick_rand_delta_factor_ );
    print_param( os, "extra_stamina_delta_min", param.extra_stamina_delta_min_ );
    print_param( os, "extra_stamina_delta_max", param.extra_stamina_delta_max_ );
    print_param( os, "effort_max_delta_factor", param.effort_max_delta_factor_ );
    print_param( os, "effort_min_delta_factor", param.effort_min_delta_factor_ );
    print_param( os, "random_seed", param.random_seed_ );
    print_param( os, "new_dash_power_rate_delta_min", param.new_dash_power_rate_delta_min_ );
    print_param( os, "new_dash_power_rate_delta_max", param.new_dash_power_rate_delta_max_ );
    print_param( os, "new_stamina_inc_max_delta_factor", param.new_stamina_inc_max_delta_factor_ );
    os << ")\n";

}

/*--------------------------------------------------------------------*/
void
print( std::ostream & os,
       const ServerParamT & param )
{
    os << "(server_param ";
    print_param( os, "goal_width", param.goal_width_ );
    print_param( os, "inertia_moment", param.inertia_moment_ );
    print_param( os, "player_size", param.player_size_ );
    print_param( os, "player_decay", param.player_decay_ );
    print_param( os, "player_rand", param.player_rand_ );
    print_param( os, "player_weight", param.player_weight_ );
    print_param( os, "player_speed_max", param.player_speed_max_ );
    print_param( os, "player_accel_max", param.player_accel_max_ );
    print_param( os, "stamina_max", param.stamina_max_ );
    print_param( os, "stamina_inc_max", param.stamina_inc_max_ );
    print_param( os, "recover_init", param.recover_init_ );
    print_param( os, "recover_dec_thr", param.recover_dec_thr_ );
    print_param( os, "recover_min", param.recover_min_ );
    print_param( os, "recover_dec", param.recover_dec_ );
    print_param( os, "effort_init", param.effort_init_ );
    print_param( os, "effort_dec_thr", param.effort_dec_thr_ );
    print_param( os, "effort_min", param.effort_min_ );
    print_param( os, "effort_dec", param.effort_dec_ );
    print_param( os, "effort_inc_thr", param.effort_inc_thr_ );
    print_param( os, "effort_inc", param.effort_inc_ );
    print_param( os, "kick_rand", param.kick_rand_ );
    print_param( os, "team_actuator_noise", param.team_actuator_noise_ );
    print_param( os, "prand_factor_l", param.player_rand_factor_l_ );
    print_param( os, "prand_factor_r", param.player_rand_factor_r_ );
    print_param( os, "kick_rand_factor_l", param.kick_rand_factor_l_ );
    print_param( os, "kick_rand_factor_r", param.kick_rand_factor_r_ );
    print_param( os, "ball_size", param.ball_size_ );
    print_param( os, "ball_decay", param.ball_decay_ );
    print_param( os, "ball_rand", param.ball_rand_ );
    print_param( os, "ball_weight", param.ball_weight_ );
    print_param( os, "ball_speed_max", param.ball_speed_max_ );
    print_param( os, "ball_accel_max", param.ball_accel_max_ );
    print_param( os, "dash_power_rate", param.dash_power_rate_ );
    print_param( os, "kick_power_rate", param.kick_power_rate_ );
    print_param( os, "kickable_margin", param.kickable_margin_ );
    print_param( os, "control_radius", param.control_radius_ );
    //( "control_radius_width", param.control_radius_width_ );
    //( "kickable_area", param.kickable_area_ );
    print_param( os, "catch_probability", param.catch_probability_ );
    print_param( os, "catchable_area_l", param.catchable_area_l_ );
    print_param( os, "catchable_area_w", param.catchable_area_w_ );
    print_param( os, "goalie_max_moves", param.goalie_max_moves_ );
    print_param( os, "maxpower", param.max_power_ );
    print_param( os, "minpower", param.min_power_ );
    print_param( os, "maxmoment", param.max_moment_ );
    print_param( os, "minmoment", param.min_moment_ );
    print_param( os, "maxneckmoment", param.max_neck_moment_ );
    print_param( os, "minneckmoment", param.min_neck_moment_ );
    print_param( os, "maxneckang", param.max_neck_angle_ );
    print_param( os, "minneckang", param.min_neck_angle_ );
    print_param( os, "visible_angle", param.visible_angle_ );
    print_param( os, "visible_distance", param.visible_distance_ );
    print_param( os, "audio_cut_dist", param.audio_cut_dist_ );
    print_param( os, "quantize_step", param.quantize_step_ );
    print_param( os, "quantize_step_l", param.landmark_quantize_step_ );
    //( "quantize_step_dir", param.dir_quantize_step_ );
    //( "quantize_step_dist_team_l", param.dist_quantize_step_l_ );
    //( "quantize_step_dist_team_r", param.dist_quantize_step_r_ );
    //( "quantize_step_dist_l_team_l", param.landmark_dist_quantize_step_l_ );
    //( "quantize_step_dist_l_team_r", param.landmark_dist_quantize_step_r_ );
    //( "quantize_step_dir_team_l", param.dir_quantize_step_l_ );
    //( "quantize_step_dir_team_r", param.dir_quantize_step_r_ );
    print_param( os, "ckick_margin", param.corner_kick_margin_ );
    print_param( os, "wind_dir", param.wind_dir_ );
    print_param( os, "wind_force", param.wind_force_ );
    print_param( os, "wind_ang", param.wind_angle_ );
    print_param( os, "wind_rand", param.wind_rand_ );
    print_param( os, "wind_none", param.wind_none_ );
    print_param( os, "wind_random", param.wind_random_ );
    print_param( os, "half_time", param.half_time_ );
    print_param( os, "drop_ball_time", param.drop_ball_time_ );
    print_param( os, "port", param.port_ );
    print_param( os, "coach_port", param.coach_port_ );
    print_param( os, "olcoach_port", param.online_coach_port_ );
    print_param( os, "say_coach_cnt_max", param.say_coach_count_max_ );
    print_param( os, "say_coach_msg_size", param.say_coach_msg_size_ );
    print_param( os, "simulator_step", param.simulator_step_ );
    print_param( os, "send_step", param.send_step_ );
    print_param( os, "recv_step", param.recv_step_ );
    print_param( os, "sense_body_step", param.sense_body_step_ );
    //( "lcm_step", param.lcm_step_ ); // not needed
    print_param( os, "say_msg_size", param.say_msg_size_ );
    print_param( os, "clang_win_size", param.clang_win_size_ );
    print_param( os, "clang_define_win", param.clang_define_win_ );
    print_param( os, "clang_meta_win", param.clang_meta_win_ );
    print_param( os, "clang_advice_win", param.clang_advice_win_ );
    print_param( os, "clang_info_win", param.clang_info_win_ );
    print_param( os, "clang_del_win", param.clang_del_win_ );
    print_param( os, "clang_rule_win", param.clang_rule_win_ );
    print_param( os, "clang_mess_delay", param.clang_mess_delay_ );
    print_param( os, "clang_mess_per_cycle", param.clang_mess_per_cycle_ );
    print_param( os, "hear_max", param.hear_max_ );
    print_param( os, "hear_inc", param.hear_inc_ );
    print_param( os, "hear_decay", param.hear_decay_ );
    print_param( os, "catch_ban_cycle", param.catch_ban_cycle_ );
    print_param( os, "coach", param.coach_mode_ );
    print_param( os, "coach_w_referee", param.coach_with_referee_mode_ );
    print_param( os, "old_coach_hear", param.old_coach_hear_ );
    print_param( os, "send_vi_step", param.send_vi_step_ );
    print_param( os, "use_offside", param.use_offside_ );
    print_param( os, "offside_kick_margin", param.offside_kick_margin_ );
    print_param( os, "forbid_kick_off_offside", param.forbid_kick_off_offside_ );
    print_param( os, "verbose", param.verbose_ );
    print_param( os, "offside_active_area_size", param.offside_active_area_size_ );
    print_param( os, "slow_down_factor", param.slow_down_factor_ );
    print_param( os, "synch_mode", param.synch_mode_ );
    print_param( os, "synch_offset", param.synch_offset_ );
    print_param( os, "synch_micro_sleep", param.synch_micro_sleep_ );
    print_param( os, "start_goal_l", param.start_goal_l_ );
    print_param( os, "start_goal_r", param.start_goal_r_ );
    print_param( os, "fullstate_l", param.fullstate_l_ );
    print_param( os, "fullstate_r", param.fullstate_r_ );
    print_param( os, "slowness_on_top_for_left_team", param.slowness_on_top_for_left_team_ );
    print_param( os, "slowness_on_top_for_right_team", param.slowness_on_top_for_right_team_ );
    print_param( os, "landmark_file", param.landmark_file_ );
    print_param( os, "send_comms", param.send_comms_ );
    print_param( os, "text_logging", param.text_logging_ );
    print_param( os, "game_logging", param.game_logging_ );
    print_param( os, "game_log_version", param.game_log_version_ );
    print_param( os, "text_log_dir", param.text_log_dir_ );
    print_param( os, "game_log_dir", param.game_log_dir_ );
    print_param( os, "text_log_fixed_name", param.text_log_fixed_name_ );
    print_param( os, "game_log_fixed_name", param.game_log_fixed_name_ );
    print_param( os, "text_log_fixed", param.text_log_fixed_ );
    print_param( os, "game_log_fixed", param.game_log_fixed_ );
    print_param( os, "text_log_dated", param.text_log_dated_ );
    print_param( os, "game_log_dated", param.game_log_dated_ );
    print_param( os, "log_date_format", param.log_date_format_ );
    print_param( os, "log_times", param.log_times_ );
    print_param( os, "record_messages", param.record_messages_ );
    print_param( os, "text_log_compression", param.text_log_compression_ );
    print_param( os, "game_log_compression", param.game_log_compression_ );
    print_param( os, "profile", param.profile_ );
    print_param( os, "point_to_ban", param.point_to_ban_ );
    print_param( os, "point_to_duration", param.point_to_duration_ );
    print_param( os, "tackle_dist", param.tackle_dist_ );
    print_param( os, "tackle_back_dist", param.tackle_back_dist_ );
    print_param( os, "tackle_width", param.tackle_width_ );
    print_param( os, "tackle_exponent", param.tackle_exponent_ );
    print_param( os, "tackle_cycles", param.tackle_cycles_ );
    print_param( os, "tackle_power_rate", param.tackle_power_rate_ );
    print_param( os, "freeform_wait_period", param.freeform_wait_period_ );
    print_param( os, "freeform_send_period", param.freeform_send_period_ );
    print_param( os, "free_kick_faults", param.free_kick_faults_ );
    print_param( os, "back_passes", param.back_passes_ );
    print_param( os, "proper_goal_kicks", param.proper_goal_kicks_ );
    print_param( os, "stopped_ball_vel", param.stopped_ball_vel_ );
    print_param( os, "max_goal_kicks", param.max_goal_kicks_ );
    print_param( os, "auto_mode", param.auto_mode_ );
    print_param( os, "kick_off_wait", param.kick_off_wait_ );
    print_param( os, "connect_wait", param.connect_wait_ );
    print_param( os, "game_over_wait", param.game_over_wait_ );
    print_param( os, "team_l_start", param.team_l_start_ );
    print_param( os, "team_r_start", param.team_r_start_ );
    print_param( os, "keepaway", param.keepaway_mode_ );
    print_param( os, "keepaway_length", param.keepaway_length_ );
    print_param( os, "keepaway_width", param.keepaway_width_ );
    print_param( os, "keepaway_logging", param.keepaway_logging_ );
    print_param( os, "keepaway_log_dir", param.keepaway_log_dir_ );
    print_param( os, "keepaway_log_fixed_name", param.keepaway_log_fixed_name_ );
    print_param( os, "keepaway_log_fixed", param.keepaway_log_fixed_ );
    print_param( os, "keepaway_log_dated", param.keepaway_log_dated_ );
    print_param( os, "keepaway_start", param.keepaway_start_ );
    print_param( os, "nr_normal_halfs", param.nr_normal_halfs_ );
    print_param( os, "nr_extra_halfs", param.nr_extra_halfs_ );
    print_param( os, "penalty_shoot_outs", param.penalty_shoot_outs_ );
    print_param( os, "pen_before_setup_wait", param.pen_before_setup_wait_ );
    print_param( os, "pen_setup_wait", param.pen_setup_wait_ );
    print_param( os, "pen_ready_wait", param.pen_ready_wait_ );
    print_param( os, "pen_taken_wait", param.pen_taken_wait_ );
    print_param( os, "pen_nr_kicks", param.pen_nr_kicks_ );
    print_param( os, "pen_max_extra_kicks", param.pen_max_extra_kicks_ );
    print_param( os, "pen_dist_x", param.pen_dist_x_ );
    print_param( os, "pen_random_winner", param.pen_random_winner_ );
    print_param( os, "pen_max_goalie_dist_x", param.pen_max_goalie_dist_x_ );
    print_param( os, "pen_allow_mult_kicks", param.pen_allow_mult_kicks_ );
    print_param( os, "pen_coach_moves_players", param.pen_coach_moves_players_ );
    print_param( os, "ball_stuck_area", param.ball_stuck_area_ );
    print_param( os, "coach_msg_file", param.coach_msg_file_ );
    print_param( os, "max_tackle_power", param.max_tackle_power_ );
    print_param( os, "max_back_tackle_power", param.max_back_tackle_power_ );
    print_param( os, "player_speed_max_min", param.player_speed_max_min_ );
    print_param( os, "extra_stamina", param.extra_stamina_ );
    print_param( os, "synch_see_offset", param.synch_see_offset_ );
    print_param( os, "max_monitors", param.max_monitors_ );
    print_param( os, "min_catch_probability", param.min_catch_probability_ );
    print_param( os, "reliable_catch_area_l", param.reliable_catch_area_l_ );
    os << ")\n";
}

/*--------------------------------------------------------------------*/
bool
RCGSplitter::parseCmdLine( int argc,
                           char ** argv )
{
    namespace po = boost::program_options;

    po::options_description visibles( "Allowed options:" );

    visibles.add_options()
        ( "help,h",
          "generates this message." )
        ( "verbose",
          po::bool_switch( &M_verbose )->default_value( false ),
          "verbose output mode." )
        ( "span-cycle,c",
          po::value< int >( &M_span_cycle )->default_value( 12000, "12000"  ),
          "set a split span cycle value" )
        ( "segment-start,s",
          po::value< int >( &M_segment_start_cycle )->default_value( -1, "-1"  ),
          "set a segment start cycle value. (negative value means the first cycle in the input file)" )
        ( "segment-end,e",
          po::value< int >( &M_segment_end_cycle )->default_value( -1, "-1"  ),
          "set a segment end cycle value. (negative value means the end cycle in the input file)" )
        ;

    po::options_description invisibles( "Invisibles" );
    invisibles.add_options()
        ( "file",
          po::value< std::string >( &M_filepath )->default_value( "" ),
          "set the path to Game Log file(.rcg) to be loaded."  )
        ;

    po::options_description all_desc( "All options:" );
    all_desc.add( visibles ).add( invisibles );

    po::positional_options_description pdesc;
    pdesc.add( "file", 1 ); // allowed only one rcg file

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
    catch ( std::exception & e )
    {
        std::cerr << e.what() << std::endl;
        help = true;
    }

    if ( help
         || M_filepath.empty() )
    {
#ifdef HAVE_LIBZ
        std::cout << "Usage: rcgsplit [options ... ] [<GameLogFile>[.gz]]\n";
#else
        std::cout << "Usage: rcgsplit [options ... ] [<GameLogFile>]\n";
#endif
        std::cout << visibles << std::endl;
        return false;
    }

    return true;
}

/*--------------------------------------------------------------------*/
void
RCGSplitter::doHandleLogVersion( int ver )
{
    M_version = ver;

    if ( M_verbose )
    {
        std::cout << "Game Log Version = " << ver << std::endl;
    }
}

/*--------------------------------------------------------------------*/
void
RCGSplitter::doHandleShowInfo( const ShowInfoT & show )
{
    M_time = show.time_;

    if ( ! M_fout.is_open() )
    {
        return;
    }

    if ( M_version == rcss::rcg::REC_VERSION_4 )
    {
        printShowV4( M_fout, show );
    }
    else if ( M_version == rcss::rcg::REC_VERSION_3 )
    {
        printShowV3( M_fout, show );
    }
    else if ( M_version == rcss::rcg::REC_VERSION_2 )
    {
        printShowV2( M_fout, show );
    }
    else
    {
        printShowOld( M_fout, show );
    }
}

/*--------------------------------------------------------------------*/
void
RCGSplitter::printShowV4( std::ostream & os,
                          const ShowInfoT & show )
{
    static const char * s_playmode_strings[] = PLAYMODE_STRINGS;

    static PlayMode s_playmode = PM_Null;
    static TeamT s_teams[2];

    bool new_file = createOutputFile( show.time_ );

    if ( new_file
         || s_playmode != M_playmode )
    {
        os << "(playmode " << M_time
           << " " << s_playmode_strings[s_playmode]
           << ")\n";
    }

    if  ( new_file
          || ! s_teams[0].equals( M_team_l )
          || ! s_teams[1].equals( M_team_r ) )
    {
        s_teams[0] = M_team_l;
        s_teams[1] = M_team_r;

        os << "(team " << M_time
           << ' ' << ( M_team_l.name_.empty() ? "null" : M_team_l.name_.c_str() )
           << ' ' << ( M_team_r.name_.empty() ? "null" : M_team_r.name_.c_str() )
           << ' ' << M_team_l.score_
           << ' ' << M_team_r.score_
           << ' ' << M_team_l.pen_score_
           << ' ' << M_team_r.pen_score_
           << ' ' << M_team_l.pen_miss_
           << ' ' << M_team_r.pen_miss_
           << ")\n";
    }


    os << "(show " << M_time;

    // ball
    os << "((b)"
       << ' ' << show.ball_.x_
       << ' ' << show.ball_.y_
       << ' ' << show.ball_.vx_
       << ' ' << show.ball_.vy_
       << ')';

    // players
    for ( int i = 0; i < MAX_PLAYER*2; ++i )
    {
        const PlayerT & p = show.player_[i];

        os << " (" << p.side_ << ' ' << p.unum_ << ')';
        os << ' ' << p.type_;
        os << ' ' << std::hex << std::showbase << p.state_ << std::dec << std::noshowbase;

        os << ' ' << p.x_ << ' ' << p.y_
           << ' ' << p.vx_ << ' ' << p.vy_
           << ' ' << p.body_ << ' ' << p.neck_;
        if ( p.point_x_ != SHOWINFO_SCALE2F
             && p.point_y_ != SHOWINFO_SCALE2F )
        {
            os << ' ' << p.point_x_ << ' ' << p.point_y_;
        }


        os << " (v " << p.view_quality_ << ' ' << p.view_width_ << ')';

        os << " (s " << p.stamina_ << ' ' << p.effort_ << ' ' << p.recovery_ << ')';

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
}

/*--------------------------------------------------------------------*/
void
RCGSplitter::printShowV3( std::ostream & os,
                          const ShowInfoT & show )
{
    static PlayMode s_playmode = PM_Null;
    static TeamT s_teams[2];

    bool new_file = createOutputFile( show.time_ );

    Int16 mode;

    if ( new_file
         || s_playmode != M_playmode )
    {
        char pm = static_cast< char >( M_playmode );
        s_playmode = M_playmode;

        mode = htons( PM_MODE );
        os.write( reinterpret_cast< const char * >( &mode ),
                  sizeof( Int16 ) );
        os.write( reinterpret_cast< const char * >( &pm ),
                  sizeof( char ) );
    }

    if  ( new_file
          || ! s_teams[0].equals( M_team_l )
          || ! s_teams[1].equals( M_team_r ) )
    {
        team_t teams[2];
        convert( M_team_l, teams[0] );
        convert( M_team_r, teams[1] );
        s_teams[0] = M_team_l;
        s_teams[1] = M_team_r;

        mode = htons( TEAM_MODE );
        os.write( reinterpret_cast< const char * >( &mode ),
                  sizeof( mode ) );
        os.write( reinterpret_cast< const char * >( teams ),
                  sizeof( team_t ) * 2 );
    }

    short_showinfo_t2 new_show;

    convert( show, new_show );

    mode = htons( SHOW_MODE );
    os.write( reinterpret_cast< const char * >( &mode ),
              sizeof( Int16 ) );
    os.write( reinterpret_cast< const char * >( &new_show ),
              sizeof( short_showinfo_t2 ) );

}

/*--------------------------------------------------------------------*/
void
RCGSplitter::printShowV2( std::ostream & os,
                          const ShowInfoT & show )
{
    showinfo_t new_show;

    convert( static_cast< char >( M_playmode ),
             M_team_l,
             M_team_r,
             show,
             new_show );

    Int16 mode = htons( SHOW_MODE );
    os.write( reinterpret_cast< const char * >( &mode ),
              sizeof( Int16 ) );
    os.write( reinterpret_cast< const char * >( &new_show ),
              sizeof( showinfo_t ) );
}

/*--------------------------------------------------------------------*/
void
RCGSplitter::printShowOld( std::ostream & os,
                           const ShowInfoT & show )
{
    dispinfo_t disp;

    disp.mode = htons( SHOW_MODE );

    convert( static_cast< char >( M_playmode ),
             M_team_l,
             M_team_r,
             show,
             disp.body.show );

    os.write( reinterpret_cast< const char * >( &disp ),
              sizeof( dispinfo_t ) );
}

/*--------------------------------------------------------------------*/
void
RCGSplitter::doHandleMsgInfo( const int board,
                              const std::string & msg )
{
    if ( ! M_fout.is_open() )
    {
        return;
    }

    if ( M_version == REC_VERSION_4 )
    {
        M_fout << "(msg " << M_time
               << " " << board
               << " \"" << msg << "\")\n";
    }
    else if ( M_version == REC_VERSION_3
              || M_version == REC_VERSION_2 )
    {
        Int16 mode = htons( MSG_MODE );
        Int16 tmp_board = htons( static_cast< Int16 >( board ) );

        M_fout.write( reinterpret_cast< const char * >( &mode ),
                      sizeof( mode ) );
        M_fout.write( reinterpret_cast< const char * >( &tmp_board ),
                      sizeof( Int16 ) );
        Int16 nlen = htons( static_cast< short >( msg.length() + 1 ) );
        M_fout.write( reinterpret_cast< const char * >( &nlen ),
                      sizeof( Int16 ) );
        M_fout.write( msg.c_str(), msg.length() + 1 );
    }
    else
    {
        dispinfo_t disp;

        disp.mode = htons( MSG_MODE );

        disp.body.msg.board = htons( static_cast< Int16 >( board ) );
        std::memset( disp.body.msg.message, 0,
                     sizeof( disp.body.msg.message ) );
        std::strncpy( disp.body.msg.message,
                      msg.c_str(),
                      std::min( sizeof( disp.body.msg.message ) - 1,
                                msg.length() ) );
        M_fout.write( reinterpret_cast< const char * >( &disp ),
                      sizeof( dispinfo_t ) );
    }
}

/*--------------------------------------------------------------------*/
void
RCGSplitter::doHandleEOF()
{
    if ( M_fout.is_open() )
    {
        M_fout.flush();
        M_fout.close();
    }

    if ( M_verbose )
    {
        std::cout << "End." << std::endl;
    }
}

/*--------------------------------------------------------------------*/
void
RCGSplitter::doHandlePlayMode( const PlayMode pm )
{
    M_playmode = pm;
}

/*--------------------------------------------------------------------*/
void
RCGSplitter::doHandleTeamInfo( const TeamT & team_l,
                               const TeamT & team_r )
{
    M_team_l = team_l;
    M_team_r = team_r;
}

/*--------------------------------------------------------------------*/
void
RCGSplitter::doHandlePlayerType( const PlayerTypeT & param )
{
    M_player_types.push_back( param );
}

/*--------------------------------------------------------------------*/
void
RCGSplitter::doHandlePlayerParam( const PlayerParamT & param )
{
    M_player_param = param;
}

/*--------------------------------------------------------------------*/
void
RCGSplitter::doHandleServerParam( const ServerParamT & param )
{
    M_server_param = param;
}

/*--------------------------------------------------------------------*/
bool
RCGSplitter::createOutputFile( const int cycle )
{
    if ( M_segment_start_cycle > 0
         && cycle < M_segment_start_cycle )
    {
        M_fout.flush();
        M_fout.close();
        return false;
    }

    if ( M_segment_end_cycle > 0
         && M_segment_end_cycle < cycle )
    {
        M_fout.flush();
        M_fout.close();
        return false;
    }

    if ( M_start_cycle == 0
         || cycle >= M_start_cycle + M_span_cycle )
    {
        M_fout.flush();
        M_fout.close();

        M_start_cycle = cycle;

        char filename[256];
        std::snprintf( filename, 255, "%08d-%08d.rcg",
                       M_start_cycle, M_start_cycle + M_span_cycle - 1 );
        M_fout.open( filename,
                     std::ios_base::out | std::ios_base::binary );
        if ( M_verbose )
        {
            std::cout << "new file [" << filename << "]" << std::endl;
        }

        writeHeader();

        return true;
    }

    return false;
}

/*--------------------------------------------------------------------*/
void
RCGSplitter::writeHeader()
{
    if ( ! M_fout.is_open() )
    {
        return;
    }

    if ( M_version == REC_OLD_VERSION )
    {
        return;
    }

    if ( M_version == REC_VERSION_4 )
    {
        M_fout << "ULG4\n";
    }
    else
    {
        char header[5];
        header[0] = 'U';
        header[1] = 'L';
        header[2] = 'G';
        header[3] = static_cast< char >( M_version );

        M_fout.write( header, 4 );
    }

    if ( M_version == REC_VERSION_4 )
    {
        print( M_fout, M_server_param );
        print( M_fout, M_player_param );
        for ( std::vector< PlayerTypeT >::iterator it = M_player_types.begin();
              it != M_player_types.end();
              ++it )
        {
            print( M_fout, *it );
        }
    }
    else if ( M_version == REC_VERSION_3 )
    {
        Int16 mode;

        {
            server_params_t param;
            convert( M_server_param, param );
            mode = htons( PARAM_MODE );
            M_fout.write( reinterpret_cast< const char * >( &mode ),
                          sizeof( mode ) );
            M_fout.write( reinterpret_cast< const char * >( &param ),
                          sizeof( server_params_t ) );
        }

        {
            player_params_t param;
            convert( M_player_param, param );
            mode = htons( PPARAM_MODE );
            M_fout.write( reinterpret_cast< const char * >( &mode ),
                          sizeof( mode ) );
            M_fout.write( reinterpret_cast< const char * >( &param ),
                          sizeof( player_params_t ) );
        }

        mode = htons( PT_MODE );
        for ( std::vector< PlayerTypeT >::iterator it = M_player_types.begin();
              it != M_player_types.end();
              ++it )
        {
            player_type_t param;
            convert( *it, param );
            M_fout.write( reinterpret_cast< const char * >( &mode ),
                          sizeof( mode ) );
            M_fout.write( reinterpret_cast< const char * >( &param ),
                          sizeof( player_type_t ) );
        }
    }
}

}
}

/*--------------------------------------------------------------------*/

int
main( int argc, char ** argv )
{
    rcss::rcg::RCGSplitter splitter;

    if ( ! splitter.parseCmdLine( argc, argv ) )
    {
        return 1;
    }

#ifdef HAVE_LIBZ
    rcss::gz::gzifstream fin( splitter.filepath().c_str() );
#else
    std::ifstream fin( splitter.filepath().c_str() );
#endif

    rcss::rcg::Parser parser( splitter );
    if ( ! parser.parse( fin ) )
    {
        return 1;
    }

    return 0;
}
