// -*-c++-*-

/*!
  \file plaeyr_painter.cpp
  \brief player painter class Source File.
*/

/*
 *Copyright:

 Copyright (C) The RoboCup Soccer Server Maintenance Group.
 Hidehisa Akiyama

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

#include <QtGui>

#include "player_painter.h"

#include "options.h"
#include "main_data.h"

#include <rcsslogplayer/types.h>

#include <cstring>
#include <cstdio>
#include <cmath>

namespace {
const double DEG2RAD = M_PI / 180.0;
}

/*-------------------------------------------------------------------*/
/*

*/
inline
PlayerPainter::Param::Param( const Player & player,
                             const Ball & ball,
                             const FieldCanvas & canvas,
                             const rcss::rcg::ServerParamT & sparam,
                             const rcss::rcg::PlayerTypeT & ptype )
    : x_( canvas.screenX( player.x() ) )
    , y_( canvas.screenY( player.y() ) )
    , body_( player.body() )
    , head_( player.head() )
    , body_radius_( canvas.scale( ptype.player_size_ ) )
    , kick_radius_( canvas.scale( ptype.player_size_ + ptype.kickable_margin_ + sparam.ball_size_ ) )
    , have_full_effort_( player.hasFullEffort( ptype.effort_max_ ) )
    , player_( player )
    , ball_( ball )
    , player_type_( ptype )
{
    if ( body_radius_ < 1 ) body_radius_ = 1;
    if ( kick_radius_ < 5 ) kick_radius_ = 5;

    draw_radius_ =  kick_radius_;
}

/*-------------------------------------------------------------------*/
/*

*/
PlayerPainter::PlayerPainter( const FieldCanvas & canvas;
                              const MainData & main_data )
    : M_canvas( canvas )
    , M_main_data( main_data )
    , M_player_font( "6x13bold", 9, QFont::Bold )
    , M_player_pen( QColor( 0, 0, 0 ), 0, Qt::SolidLine )
    , M_left_team_pen( QColor( 255, 0, 0 ), 0, Qt::SolidLine )
    , M_left_team_brush( QColor( 255, 255, 0 ), Qt::SolidPattern )
    , M_left_goalei_brush( QColor( 0, 255, 0 ), Qt::SolidPattern )
    , M_right_team_pen( QColor( 0, 0, 139 ), 0, Qt::SolidLine )
    , M_right_team_brush( QColor( 0, 255, 255 ), Qt::SolidPattern )
    , M_right_goalie_brush( QColor( 255, 153, 255 ), Qt::SolidPattern )
    , M_player_number_pen( QColor( 255, 255, 255 ), 0, Qt::SolidLine )
    , M_player_stamina_pen( QColor( 15, 255, 141 ), 0, Qt::SolidLine )
    , M_neck_pen( QColor( 55, 255, 255 ), 0, Qt::SolidLine )
    , M_view_area_pen( QColor( 55, 255, 255 ), 0, Qt::SolidLine )
    , M_ball_collide_brush( QColor( 255, 0, 0 ), Qt::SolidPattern )
    , M_player_collide_brush( QColor( 105, 185, 255 ), Qt::SolidPattern )
    , M_player_collide_brush( QColor( 105, 185, 255 ), Qt::SolidPattern )
    , M_kick_pen( QColor( 255, 255, 255 ), 2, Qt::SolidLine )
    , M_kick_fault_brush( QColor( 255, 0, 0 ), Qt::SolidPattern )
    , M_catch_brush( QColor( 10, 80, 10 ), Qt::SolidPattern )
    , M_catch_fault_brush( QColor( 10, 80, 150 ), Qt::SolidPattern )
    , M_tackle_pen( QColor( 255, 136, 127 ), 2, Qt::SolidPattern )
    , M_tackle_brush( QColor( 255, 136, 127 ), Qt::SolidPattern )
    , M_tackle_fault_brush( QColor( 79, 159, 159 ), Qt::SolidPattern )
{
    M_player_font.setPointSize( 9 );
    M_player_font.setBold( true );
    //M_player_font.setStyleHint( QFont::System, QFont::PreferBitmap );
    M_player_font.setBold( true );
    M_player_font.setFixedPitch( true );

    readSettings();
}

