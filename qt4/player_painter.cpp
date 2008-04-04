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

#include "main_data.h"
#include "options.h"
#include "vector_2d.h"

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
PlayerPainter::Param::Param(  const rcss::rcg::PlayerT & player,
                              const rcss::rcg::BallT & ball,
                             const rcss::rcg::ServerParamT & sparam,
                             const rcss::rcg::PlayerTypeT & ptype )
    : x_( Options::instance().screenX( player.x_ ) )
    , y_( Options::instance().screenY( player.y_ ) )
    , body_radius_( Options::instance().scale( ptype.player_size_ ) )
    , kick_radius_( Options::instance().scale( ptype.player_size_ + ptype.kickable_margin_ + sparam.ball_size_ ) )
    , have_full_effort_( std::fabs( player.effort_ - ptype.effort_max_ ) < 0.0001 )
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
PlayerPainter::PlayerPainter( const MainData & main_data )
    : M_main_data( main_data )
    , M_player_font( "6x13bold", 9, QFont::Bold )
    , M_player_pen( QColor( 0, 0, 0 ), 0, Qt::SolidLine )
    , M_left_team_pen( QColor( 255, 255, 0 ), 0, Qt::SolidLine )
    , M_left_team_brush( QColor( 255, 255, 0 ), Qt::SolidPattern )
    , M_left_goalie_pen( QColor( 0, 255, 0 ), 0, Qt::SolidLine )
    , M_left_goalie_brush( QColor( 0, 255, 0 ), Qt::SolidPattern )
    , M_right_team_pen( QColor( 0, 255, 255 ), 0, Qt::SolidLine )
    , M_right_team_brush( QColor( 0, 255, 255 ), Qt::SolidPattern )
    , M_right_goalie_pen( QColor( 255, 153, 255 ), 0, Qt::SolidLine )
    , M_right_goalie_brush( QColor( 255, 153, 255 ), Qt::SolidPattern )
    , M_player_number_pen( QColor( 255, 255, 255 ), 0, Qt::SolidLine )
    , M_player_stamina_pen( QColor( 15, 255, 141 ), 0, Qt::SolidLine )
    , M_neck_pen( QColor( 255, 0, 0 ), 0, Qt::SolidLine )
    , M_view_area_pen( QColor( 191, 239, 191 ), 0, Qt::SolidLine )
    , M_large_view_area_pen( QColor( 255, 255, 255 ), 0, Qt::SolidLine )
    , M_ball_collide_brush( QColor( 255, 0, 0 ), Qt::SolidPattern )
    , M_player_collide_brush( QColor( 105, 185, 255 ), Qt::SolidPattern )
    , M_kick_pen( QColor( 255, 255, 255 ), 2, Qt::SolidLine )
    , M_kick_fault_brush( QColor( 255, 0, 0 ), Qt::SolidPattern )
    , M_catch_brush( QColor( 10, 80, 10 ), Qt::SolidPattern )
    , M_catch_fault_brush( QColor( 10, 80, 150 ), Qt::SolidPattern )
    , M_tackle_pen( QColor( 255, 136, 127 ), 2, Qt::SolidLine )
    , M_tackle_brush( QColor( 255, 136, 127 ), Qt::SolidPattern )
    , M_tackle_fault_brush( QColor( 79, 159, 159 ), Qt::SolidPattern )
    , M_pointto_pen( QColor( 255, 0, 191 ), 2, Qt::SolidLine )
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
    if ( val.isValid() )
    {
        M_left_goalie_pen.setColor( val.toString() );
        M_left_goalie_brush.setColor( val.toString() );
    }

    val = settings.value( "right_team" );
    if ( val.isValid() )
    {
        M_right_team_pen.setColor( val.toString() );
        M_right_team_brush.setColor( val.toString() );
    }

    val = settings.value( "right_goalie" );
    if ( val.isValid() )
    {
        M_right_goalie_pen.setColor( val.toString() );
        M_right_goalie_brush.setColor( val.toString() );
    }

    val = settings.value( "player_number_pen" );
    if ( val.isValid() ) M_player_number_pen.setColor( val.toString() );

    val = settings.value( "player_stamina_pen" );
    if ( val.isValid() ) M_player_stamina_pen.setColor( val.toString() );

    val = settings.value( "neck_pen" );
    if ( val.isValid() ) M_neck_pen.setColor( val.toString() );

    val = settings.value( "view_area_pen" );
    if ( val.isValid() ) M_view_area_pen.setColor( val.toString() );

    val = settings.value( "large_view_area_pen" );
    if ( val.isValid() ) M_large_view_area_pen.setColor( val.toString() );

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
    settings.setValue( "large_view_area_pen", M_large_view_area_pen.color().name() );
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

    DispConstPtr disp = M_main_data.getDispInfo( M_main_data.index() );

    if ( ! disp )
    {
        return;
    }

    const rcss::rcg::BallT & ball = disp->show_.ball_;

    for ( int i = 0; i < rcss::rcg::MAX_PLAYER*2; ++i )
    {
        drawAll( painter, disp->show_.player_[i], ball );
    }

    if ( Options::instance().showOffsideLine() )
    {
        drawOffsideLine( painter, disp->show_ );
    }
}

