// -*-c++-*-

/*!
  \file types.cpp
  \brief Type definitions for rcg data format.
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "types.h"

#include <iostream>

namespace {

template < typename P >
void
print_param( std::ostream & os,
             const char * name,
             const P & value )
{
    os << '(' << name << ' ' << value << ')';
}

template <>
void
print_param< std::string >( std::ostream & os,
                            const char * name,
                            const std::string & value )
{
    os << '(' << name << " \"" << value << "\")";
}

}

namespace rcss {
namespace rcg {

std::ostream &
PlayerTypeT::print( std::ostream & os ) const
{
    os << "(player_type ";
    print_param( os, "id", id_ );
    print_param( os, "player_speed_max", player_speed_max_ );
    print_param( os, "stamina_inc_max", stamina_inc_max_ );
    print_param( os, "player_decay", player_decay_ );
    print_param( os, "inertia_moment", inertia_moment_ );
    print_param( os, "dash_power_rate", dash_power_rate_ );
    print_param( os, "player_size", player_size_ );
    print_param( os, "kickable_margin", kickable_margin_ );
    print_param( os, "kick_rand", kick_rand_ );
    print_param( os, "extra_stamina", extra_stamina_ );
    print_param( os, "effort_max", effort_max_ );
    print_param( os, "effort_min", effort_min_ );
    os << ')';

    return os;
}

std::ostream &
PlayerParamT::print( std::ostream & os ) const
{
    os << "(player_param ";
    print_param( os, "player_types", player_types_ );
    print_param( os, "subs_max", subs_max_ );
    print_param( os, "pt_max", pt_max_ );
    print_param( os, "allow_mult_default_type", allow_mult_default_type_ );
    print_param( os, "player_speed_max_delta_min", player_speed_max_delta_min_ );
    print_param( os, "player_speed_max_delta_max", player_speed_max_delta_max_ );
    print_param( os, "stamina_inc_max_delta_factor", stamina_inc_max_delta_factor_ );
    print_param( os, "player_decay_delta_min", player_decay_delta_min_ );
    print_param( os, "player_decay_delta_max", player_decay_delta_max_ );
    print_param( os, "inertia_moment_delta_factor", inertia_moment_delta_factor_ );
    print_param( os, "dash_power_rate_delta_min", dash_power_rate_delta_min_ );
    print_param( os, "dash_power_rate_delta_max", dash_power_rate_delta_max_ );
    print_param( os, "player_size_delta_factor", player_size_delta_factor_ );
    print_param( os, "kickable_margin_delta_min", kickable_margin_delta_min_ );
    print_param( os, "kickable_margin_delta_max", kickable_margin_delta_max_ );
    print_param( os, "kick_rand_delta_factor", kick_rand_delta_factor_ );
    print_param( os, "extra_stamina_delta_min", extra_stamina_delta_min_ );
    print_param( os, "extra_stamina_delta_max", extra_stamina_delta_max_ );
    print_param( os, "effort_max_delta_factor", effort_max_delta_factor_ );
    print_param( os, "effort_min_delta_factor", effort_min_delta_factor_ );
    print_param( os, "random_seed", random_seed_ );
    print_param( os, "new_dash_power_rate_delta_min", new_dash_power_rate_delta_min_ );
    print_param( os, "new_dash_power_rate_delta_max", new_dash_power_rate_delta_max_ );
    print_param( os, "new_stamina_inc_max_delta_factor", new_stamina_inc_max_delta_factor_ );
    os << ')';

    return os;
}

std::ostream &
ServerParamT::print( std::ostream & os ) const
{
    os << "(server_param ";
    print_param( os, "goal_width", goal_width_ );
    print_param( os, "inertia_moment", inertia_moment_ );
    print_param( os, "player_size", player_size_ );
    print_param( os, "player_decay", player_decay_ );
    print_param( os, "player_rand", player_rand_ );
    print_param( os, "player_weight", player_weight_ );
    print_param( os, "player_speed_max", player_speed_max_ );
    print_param( os, "player_accel_max", player_accel_max_ );
    print_param( os, "stamina_max", stamina_max_ );
    print_param( os, "stamina_inc_max", stamina_inc_max_ );
    print_param( os, "recover_init", recover_init_ );
    print_param( os, "recover_dec_thr", recover_dec_thr_ );
    print_param( os, "recover_min", recover_min_ );
    print_param( os, "recover_dec", recover_dec_ );
    print_param( os, "effort_init", effort_init_ );
    print_param( os, "effort_dec_thr", effort_dec_thr_ );
    print_param( os, "effort_min", effort_min_ );
    print_param( os, "effort_dec", effort_dec_ );
    print_param( os, "effort_inc_thr", effort_inc_thr_ );
    print_param( os, "effort_inc", effort_inc_ );
    print_param( os, "kick_rand", kick_rand_ );
    print_param( os, "team_actuator_noise", team_actuator_noise_ );
    print_param( os, "prand_factor_l", player_rand_factor_l_ );
    print_param( os, "prand_factor_r", player_rand_factor_r_ );
    print_param( os, "kick_rand_factor_l", kick_rand_factor_l_ );
    print_param( os, "kick_rand_factor_r", kick_rand_factor_r_ );
    print_param( os, "ball_size", ball_size_ );
    print_param( os, "ball_decay", ball_decay_ );
    print_param( os, "ball_rand", ball_rand_ );
    print_param( os, "ball_weight", ball_weight_ );
    print_param( os, "ball_speed_max", ball_speed_max_ );
    print_param( os, "ball_accel_max", ball_accel_max_ );
    print_param( os, "dash_power_rate", dash_power_rate_ );
    print_param( os, "kick_power_rate", kick_power_rate_ );
    print_param( os, "kickable_margin", kickable_margin_ );
    print_param( os, "control_radius", control_radius_ );
    //( "control_radius_width", control_radius_width_ );
    //( "kickable_area", kickable_area_ );
    print_param( os, "catch_probability", catch_probability_ );
    print_param( os, "catchable_area_l", catchable_area_l_ );
    print_param( os, "catchable_area_w", catchable_area_w_ );
    print_param( os, "goalie_max_moves", goalie_max_moves_ );
    print_param( os, "maxpower", max_power_ );
    print_param( os, "minpower", min_power_ );
    print_param( os, "maxmoment", max_moment_ );
    print_param( os, "minmoment", min_moment_ );
    print_param( os, "maxneckmoment", max_neck_moment_ );
    print_param( os, "minneckmoment", min_neck_moment_ );
    print_param( os, "maxneckang", max_neck_angle_ );
    print_param( os, "minneckang", min_neck_angle_ );
    print_param( os, "visible_angle", visible_angle_ );
    print_param( os, "visible_distance", visible_distance_ );
    print_param( os, "audio_cut_dist", audio_cut_dist_ );
    print_param( os, "quantize_step", quantize_step_ );
    print_param( os, "quantize_step_l", landmark_quantize_step_ );
    //( "quantize_step_dir", dir_quantize_step_ );
    //( "quantize_step_dist_team_l", dist_quantize_step_l_ );
    //( "quantize_step_dist_team_r", dist_quantize_step_r_ );
    //( "quantize_step_dist_l_team_l", landmark_dist_quantize_step_l_ );
    //( "quantize_step_dist_l_team_r", landmark_dist_quantize_step_r_ );
    //( "quantize_step_dir_team_l", dir_quantize_step_l_ );
    //( "quantize_step_dir_team_r", dir_quantize_step_r_ );
    print_param( os, "ckick_margin", corner_kick_margin_ );
    print_param( os, "wind_dir", wind_dir_ );
    print_param( os, "wind_force", wind_force_ );
    print_param( os, "wind_ang", wind_angle_ );
    print_param( os, "wind_rand", wind_rand_ );
    print_param( os, "wind_none", wind_none_ );
    print_param( os, "wind_random", wind_random_ );
    print_param( os, "half_time", half_time_ );
    print_param( os, "drop_ball_time", drop_ball_time_ );
    print_param( os, "port", port_ );
    print_param( os, "coach_port", coach_port_ );
    print_param( os, "olcoach_port", online_coach_port_ );
    print_param( os, "say_coach_cnt_max", say_coach_count_max_ );
    print_param( os, "say_coach_msg_size", say_coach_msg_size_ );
    print_param( os, "simulator_step", simulator_step_ );
    print_param( os, "send_step", send_step_ );
    print_param( os, "recv_step", recv_step_ );
    print_param( os, "sense_body_step", sense_body_step_ );
    //( "lcm_step", lcm_step_ ); // not needed
    print_param( os, "say_msg_size", say_msg_size_ );
    print_param( os, "clang_win_size", clang_win_size_ );
    print_param( os, "clang_define_win", clang_define_win_ );
    print_param( os, "clang_meta_win", clang_meta_win_ );
    print_param( os, "clang_advice_win", clang_advice_win_ );
    print_param( os, "clang_info_win", clang_info_win_ );
    print_param( os, "clang_del_win", clang_del_win_ );
    print_param( os, "clang_rule_win", clang_rule_win_ );
    print_param( os, "clang_mess_delay", clang_mess_delay_ );
    print_param( os, "clang_mess_per_cycle", clang_mess_per_cycle_ );
    print_param( os, "hear_max", hear_max_ );
    print_param( os, "hear_inc", hear_inc_ );
    print_param( os, "hear_decay", hear_decay_ );
    print_param( os, "catch_ban_cycle", catch_ban_cycle_ );
    print_param( os, "coach", coach_mode_ );
    print_param( os, "coach_w_referee", coach_with_referee_mode_ );
    print_param( os, "old_coach_hear", old_coach_hear_ );
    print_param( os, "send_vi_step", send_vi_step_ );
    print_param( os, "use_offside", use_offside_ );
    print_param( os, "offside_kick_margin", offside_kick_margin_ );
    print_param( os, "forbid_kick_off_offside", forbid_kick_off_offside_ );
    print_param( os, "verbose", verbose_ );
    print_param( os, "offside_active_area_size", offside_active_area_size_ );
    print_param( os, "slow_down_factor", slow_down_factor_ );
    print_param( os, "synch_mode", synch_mode_ );
    print_param( os, "synch_offset", synch_offset_ );
    print_param( os, "synch_micro_sleep", synch_micro_sleep_ );
    print_param( os, "start_goal_l", start_goal_l_ );
    print_param( os, "start_goal_r", start_goal_r_ );
    print_param( os, "fullstate_l", fullstate_l_ );
    print_param( os, "fullstate_r", fullstate_r_ );
    print_param( os, "slowness_on_top_for_left_team", slowness_on_top_for_left_team_ );
    print_param( os, "slowness_on_top_for_right_team", slowness_on_top_for_right_team_ );
    print_param( os, "landmark_file", landmark_file_ );
    print_param( os, "send_comms", send_comms_ );
    print_param( os, "text_logging", text_logging_ );
    print_param( os, "game_logging", game_logging_ );
    print_param( os, "game_log_version", game_log_version_ );
    print_param( os, "text_log_dir", text_log_dir_ );
    print_param( os, "game_log_dir", game_log_dir_ );
    print_param( os, "text_log_fixed_name", text_log_fixed_name_ );
    print_param( os, "game_log_fixed_name", game_log_fixed_name_ );
    print_param( os, "text_log_fixed", text_log_fixed_ );
    print_param( os, "game_log_fixed", game_log_fixed_ );
    print_param( os, "text_log_dated", text_log_dated_ );
    print_param( os, "game_log_dated", game_log_dated_ );
    print_param( os, "log_date_format", log_date_format_ );
    print_param( os, "log_times", log_times_ );
    print_param( os, "record_messages", record_messages_ );
    print_param( os, "text_log_compression", text_log_compression_ );
    print_param( os, "game_log_compression", game_log_compression_ );
    print_param( os, "profile", profile_ );
    print_param( os, "point_to_ban", point_to_ban_ );
    print_param( os, "point_to_duration", point_to_duration_ );
    print_param( os, "tackle_dist", tackle_dist_ );
    print_param( os, "tackle_back_dist", tackle_back_dist_ );
    print_param( os, "tackle_width", tackle_width_ );
    print_param( os, "tackle_exponent", tackle_exponent_ );
    print_param( os, "tackle_cycles", tackle_cycles_ );
    print_param( os, "tackle_power_rate", tackle_power_rate_ );
    print_param( os, "freeform_wait_period", freeform_wait_period_ );
    print_param( os, "freeform_send_period", freeform_send_period_ );
    print_param( os, "free_kick_faults", free_kick_faults_ );
    print_param( os, "back_passes", back_passes_ );
    print_param( os, "proper_goal_kicks", proper_goal_kicks_ );
    print_param( os, "stopped_ball_vel", stopped_ball_vel_ );
    print_param( os, "max_goal_kicks", max_goal_kicks_ );
    print_param( os, "auto_mode", auto_mode_ );
    print_param( os, "kick_off_wait", kick_off_wait_ );
    print_param( os, "connect_wait", connect_wait_ );
    print_param( os, "game_over_wait", game_over_wait_ );
    print_param( os, "team_l_start", team_l_start_ );
    print_param( os, "team_r_start", team_r_start_ );
    print_param( os, "keepaway", keepaway_mode_ );
    print_param( os, "keepaway_length", keepaway_length_ );
    print_param( os, "keepaway_width", keepaway_width_ );
    print_param( os, "keepaway_logging", keepaway_logging_ );
    print_param( os, "keepaway_log_dir", keepaway_log_dir_ );
    print_param( os, "keepaway_log_fixed_name", keepaway_log_fixed_name_ );
    print_param( os, "keepaway_log_fixed", keepaway_log_fixed_ );
    print_param( os, "keepaway_log_dated", keepaway_log_dated_ );
    print_param( os, "keepaway_start", keepaway_start_ );
    print_param( os, "nr_normal_halfs", nr_normal_halfs_ );
    print_param( os, "nr_extra_halfs", nr_extra_halfs_ );
    print_param( os, "penalty_shoot_outs", penalty_shoot_outs_ );
    print_param( os, "pen_before_setup_wait", pen_before_setup_wait_ );
    print_param( os, "pen_setup_wait", pen_setup_wait_ );
    print_param( os, "pen_ready_wait", pen_ready_wait_ );
    print_param( os, "pen_taken_wait", pen_taken_wait_ );
    print_param( os, "pen_nr_kicks", pen_nr_kicks_ );
    print_param( os, "pen_max_extra_kicks", pen_max_extra_kicks_ );
    print_param( os, "pen_dist_x", pen_dist_x_ );
    print_param( os, "pen_random_winner", pen_random_winner_ );
    print_param( os, "pen_max_goalie_dist_x", pen_max_goalie_dist_x_ );
    print_param( os, "pen_allow_mult_kicks", pen_allow_mult_kicks_ );
    print_param( os, "pen_coach_moves_players", pen_coach_moves_players_ );
    print_param( os, "ball_stuck_area", ball_stuck_area_ );
    print_param( os, "coach_msg_file", coach_msg_file_ );
    print_param( os, "max_tackle_power", max_tackle_power_ );
    print_param( os, "max_back_tackle_power", max_back_tackle_power_ );
    print_param( os, "player_speed_max_min", player_speed_max_min_ );
    print_param( os, "extra_stamina", extra_stamina_ );
    print_param( os, "synch_see_offset", synch_see_offset_ );
    print_param( os, "max_monitors", max_monitors_ );
    print_param( os, "min_catch_probability", min_catch_probability_ );
    print_param( os, "reliable_catch_area_l", reliable_catch_area_l_ );
    os << ')';

    return os;
}

}
}