/*-------------------------------------------------------------------*/
/*

*/
PlayerPainter::~PlayerPainter()
{
    writeSettings();
}

/*-------------------------------------------------------------------*/
/*

*/
void
PlayerPainter::readSettings()
{
    QSettings settings( QDir::homePath() + "/.rcsslogplayer",
                        QSettings::IniFormat );

    settings.beginGroup( "PlayerPainter" );

    QVariant val;

    val = settings.value( "player_font" );
    if ( val.isValid() ) M_player_font.fromString( val.toString() );

    val = settings.value( "player_pen" );
    if ( val.isValid() ) M_player_pen.setColor( val.toString() );

    val = settings.value( "left_team" );
    if ( val.isValid() )
    {
        M_left_team_pen.setColor( val.toString() );
        M_left_team_brush.setColor( val.toString() );
    }

    val = settings.value( "left_goalie" );
    if ( val.isValid() ) M_left_goalie_brush.setColor( val.toString() );

    val = settings.value( "right_team" );
    if ( val.isValid() )
    {
        M_right_team_pen.setColor( val.toString() );
        M_right_team_brush.setColor( val.toString() );
    }

    val = settings.value( "right_goalie" );
    if ( val.isValid() ) M_right_goalie_brush.setColor( val.toString() );

    val = settings.value( "player_number_pen" );
    if ( val.isValid() ) M_player_number_pen.setColor( val.toString() );

    val = settings.value( "player_stamina_pen" );
    if ( val.isValid() ) M_player_stamina_pen.setColor( val.toString() );

    val = settings.value( "neck_pen" );
    if ( val.isValid() ) M_neck_pen.setColor( val.toString() );

    val = settings.value( "view_area_pen" );
    if ( val.isValid() ) M_view_area_pen.setColor( val.toString() );

    val = settings.value( "ball_collide_brush" );
    if ( val.isValid() ) M_ball_collide_brush.setColor( val.toString() );

    val = settings.value( "kick_pen" );
    if ( val.isValid() ) M_kick_pen.setColor( val.toString() );

    val = settings.value( "kick_fault_brush" );
    if ( val.isValid() ) M_kick_fault_brush.setColor( val.toString() );

    val = settings.value( "catch_brush" );
    if ( val.isValid() ) M_catch_brush.setColor( val.toString() );

    val = settings.value( "catch_fault_brush" );
    if ( val.isValid() ) M_catch_fault_brush.setColor( val.toString() );

    val = settings.value( "tackle_pen" );
    if ( val.isValid() ) M_tackle_pen.setColor( val.toString() );

    val = settings.value( "tackle_brush" );
    if ( val.isValid() ) M_tackle_brush.setColor( val.toString() );

    val = settings.value( "tackle_fault_brush" );
    if ( val.isValid() ) M_tackle_fault_brush.setColor( val.toString() );

    settings.endGroup();
}