/*-------------------------------------------------------------------*/
/*

*/
void
PlayerPainter::drawAll( QPainter & painter,
                        const rcss::rcg::PlayerT & player,
                        const rcss::rcg::BallT & ball ) const
{
    const Param param( player,
                       ball,
                       M_main_data.serverParam(),
                       M_main_data.playerType( player.type_ ) );

    drawBody( painter, param );
    drawDir( painter, param );

    if ( player.hasNeck()
         && player.hasView()
         && Options::instance().showViewArea() )
    {
        drawViewArea( painter, param );
    }

    if ( Options::instance().showControlArea() )
    {
        drawControlArea( painter, param );
    }

    if ( Options::instance().showPointto()
         && player.isPointing() )
    {
        drawPointto( painter, param );
    }

    if ( Options::instance().showPlayerTrace()
         && Options::instance().isSelectedPlayer( param.player_.side(),
                                                  param.player_.unum_ ) )
    {
        drawTrace( painter, param );
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

    switch ( param.player_.side_ ) {
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

    // draw stamina status if effort or recovery is decayed.
    if ( param.player_.hasStamina() )
    {
//         double stamina_rate = param.player_.stamina_ / M_main_data.serverParam().stamina_max_;
//         int dark_rate = 200 - static_cast< int >( rint( 200 * rint( stamina_rate * 8.0 ) / 8.0 ) );

//         painter.setPen( Qt::NoPen );
//         painter.setBrush( painter.brush().color().darker( 100 + dark_rate ) );
//         painter.drawEllipse( param.x_ - param.body_radius_,
//                              param.y_ - param.body_radius_,
//                              param.body_radius_ * 2 ,
//                              param.body_radius_ * 2  );

        if ( param.player_.stamina_ < 1500.0f )
        {
            painter.setPen( Qt::NoPen );
            painter.setBrush( Qt::black );
            painter.drawEllipse( param.x_ - param.body_radius_,
                                 param.y_ - param.body_radius_,
                                 param.body_radius_ * 2 ,
                                 param.body_radius_ * 2  );
        }
    }

    // draw real body edge
    painter.setPen( M_player_pen );
    painter.setBrush( Qt::NoBrush );
    painter.drawEllipse( param.x_ - param.body_radius_,
                         param.y_ - param.body_radius_,
                         param.body_radius_ * 2 ,
                         param.body_radius_ * 2  );

}

/*-------------------------------------------------------------------*/
/*!

*/
void
PlayerPainter::drawDir( QPainter & painter,
                        const PlayerPainter::Param & param ) const
{
    // body direction line
    double real_r
        = param.player_type_.player_size_
        + param.player_type_.kickable_margin_
        + M_main_data.serverParam().ball_size_;
    double body = param.player_.body_ * DEG2RAD;

    int bx = Options::instance().screenX( param.player_.x_ + real_r * std::cos( body ) );
    int by = Options::instance().screenY( param.player_.y_ + real_r * std::sin( body ) );

    painter.setPen( M_player_pen );
    painter.setBrush( Qt::NoBrush );
    painter.drawLine( param.x_, param.y_, bx, by );

    if ( param.player_.hasNeck()
         && ! Options::instance().showViewArea() )
    {
        double head = param.player_.body_ + param.player_.neck_;
        head *= DEG2RAD;

        int nx = Options::instance().screenX( param.player_.x_ + real_r * std::cos( head ) );
        int ny = Options::instance().screenY( param.player_.y_ + real_r * std::sin( head ) );

        painter.setPen( M_neck_pen );
        painter.setBrush( Qt::NoBrush );
        painter.drawLine( param.x_, param.y_, nx, ny );
    }

}

/*-------------------------------------------------------------------*/
/*!

*/
void
PlayerPainter::drawViewArea( QPainter & painter,
                             const PlayerPainter::Param & param ) const
{
    const Options & opt = Options::instance();

    const int visible_radius = opt.scale( M_main_data.serverParam().visible_distance_ );
    const double head = param.player_.body_ + param.player_.neck_;
    const int view_start_angle = static_cast< int >( rint( ( -head - param.player_.view_width_ * 0.5 ) * 16 ) );
    const int span_angle = static_cast< int >( rint( param.player_.view_width_ * 16 ) );


    if ( opt.isSelectedPlayer( param.player_.side(),
                               param.player_.unum_ ) )
    {
        // draw large view area
        const int UNUM_FAR = opt.scale( 20.0 );
        const int TEAM_FAR = opt.scale( 40.0 );
        const int TEAM_TOOFAR = opt.scale( 60.0 );

        painter.setPen( M_large_view_area_pen );
        painter.setBrush( Qt::NoBrush );

        painter.drawArc( param.x_ - UNUM_FAR, // left x
                         param.y_ - UNUM_FAR, // toop y
                         UNUM_FAR * 2, // width
                         UNUM_FAR * 2, // height
                         view_start_angle,
                         span_angle );
        painter.drawArc( param.x_ - TEAM_FAR, // left x
                         param.y_ - TEAM_FAR, // toop y
                         TEAM_FAR * 2, // width
                         TEAM_FAR * 2, // height
                         view_start_angle,
                         span_angle );
        // pie, no an arc
        painter.drawArc( param.x_ - TEAM_TOOFAR, // left x
                         param.y_ - TEAM_TOOFAR, // toop y
                         TEAM_TOOFAR * 2, // width
                         TEAM_TOOFAR * 2, // height
                         view_start_angle,
                         span_angle );

        // draw feeling area circle
        painter.drawArc( param.x_ - visible_radius,
                         param.y_ - visible_radius,
                         visible_radius * 2,
                         visible_radius * 2,
                         0,
                         360 * 16 );

        const double view_start_angle_real = ( head - param.player_.view_width_ * 0.5 ) * DEG2RAD;
        const double view_end_angle_real = ( head + param.player_.view_width_ * 0.5 ) * DEG2RAD;;
        // left side view cone end point x
        int lx = param.x_
            + opt.scale( 60.0 * std::cos( view_start_angle_real ) );
        // left side view cone end point y
        int ly = param.y_
            + opt.scale( 60.0  * std::sin( view_start_angle_real ) );
        // right side view cone end point x
        int rx = param.x_
            + opt.scale( 60.0 * std::cos( view_end_angle_real ) );
        // right side view cone end point y
        int ry = param.y_
            + opt.scale( 60.0 * std::sin( view_end_angle_real ) );
        painter.drawLine( lx, ly, param.x_, param.y_ );
        painter.drawLine( rx, ry, param.x_, param.y_ );

    }
    else
    {
        // draw small view area
        painter.setPen( M_view_area_pen );
        painter.setBrush( Qt::NoBrush );

        painter.drawPie( param.x_ - visible_radius,
                         param.y_ - visible_radius,
                         visible_radius * 2,
                         visible_radius * 2,
                         view_start_angle,
                         span_angle );
    }
}

/*-------------------------------------------------------------------*/
/*!

*/
void
PlayerPainter::drawControlArea( QPainter & painter,
                                const PlayerPainter::Param & param ) const
{
    const Options & opt = Options::instance();
    const rcss::rcg::ServerParamT & sparam = M_main_data.serverParam();

    if ( param.player_.isGoalie() )
    {
        double catchable_area
            = std::sqrt( std::pow( sparam.catchable_area_w_ * 0.5, 2.0 )
                         + std::pow( sparam.catchable_area_l_, 2.0 ) );

        int catchable = opt.scale( catchable_area );
        painter.setPen( ( param.player_.side_ == 'l' )
                        ? M_left_goalie_pen
                        : M_right_goalie_pen );
        painter.setBrush( Qt::NoBrush );

        painter.drawEllipse( param.x_ - catchable,
                             param.y_ - catchable,
                             catchable * 2,
                             catchable * 2 );
    }

    Vector2D ppos( param.player_.x_,
                   param.player_.y_ );
    Vector2D bpos( param.ball_.x_,
                   param.ball_.y_ );

    Vector2D player_to_ball = bpos - ppos;
    player_to_ball.rotate( - param.player_.body_ );

    // draw tackle area & probability
    double tackle_dist = ( player_to_ball.x > 0.0
                           ? sparam.tackle_dist_
                           : sparam.tackle_back_dist_ );
    double tackle_prob = 1.0;
    if ( tackle_dist > 0.0 )
    {
        tackle_prob = ( std::pow( std::fabs( player_to_ball.x ) / tackle_dist,
                                  sparam.tackle_exponent_ )
                        + std::pow( player_to_ball.absY() / sparam.tackle_width_,
                                    sparam.tackle_exponent_ ) );
    }

    if ( tackle_prob < 1.0 )
    {
#if 0
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
        pts[0].setX( opt.screenX( ppos.x + forward_x + right_x ) );
        pts[0].setY( opt.screenY( ppos.y + forward_y + right_y ) );
        pts[1].setX( opt.screenX( ppos.x + forward_x - right_x ) );
        pts[1].setY( opt.screenY( ppos.y + forward_y - right_y ) );
        pts[2].setX( opt.screenX( ppos.x + back_x - right_x ) );
        pts[2].setY( opt.screenY( ppos.y + back_y - right_y ) );
        pts[3].setX( opt.screenX( ppos.x + back_x + right_x ) );
        pts[3].setY( opt.screenY( ppos.y + back_y + right_y ) );
        pts[4] = pts[0];

        painter.setPen( M_tackle_pen );
        painter.setBrush( Qt::NoBrush );

        //painter.drawConvexPolygon( pts, 4 );
        painter.drawPolyline( pts, 5 );
#else
        painter.save();
        painter.translate( param.x_, param.y_ );
        painter.rotate( param.player_.body_ );

        painter.setPen( M_tackle_pen );
        painter.setBrush( Qt::NoBrush );

        painter.drawRect( opt.scale( - sparam.tackle_back_dist_ ),
                          opt.scale( - sparam.tackle_width_ ),
                          opt.scale( sparam.tackle_dist_ + sparam.tackle_back_dist_ ),
                          opt.scale( sparam.tackle_width_ * 2.0 ) );


        painter.restore();
#endif


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
PlayerPainter::drawPointto( QPainter & painter,
                            const PlayerPainter::Param & param ) const
{
    int ix = Options::instance().screenX( param.player_.point_x_ );
    int iy = Options::instance().screenY( param.player_.point_y_ );

    painter.setPen( M_pointto_pen );
    painter.setBrush( Qt::NoBrush );
    painter.drawLine( param.x_, param.y_,
                      ix, iy );
}


/*-------------------------------------------------------------------*/
/*!

*/
void
PlayerPainter::drawTrace( QPainter & painter,
                          const PlayerPainter::Param & param ) const
{
    const Options & opt = Options::instance();

    const std::size_t first = M_main_data.dispHolder().getIndexOf( opt.playerTraceStart() );
    const std::size_t last = M_main_data.dispHolder().getIndexOf( opt.playerTraceEnd() );
    if ( first >= last )
    {
        return;
    }

    const std::vector< DispPtr > & cont = M_main_data.dispHolder().dispInfoCont();
    if ( cont.empty() )
    {
        return;
    }

    if ( opt.antiAliasing() )
    {
        painter.setRenderHint( QPainter::Antialiasing, false );
    }

    const bool line_trace = opt.lineTrace();

    const std::size_t idx = static_cast< std::size_t >( opt.selectedNumber() > 0
                                                        ? opt.selectedNumber() - 1
                                                        : 11 - opt.selectedNumber() - 1 );

    const QPen & pen = ( opt.selectedNumber() > 0
                         ? M_left_team_pen
                         : M_right_team_pen );
    QPen black_dot_pen( Qt::black );
    black_dot_pen.setStyle( Qt::DotLine );

    painter.setBrush( Qt::NoBrush );

    std::size_t i = first;
    int prev_x = opt.screenX( cont[i]->show_.player_[idx].x_ );
    int prev_y = opt.screenY( cont[i]->show_.player_[idx].y_ );
    ++i;
    for ( ; i <= last; ++i )
    {
        switch ( cont[i]->pmode_ ) {
        case rcss::rcg::PM_BeforeKickOff:
        case rcss::rcg::PM_TimeOver:
        case rcss::rcg::PM_AfterGoal_Left:
        case rcss::rcg::PM_AfterGoal_Right:
            continue;
        case rcss::rcg::PM_PlayOn:
            painter.setPen( pen );
            break;
        default:
            painter.setPen( black_dot_pen );
            break;
        }

        int ix = opt.screenX( cont[i]->show_.player_[idx].x_ );
        int iy = opt.screenY( cont[i]->show_.player_[idx].y_ );

        painter.drawLine( prev_x, prev_y, ix, iy );
        if ( ! line_trace )
        {
            painter.drawEllipse( ix - 2, iy - 2, 5, 5 );
        }
        prev_x = ix;
        prev_y = iy;
    }

    if ( opt.antiAliasing() )
    {
        painter.setRenderHint( QPainter::Antialiasing );
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
        std::snprintf( buf, 8, "%d", param.player_.unum_ );
        std::strcat( main_buf, buf );
    }

    if ( param.player_.hasStamina()
         && Options::instance().showStamina() )
    {
        char buf[16];
        std::snprintf( buf, 16, "%4.0f", param.player_.stamina_ );
        if ( main_buf[0] != '\0' ) std::strcat( main_buf, " " );
        std::strcat( main_buf, buf );
    }

    if ( Options::instance().showPlayerType() )
    {
        char buf[8];
        std::snprintf( buf, 8, "t%d", param.player_.type_ );
        if ( main_buf[0] != '\0' ) std::strcat( main_buf, ", " );
        strcat( main_buf, buf );
    }

    if ( main_buf[0] != '\0' )
    {
        const int text_radius = std::min( 40, param.draw_radius_ );

        painter.setFont( M_player_font );

        //painter.setPen( param.player_.side() == rcss::rcg::LEFT
        //                ? M_left_team_pen
        //                : M_right_team_pen );
        painter.setPen( M_player_number_pen );
        painter.setBrush( Qt::NoBrush );
        painter.drawText( param.x_ + text_radius,
                          param.y_,
                          QString::fromAscii( main_buf ) );
    }
}

/*-------------------------------------------------------------------*/
/*!

*/
void
PlayerPainter::drawOffsideLine( QPainter & painter,
                                const rcss::rcg::ShowInfoT & show ) const
{
    const Options & opt = Options::instance();

    const float ball_x = show.ball_.x_;

    float offside_l = 0.0;
    {
        float min_x = 0.0f;
        for ( int i = 0; i < rcss::rcg::MAX_PLAYER*2; ++i )
        {
            if ( show.player_[i].state_ != 0
                 && show.player_[i].side_ == 'l' )
            {
                float x = show.player_[i].x_;
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
        offside_l = std::max( offside_l, - float( Options::PITCH_HALF_LENGTH ) );
    }

    float offside_r = 0.0;
    {
        float max_x = 0.0;
        for ( int i = 0; i < rcss::rcg::MAX_PLAYER*2; ++i )
        {
            if ( show.player_[i].state_ != 0
                 && show.player_[i].side_ == 'r' )
            {
                float x = show.player_[i].x_;
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
        offside_r = std::min( offside_r, float( Options::PITCH_HALF_LENGTH ) );
    }

    const int offside_line_l = opt.screenX( offside_l );
    const int offside_line_r = opt.screenX( offside_r );

    const int top_y = opt.screenY( Options::PITCH_HALF_WIDTH );
    const int bottom_y = opt.screenY( - Options::PITCH_HALF_WIDTH );

    painter.setBrush( Qt::NoBrush );

    painter.setPen( M_left_team_pen );
    painter.drawLine( offside_line_l, top_y,
                      offside_line_l, bottom_y );

    painter.setPen( M_right_team_pen );
    painter.drawLine( offside_line_r, top_y,
                      offside_line_r, bottom_y );
}
