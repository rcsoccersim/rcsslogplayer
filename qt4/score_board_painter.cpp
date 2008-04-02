// -*-c++-*-

/*!
  \file score_board_painter.cpp
  \brief score board painter class Source File.
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

#include "score_board_painter.h"

#include "options.h"
#include "main_data.h"
#include "monitor_view_data.h"

#include <iostream>


/*-------------------------------------------------------------------*/
/*!

*/
ScoreBoardPainter::ScoreBoardPainter( const FieldCanvas & canvas,
                                      const MainData & main_data )
    : M_canvas( canvas )
    , M_main_data( main_data )
    , M_brush( QColor( 0, 0, 0 ), Qt::SolidPattern )
    , M_font_pen( QColor( 255, 255, 255 ), 0, Qt::SolidLine )
    , M_font( "6x13bold", 9, QFont::Bold )
{
    M_font.setPointSize( 9 );
    M_font.setBold( true );
    //M_font.setStyleHint( QFont::System, QFont::PreferBitmap );
    M_font.setBold( true );
    M_font.setFixedPitch( true );

    readSettings();
}

/*-------------------------------------------------------------------*/
/*!

*/
ScoreBoardPainter::~ScoreBoardPainter()
{
    writeSettings();
}

/*-------------------------------------------------------------------*/
/*!

 */
void
FieldPainter::readSettings()
{
    QSettings settings( QDir::homePath() + "/.rcsslogplayer",
                        QSettings::IniFormat );

    settings.beginGroup( "ScoreBoardPainter" );

    QVariant val;

    val = settings.value( "score_board_brush" );
    if ( val.isValid() ) M_brush.setColor( val.toString() );

    val = settings.value( "score_board_pen" );
    if ( val.isValid() ) M_font_pen.setColor( val.toString() );

    val = settings.value( "score_board_font" );
    if ( val.isValid() ) M_score_board_font.fromString( val.toString() );

    settings.endGroup();
}

/*-------------------------------------------------------------------*/
/*!

 */
void
FieldPainter::readSettings()
{
    QSettings settings( QDir::homePath() + "/.rcsslogplayer",
                        QSettings::IniFormat );

    settings.beginGroup( "ScoreBoardPainter" );

    settings.setValue( "score_board_brush", M_brush.color().name() );
    settings.setValue( "score_board_pen", M_font_pen.color().name() );
    settings.setValue( "score_board_font", M_font.toString() );

    settings.endGroup();
}

/*-------------------------------------------------------------------*/
/*!

*/
void
ScoreBoardPainterRCSS::draw( QPainter & painter )
{
    const Options & opt = Options::instance();

    if ( ! opt.showScoreBoard() )
    {
        return;
    }

    MonitorViewConstPtr view = M_main_data.getViewData( M_main_data.viewIndex() );

    if ( ! view )
    {
        return;
    }

    const Team & left_team = view->leftTeam();

    const Team & right_team = view->rightTeam();

    const std::vector< std::pair< int, rcss::rcg::PlayMode > > & pen_scores_left
        = M_main_data.viewHolder().penaltyScoresLeft();
    const std::vector< std::pair< int, rcss::rcg::PlayMode > > & pen_scores_right
        = M_main_data.viewHolder().penaltyScoresRight();

    bool show_pen_score = true;

    if ( pen_scores_left.empty()
         && pen_scores_right.empty() )
    {
        show_pen_score = false;
    }
    else if ( ( ! pen_scores_left.empty()
                && view->cycle() < pen_scores_left.front().first )
              && ( ! pen_scores_right.empty()
                   && view->cycle() < pen_scores_right.front().first )
              && view->playmode().mode() != rcss::rcg::::PM_PenaltySetup_Left
              && view->playmode().mode() != rcss::rcg::PM_PenaltySetup_Right
              && view->playmode().mode() != rcss::rcg::PM_PenaltyReady_Left
              && view->playmode().mode() != rcss::rcg::PM_PenaltyReady_Right
              && view->playmode().mode() != rcss::rcg::PM_PenaltyTaken_Left
              && view->playmode().mode() != rcss::rcg::PM_PenaltyTaken_Right )
    {
        show_pen_score = false;
    }


    QString main_buf;

    std::string mode_str = view->playmode().name();

    if ( ! show_pen_score )
    {
        main_buf.sprintf( " %10s %d:%d %-10s %16s %6ld    ",
                          ( left_team.name().empty()
                            || left_team.name() == "null"
                          ? ""
                          : left_team.name().c_str(),
                          left_team.score(),
                          right_team.score(),
                          ( right_team.name().empty()
                            || right_team.name() == "null" )
                          ? ""
                          : right_team.name().c_str(),
                          mode_str.c_str(),
                          view->cycle() );
    }
    else
    {
        int left_pen_score = 0;
        int left_pen_miss = 0;
        int right_pen_score = 0;
        int right_pen_miss = 0;

        for ( std::vector< std::pair< long, rcss::rcg::PlayMode > >::const_iterator it = pen_scores_left.begin();
              it != pen_scores_left.end();
              ++it )
        {
            if ( it->first > view->cycle() ) break;

            if ( it->second == rcss::rcg::PM_PenaltyScore_Left
                 || it->second == rcss::rcg::PM_PenaltyScore_Right )
            {
                ++left_pen_score;
            }
            else if ( it->second == rcss::rcg::PM_PenaltyMiss_Left
                      || it->second == rcss::rcg::PM_PenaltyMiss_Right )
            {
                ++left_pen_miss;
            }
        }

        for ( std::vector< std::pair< long, rcss::rcg::PlayMode > >::const_iterator it = pen_scores_right.begin();
              it != pen_scores_right.end();
              ++it )
        {
            if ( it->first > view->cycle() ) break;

            if ( it->second == rcss::rcg::PM_PenaltyScore_Left
                 || it->second == rcss::rcg::PM_PenaltyScore_Right )
            {
                ++right_pen_score;
            }
            else if ( it->second == rcss::rcg::PM_PenaltyMiss_Left
                      || it->second == rcss::rcg::PM_PenaltyMiss_Right )
            {
                ++right_pen_miss;
            }
        }

        main_buf.sprintf( " %10s %d(%d/%d):%d(%d/%d) %-10s %16s %6ld",
                          ( left_team.name().empty()
                            || left_team.name() == "null" )
                          ? ""
                          : left_team.name().c_str(),
                          left_team.score(), left_pen_score, left_pen_score + left_pen_miss,
                          right_team.score(), right_pen_score, right_pen_score + right_pen_miss,
                          ( right_team.name().empty()
                            || right_team.name() == "null" )
                          ? ""
                          : right_team.name().c_str(),
                          mode_str.c_str(),
                          view->cycle() );
    }

    painter.setFont( M_font );

    QRect bounding_rect = painter.fontMetrics().boundingRect( main_buf );
    QRect rect( 0,
                painter.window().bottom() - bounding_rect.height() + 1,
                bounding_rect.width(),
                bounding_rect.height() );

    painter.fillRect( rect, M_brush );

    painter.setPen( M_font_pen );
    painter.setBrush( Qt::NoBrush );

    painter.drawText( rect,
                      Qt::AlignVCenter,
                      main_buf );
}