/*-------------------------------------------------------------------*/
/*

*/
void
PlayerPainter::writeSettings()
{
    QSettings settings( QDir::homePath() + "/.rcsslogplayer",
                        QSettings::IniFormat );

    settings.beginGroup( "PlayerPainter" );

    settings.setValue( "player_font", M_player_font.toString() );

    settings.setValue( "player_pen", M_player_pen.color().name() );
    settings.setValue( "left_team", M_left_team_pen.color().name() );
    settings.setValue( "left_goalie", M_left_goalie_brush.color().name() );
    settings.setValue( "right_team", M_right_team_pen.color().name() );
    settings.setValue( "right_goalie", M_right_goalie_brush.color().name() );

    settings.setValue( "player_number_pen", M_player_number_pen.color().name() );
    settings.setValue( "player_stamina_pen", M_player_stamina_pen.color().name() );

    settings.setValue( "neck_pen", M_neck_pen.color().name() );
    settings.setValue( "view_area_pen", M_view_area_pen.color().name() );
    settings.setValue( "ball_collide_brush", M_ball_collide_brush.color().name() );
    settings.setValue( "kick_pen", M_kick_pen.color().name() );
    settings.setValue( "kick_fault_brush", M_kick_fault_brush.color().name() );
    settings.setValue( "catch_brush", M_catch_brush.color().name() );
    settings.setValue( "catch_fault_brush", M_catch_fault_brush.color().name() );
    settings.setValue( "tackle_pen", M_tackle_pen.color().name() );
    settings.setValue( "tackle_brush", M_tackle_brush.color().name() );
    settings.setValue( "tackle_fault_brush", M_tackle_fault_brush.color().name() );

    settings.endGroup();
}

/*-------------------------------------------------------------------*/
/*

*/
void
PlayerPainter::draw( QPainter & painter )
{
    if ( ! Options::instance().showPlayer() )
    {
        return;
    }

    MonitorViewConstPtr view = M_main_data.getViewData( M_main_data.viewIndex() );

    if ( ! view )
    {
        return;
    }

    const Ball & ball = view->ball();

    const std::vector< Player >::const_iterator end = view->players().end();
    for ( std::vector< Player >::const_iterator it = view->players().begin();
          it != end;
          ++it )
    {
        drawAll( painter, *it, ball );
    }

    if ( Options::instance().showOffsideLine() )
    {
        drawOffsideLine( painter, ball, view->players() );
    }
}

/*-------------------------------------------------------------------*/
/*

*/
void
PlayerPainter::drawAll( QPainter & painter,
                        const Player & player,
                        const Ball & ball ) const
{
    const Param param( player,
                       ball,
                       M_canvas,
                       M_main_data.serverParam(),
                       M_main_data.playerType( player.type() ) );

    drawBody( painter, param );
    drawEdge( painter, param );

//     if ( M_main_data.viewConfig().isSelectedPlayer( player.side(), player.unum() )
//          && M_main_data.viewConfig().playerFutureCycle() > 0
//          && player.hasDelta() )
//     {
//         drawFuture( painter, param );
//     }

    if ( player.hasView()
         && Options::instance().showViewArea() )
    {
        drawViewCone( painter, param );
    }

    if ( Options::instance().showContorolArea() )
    {
        drawControlArea( painter, param );
    }

    drawText( painter, param );
}

/*-------------------------------------------------------------------*/
/*

*/
void
PlayerPainter::drawBody( QPainter & painter,
                             const PlayerPainter::Param & param ) const
{
    // decide base color
    painter.setPen( M_player_pen );

    switch ( param.player_.side() ) {
    case 'l':
        if ( param.player_.isGoalie() )
        {
            painter.setBrush( M_left_goalie_brush );
        }
        else
        {
            painter.setBrush( M_left_team_brush );
        }
        break;
    case 'r':
        if ( param.player_.isGoalie() )
        {
            painter.setBrush( M_right_goalie_brush );
        }
        else
        {
            painter.setBrush( M_right_team_brush );
        }
        break;
    case 'n':
        painter.setBrush( Qt::black );
        break;
    default:
        painter.setBrush( Qt::black );
        break;
    }


    // decide status color
    if ( ! param.player_.isAlive() )
    {
        painter.setBrush( Qt::black );
    }
    if ( param.player_.isKicking() )
    {
        painter.setPen( M_kick_pen );
    }
    if ( param.player_.isKickingFault() )
    {
        painter.setBrush( M_kick_fault_brush );
    }
    if ( param.player_.isCatching() )
    {
        painter.setBrush( M_catch_brush );
    }
    if ( param.player_.isCatchingFault() )
    {
        painter.setBrush( M_catch_fault_brush );
    }
    if ( param.player_.isTackling() )
    {
        painter.setPen( M_tackle_pen );
        painter.setBrush( M_tackle_brush );
    }
    if ( param.player_.isTacklingFault() )
    {
        painter.setPen( M_tackle_pen );
        painter.setBrush( M_tackle_fault_brush );
    }
    if ( param.player_.isCollidedBall() )
    {
        painter.setBrush( M_ball_collide_brush );
    }
    if ( param.player_.isCollidedPlayer() )
    {
        painter.setBrush( M_player_collide_brush );
    }

    painter.drawEllipse( param.x_ - param.kick_radius_ ,
                         param.y_ - param.kick_radius_ ,
                         param.kick_radius_ * 2 ,
                         param.kick_radius_ * 2 );

}

/*-------------------------------------------------------------------*/
/*!

*/
void
PlayerPainter::drawEdge( QPainter & painter,
                         const PlayerPainter::Param & param ) const
{

    // draw real body edge
    painter.setPen( M_player_pen );
    painter.setBrush( Qt::NoBrush );
    painter.drawEllipse( param.x_ - param.body_radius_,
                         param.y_ - param.body_radius_,
                         param.body_radius_ * 2 ,
                         param.body_radius_ * 2  );

    // draw stamina status if effort or recovery is decayed.
    if ( param.player_.hasStamina() )
    {
        if ( param.player_.stamina() < 1500.0 )
        {
            painter.setBrush( Qt::black );
            painter.drawEllipse( param.x_ - param.body_radius_,
                                 param.y_ - param.body_radius_,
                                 param.body_radius_ * 2 ,
                                 param.body_radius_ * 2  );
        }
    }

    // body direction line
    {
        double kickable_area
            = param.player_type_.player_size_
            + param.player_type_.kickable_margin_
            + M_main_data.serverParam().ball_size_;

        int end_x = vconf.screenX( param.player_.x()
                                   + kickable_area
                                   * std::cos( param.body_ * DEG2RAD ) );
        int end_y = vconf.screenY( param.player_.y()
                                   + kickable_area
                                   * std::sin( param.body_ * DEG2RAD ) );

        painter.setPen( M_player_pen );
        painter.setBrush( Qt::NoBrush );
        painter.drawLine( param.x_, param.y_, end_x, end_y );

    }
}

/*-------------------------------------------------------------------*/
/*!

*/
void
PlayerPainter::drawViewCone( QPainter & painter,
                             const PlayerPainter::Param & param ) const
{
    const double view_width = param.player_.viewWidth();
    const double visible_dist = M_main_data.serverParam().visible_dstance_;
    const int visible_radius = M_canvas.scale( visible_dist );

    const double view_start_angle = -param.head_ - view_width * 0.5;
    const double view_start_angle_real = ( param.head_ - view_width * 0.5 ) * DEG2RAD;
    const double view_end_angle_real = ( param.head_ + view_width * 0.5 ) * DEG2RAD;


    // draw face direction
    painter.setPen( M_neck_pen );
    painter.setBrush( Qt::NoBrush );

    QPainterPath view_cone_path;
    view_cone_path.moveTo( param.x_, param.y_ );
    view_cone_path.arcTo( param.x_ - visible_radius,
                          param.y_ - visible_radius,
                          visible_radius * 2,
                          visible_radius * 2,
                          view_start_angle,
                          view_width );
    view_cone_path.closeSubpath();

    painter.drawPath( view_cone_path );
}

/*-------------------------------------------------------------------*/
/*!

*/
void
PlayerPainter::drawControlArea( QPainter & painter,
                                    const PlayerPainter::Param & param ) const
{
    const rcss::rcg::ServerParamT & sparam = M_main_data.serverParam();

    if ( param.player_.isGoalie() )
    {
        double catchable_area
            = std::sqrt( std::pow( sparam.catchable_area_w_ * 0.5, 2.0 )
                         + std::pow( sparam.catchable_area_l_, 2.0 ) );

        int catchable = M_canvas.scale( catchable_area );
        painter.setPen( ( param.player_.side() == 'l' )
                        ? M_left_goalie_pen
                        : M_right_goalie_pen );
        painter.setBrush( Qt::NoBrush );

        painter.drawEllipse( param.x_ - catchable,
                             param.y_ - catchable,
                             catchable * 2,
                             catchable * 2 );
    }

    QPointF ppos( param.player_.x(),
                  param.player_.y() );
    QPointF bpos( param.ball_.x(),
                  param.ball_.y() );

    QPointF player_to_ball = bpos - ppos;
    double r = std::sqrt( std::pow( player_to_ball.x(), 2.0 )
                          + std::pow( player_to_ball.y(), 2.0 ) );
    double ang = ( player_to_ball.x() == 0.0 && player_to_ball.y() == 0.0
                   ? 0.0
                   : std::atan2( player_to_ball.y(), player_to_ball.x() ) * RAD2DEG );
    ang -= param.body_;
    ang += DEG2RAD;
    player_to_ball.setX( r * std::cos( ang ) );
    player_to_ball.setY( r * std::sin( ang ) );

    // draw tackle area & probability
    double tackle_dist = ( player_to_ball.x() > 0.0
                           ? sparam.tackle_dist_
                           : sparam.tackle_back_dist_ );
    double tackle_prob = 1.0;
    if ( tackle_dist > 0.0 )
    {
        tackle_prob = ( std::pow( std::fabs( player_to_ball.x() ) / tackle_dist,
                                  sparam.tackle_exponent_ )
                        + std::pow( player_to_ball.absY() / sparam.tackle_width_,
                                    sparam.tackle_exponent_ ) );
    }

    if ( tackle_prob < 1.0 )
    {
        double body_angle = param.body_ * DEG2RAD;
        double body_angle_side = ( body_angle + 90.0 ) * DEG2RAD;
        double body_x = std::cos( body_angle );
        double body_y = std::sin( body_angle );
        double forward_x = sparam.tackle_dist_ * body_x;
        double forward_y = sparam.tackle_dist_ * body_y;
        double back_x = sparam.tackle_back_dist_ * -body_x;
        double back_y = sparam.tackle_back_dist_ * -body_y;
        double right_x = sparam.tackle_width_ * std::cos( body_angle_side );
        double right_y = sparam.tackle_width_ * std::sin( body_angle_side );

        QPoint pts[5];
        pts[0].setX( M_canvas.screenX( ppos.x() + forward_x + right_x ) );
        pts[0].setY( M_canvas.screenY( ppos.y() + forward_y + right_y ) );
        pts[1].setX( M_canvas.screenX( ppos.x() + forward_x - right_x ) );
        pts[1].setY( M_canvas.screenY( ppos.y() + forward_y - right_y ) );
        pts[2].setX( M_canvas.screenX( ppos.x() + back_x - right_x ) );
        pts[2].setY( M_canvas.screenY( ppos.y() + back_y - right_y ) );
        pts[3].setX( M_canvas.screenX( ppos.x() + back_x + right_x ) );
        pts[3].setY( M_canvas.screenY( ppos.y() + back_y + right_y ) );
        pts[4] = pts[0];

        painter.setPen( M_tackle_area_pen );
        painter.setBrush( Qt::NoBrush );

        //painter.drawConvexPolygon( pts, 4 );
        painter.drawPolyline( pts, 5 );

        int text_radius = param.draw_radius_;
        if ( text_radius > 40 )
        {
            text_radius = 40;
        }

        painter.setFont( M_player_font );
        painter.setPen( M_tackle_pen );

        char msg[32];
        std::snprintf( msg, 32, "TackleProb=%.3f", 1.0 - tackle_prob );
        painter.drawText( param.x_ + text_radius,
                          param.y_ + 4 + painter.fontMetrics().ascent(),
                          QString::fromAscii( msg ) );
    }
}

/*-------------------------------------------------------------------*/
/*!

*/
void
PlayerPainter::drawText( QPainter & painter,
                         const PlayerPainter::Param & param ) const
{
    char main_buf[64];
    std::memset( main_buf, 0, 64 );

    if ( Options::instance().showPlayerNumber() )
    {
        char buf[8];
        std::snprintf( buf, 8, "%X", param.player_.unum() );
        std::strcat( main_buf, buf );
    }

    if ( param.player_.hasStamina()
         && Options::instance().showStamina() )
    {
        char buf[16];
        std::snprintf( buf, 16, "%4.0f", param.player_.stamina() );
        if ( main_buf[0] != '\0' ) std::strcat( main_buf, " " );
        std::strcat( main_buf, buf );
    }

    if ( Options::instance().showPlayerType() )
    {
        char buf[8];
        std::snprintf( buf, 8, "t%d", param.player_.type() );
        if ( main_buf[0] != '\0' ) std::strcat( main_buf, ", " );
        strcat( main_buf, buf );
    }

    if ( main_buf[0] != '\0' )
    {
        painter.setFont( M_player_font );

        painter.setPen( param.player_.side() == rcss::rcg::LEFT
                        ? M_left_team_pen
                        : M_right_team_pen );
        painter.setBrush( Qt::NoBrush );
        painter.drawText( param.x_,
                          param.y_,
                          QString::fromAscii( main_buf ) );
    }
}

/*-------------------------------------------------------------------*/
/*!

*/
void
PlayerPainter::drawOffsideLine( QPainter & painter,
                                const Ball & ball,
                                const std::vector< Player > & players )
{


    const double PITCH_HALF_LENGTH = 52.5;
    const double PITCH_HALF_WIDTH = 34.0;

    const double ball_x = view->ball().x();
    const std::vector< Player >::const_iterator end = playrs.end();

    double offside_l = 0.0;
    {
        double min_x = 0.0;
        for ( std::vector< Player >::const_iterator it = players.begin();
              it != end;
              ++it )
        {
            if ( it->isAlive()
                 && it->side() == rcss::rcg::LEFT )
            {
                double x = it->x();
                if ( x < offside_l )
                {
                    if ( x < min_x )
                    {
                        offside_l = min_x;
                        min_x = x;
                    }
                    else
                    {
                        offside_l = x;
                    }
                }
            }
        }
        offside_l = std::min( offside_l, ball_x );
        offside_l = std::max( offside_l, - PITCH_HALF_LENGTH );
    }
    const int offside_line_l = vconf.screenX( offside_l );

    double offside_r = 0.0;
    {
        double max_x = 0.0;
        for ( std::vector< Player >::const_iterator it = players.begin();
              it != end;
              ++it )
        {
            if ( it->isAlive()
                 && it->side() == rcss::rcg::RIGHT )
            {
                double x = it->x();
                if ( offside_r < x )
                {
                    if ( max_x < x )
                    {
                        offside_r = max_x;
                        max_x = x;
                    }
                    else
                    {
                        offside_r = x;
                    }
                }
            }
        }
        offside_r = std::max( offside_r, ball_x );
        offside_r = std::min( offside_r, PITCH_HALF_LENGTH );
    }
    const int offside_line_r = M_canvas.screenX( offside_r );

    const int top_y = M_canvas.screenY( PITCH_HALF_WIDTH );
    const int bottom_y = M_canvas.screenY( - PITCH_HALF_WIDTH );

    painter.setBrush( Qt::NoBrush );

    painter.setPen( M_left_team_pen );
    painter.drawLine( offside_line_l, top_y,
                      offside_line_l, bottom_y );

    painter.setPen( M_right_team_pen );
    painter.drawLine( offside_line_r, top_y,
                      offside_line_r, bottom_y );
}
