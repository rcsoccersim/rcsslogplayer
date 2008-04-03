// -*-c++-*-

/*!
  \file config_dialog.cpp
  \brief Config Dialog class Source File.
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

#include <QtGui>

#include "config_dialog.h"

#include <iostream>
#include <cassert>

/*-------------------------------------------------------------------*/
/*!

*/
ConfigDialog::ConfigDialog( QWidget * parent )
    : QDialog( parent )
{
    this->setWindowTitle( tr( "Preference" ) );

    createWidgets();
}

/*-------------------------------------------------------------------*/
/*!

*/
ConfigDialog::~ConfigDialog()
{
    //std::cerr << "delete ConfigDialog" << std::endl;
}

/*-------------------------------------------------------------------*/
/*!

*/
void
ConfigDialog::createWidgets()
{
    M_tab_widget = new QTabWidget();
    QVBoxLayout * top_layout = new QVBoxLayout();

    // canvas
    {
        QFrame * frame = new QFrame();
        QVBoxLayout * layout = new QVBoxLayout();
        layout->setSizeConstraint( QLayout::SetFixedSize );
        layout->setMargin( 4 );
        layout->setSpacing( 4 );

        layout->addWidget( createZoomControls(),
                           0, Qt::AlignLeft );
        layout->addWidget( createObjectSizeControls(),
                           0, Qt::AlignLeft );
        layout->addWidget( createCanvasSizeControls(),
                           0, Qt::AlignLeft );
        layout->addWidget( createFieldStyleControls(),
                           0, Qt::AlignLeft );
        layout->addWidget( createMiscControls(),
                           0, Qt::AlignLeft );

        frame->setLayout( layout );
        M_tab_widget->addTab( frame, tr( "Canvas" ) );
    }
    // show
    {
        QFrame * frame = new QFrame();
        QVBoxLayout * layout = new QVBoxLayout();
        layout->setSizeConstraint( QLayout::SetFixedSize );
        layout->setMargin( 4 );
        layout->setSpacing( 4 );

        layout->addWidget( createShowControls(),
                           0, Qt::AlignLeft );
        layout->addWidget( createPlayersDetailControls(),
                           0, Qt::AlignLeft );
        layout->addWidget( createGridStepControls(),
                           0, Qt::AlignLeft );

        frame->setLayout( layout );
        M_tab_widget->addTab( frame, tr( "Show" ) );
    }
    // trace / future
    {
        QFrame * frame = new QFrame();
        QVBoxLayout * layout = new QVBoxLayout();
        layout->setSizeConstraint( QLayout::SetFixedSize );
        layout->setMargin( 4 );
        layout->setSpacing( 4 );

        layout->addWidget( createFocusControls(),
                           0, Qt::AlignLeft );
        layout->addWidget( createPlayerSelectionControls(),
                           0, Qt::AlignLeft );
        layout->addWidget( createTraceControls(),
                           0, Qt::AlignLeft );
        layout->addWidget( createInertiaMoveControls(),
                           0, Qt::AlignLeft );

        frame->setLayout( layout );
        M_tab_widget->addTab( frame, tr( "Trace" ) );
    }

    top_layout->addWidget( M_tab_widget );
    this->setLayout( top_layout );
}

/*-------------------------------------------------------------------*/
/*!

*/
QWidget *
ConfigDialog::createZoomControls()
{
    QGroupBox * group_box = new QGroupBox( tr( "Zoom" ) );

    QVBoxLayout * top_layout = new QVBoxLayout();
    top_layout->setMargin( 1 );
    top_layout->setSpacing( 0 );

    const QFontMetrics metrics = this->fontMetrics();
    const int button_height = metrics.height() + 12;

    {
        QHBoxLayout * layout = new QHBoxLayout();
        layout->setSpacing( 0 );

        QPushButton * zoomin_button = new QPushButton( tr( "+" ) );
        zoomin_button->setMaximumSize( 48, button_height );
        connect( zoomin_button, SIGNAL( clicked() ),
                 this, SLOT( zoomIn() ) );
        layout->addWidget( zoomin_button );
        //
        QPushButton * zoomout_button = new QPushButton( tr( "-" ) );
        zoomout_button->setMaximumSize( 48, button_height );
        connect( zoomout_button, SIGNAL( clicked() ),
                 this, SLOT( zoomOut() ) );
        layout->addWidget( zoomout_button );
        //
        layout->addSpacing( 1 );
        layout->addStretch( 1 );
        //
        QPushButton * fit_button = new QPushButton( tr( "Fit" ) );
        fit_button->setMaximumSize( 60, button_height );
        connect( fit_button, SIGNAL( clicked() ),
                 this, SLOT( unzoom() ) );
        layout->addWidget( fit_button );

        top_layout->addLayout( layout );
    }

    {
        QHBoxLayout * layout = new QHBoxLayout();
        layout->setSpacing( 0 );

        layout->addWidget( new QLabel( tr( "Scale:" ) ) );

        layout->addSpacing( 2 );

        M_scale_slider = new QSlider( Qt::Horizontal );
        M_scale_slider->setRange( 10, 4000 ); // [1.0, 400.0]
        M_scale_slider->setValue( 80 );
        M_scale_slider->setSingleStep( 1 );
        M_scale_slider->setPageStep( 1 ); // set the minimal increment/decrement step
        M_scale_slider->setMinimumSize( 260, 24 );
        connect( M_scale_slider, SIGNAL( sliderMoved( int ) ),
                 this, SLOT( slideFieldScale( int ) ) );
        layout->addWidget( M_scale_slider );

        layout->addSpacing( 2 );

        M_scale_text = new QLineEdit( tr( "0.0" ) );
        M_scale_text->setValidator( new QDoubleValidator( 0.0, 400.0, 3, M_scale_text ) );
        M_scale_text->setMaximumSize( 48, 24 );
        connect( M_scale_text, SIGNAL( textChanged( const QString & ) ),
                 this, SLOT( editFieldScale( const QString & ) ) );
        layout->addWidget( M_scale_text );

        top_layout->addLayout( layout );
    }


    group_box->setLayout( top_layout );
    return group_box;;
}

/*-------------------------------------------------------------------*/
/*!

*/
QWidget *
ConfigDialog::createObjectSizeControls()
{
    QGroupBox * group_box = new QGroupBox( tr( "Object Size" ) );

    QVBoxLayout * top_layout = new QVBoxLayout();
    top_layout->setMargin( 1 );
    top_layout->setSpacing( 0 );

    M_enlarge_cb = new QCheckBox( tr( "Enlarge Objects" ) );
    M_enlarge_cb->setChecked( true ); //M_view_config.isEnlarged() );
    connect( M_enlarge_cb, SIGNAL( clicked( bool ) ),
             this, SLOT( clickEnlarge( bool ) ) );
    top_layout->addWidget( M_enlarge_cb );


    QHBoxLayout * box = new QHBoxLayout();
    //
    box->addWidget( new QLabel( tr( "Ball Size:" ) ) );
    //
    M_ball_size_text = new QLineEdit( tr( "0.35" ) );
    M_ball_size_text->setValidator( new QDoubleValidator( 0.01, 100.0, 3, M_ball_size_text ) );
    M_ball_size_text->setMaximumSize( 48, 24 );
    connect( M_ball_size_text, SIGNAL( textChanged( const QString & ) ),
             this, SLOT( editBallSize( const QString & ) ) );
    box->addWidget( M_ball_size_text );
    //
    box->addWidget( new QLabel( tr( " Player Size:" ) ) );
    //
    M_player_size_text = new QLineEdit( tr( "0.0" ) );
    M_player_size_text->setValidator( new QDoubleValidator( 0.0, 100.0, 3, M_ball_size_text ) );
    M_player_size_text->setMaximumSize( 48, 24 );
    connect( M_player_size_text, SIGNAL( textChanged( const QString & ) ),
             this, SLOT( editPlayerSize( const QString & ) ) );
    box->addWidget( M_player_size_text );
    //
    top_layout->addLayout( box );

    group_box->setLayout( top_layout );
    return group_box;
}

/*-------------------------------------------------------------------*/
/*!

*/
QWidget *
ConfigDialog::createCanvasSizeControls()
{
    QGroupBox * group_box = new QGroupBox( tr( "Canvas Size" ) );

    QHBoxLayout * layout = new QHBoxLayout();
    layout->setMargin( 1 );
    layout->setSpacing( 0 );

    layout->addWidget( new QLabel( tr( " Width:" ) ) );

    M_canvas_width_text = new QLineEdit( tr( "640" ) );
    M_canvas_width_text->setValidator( new QIntValidator( 100, 3000, M_canvas_width_text ) );
    M_canvas_width_text->setMaximumSize( 48, 24 );
    layout->addWidget( M_canvas_width_text );

    layout->addWidget( new QLabel( tr( " Height:" ) ) );

    M_canvas_height_text = new QLineEdit( tr( "480" ) );
    M_canvas_height_text->setValidator( new QIntValidator( 100, 3000, M_canvas_height_text ) );
    M_canvas_height_text->setMaximumSize( 48, 24 );
    layout->addWidget( M_canvas_height_text );

    layout->addSpacing( 12 );

    QPushButton * apply_canvas_size_btn = new QPushButton( tr( "apply" ) );
    apply_canvas_size_btn->setMaximumSize( 60, this->fontMetrics().height() + 12 );
    connect( apply_canvas_size_btn, SIGNAL( clicked() ),
             this, SLOT( applyCanvasSize() ) );
    layout->addWidget( apply_canvas_size_btn );

    group_box->setLayout( layout );
    return group_box;
}

/*-------------------------------------------------------------------*/
/*!

*/
QWidget *
ConfigDialog::createPlayersDetailControls()
{
    QGroupBox * group_box = new QGroupBox( tr( "Players\' Detail" ) );

    QVBoxLayout * top_layout = new QVBoxLayout();
    top_layout->setMargin( 1 );
    top_layout->setSpacing( 0 );

    {
        QHBoxLayout * layout = new QHBoxLayout();
        layout->setMargin( 0 );
        layout->setSpacing( 0 );

        M_player_number_cb = new QCheckBox( tr( "Unum" ) );
        M_player_number_cb->setChecked( M_view_config.isShownPlayerNumber() );
        connect( M_player_number_cb, SIGNAL( clicked( bool ) ),
                 this, SLOT( clickShowPlayerNumber( bool ) ) );
        layout->addWidget( M_player_number_cb );
        //
        M_hetero_number_cb = new QCheckBox( tr( "Type" ) );
        M_hetero_number_cb->setChecked( M_view_config.isShownHeteroNumber() );
        connect( M_hetero_number_cb, SIGNAL( clicked( bool ) ),
                 this, SLOT( clickShowHeteroNumber( bool ) ) );
        layout->addWidget( M_hetero_number_cb );
        //
        M_stamina_cb = new QCheckBox( tr( "Stamina" ) );
        M_stamina_cb->setChecked( M_view_config.isShownStamina() );
        connect( M_stamina_cb, SIGNAL( clicked( bool ) ),
                 this, SLOT( clickShowStamina( bool ) ) );
        layout->addWidget( M_stamina_cb );

        top_layout->addLayout( layout );
    }
    {
        QHBoxLayout * layout = new QHBoxLayout();
        layout->setMargin( 0 );
        layout->setSpacing( 0 );

        M_view_cone_cb = new QCheckBox( tr( "View Cone" ) );
        M_view_cone_cb->setChecked( M_view_config.isShownViewCone() );
        connect( M_view_cone_cb, SIGNAL( clicked( bool ) ),
                 this, SLOT( clickShowViewCone( bool ) ) );
        layout->addWidget( M_view_cone_cb );
        //
        M_body_shadow_cb = new QCheckBox( tr( "Body Shadow" ) );
        M_body_shadow_cb->setChecked( M_view_config.isShownBodyShadow() );
        connect( M_body_shadow_cb, SIGNAL( clicked( bool ) ),
                 this, SLOT( clickShowBodyShadow( bool ) ) );
        layout->addWidget( M_body_shadow_cb );
        //
        M_control_area_cb = new QCheckBox( tr( "Control Area" ) );
        M_control_area_cb->setChecked( M_view_config.isShownControlArea() );
        connect( M_control_area_cb, SIGNAL( clicked( bool ) ),
                 this, SLOT( clickShowControlArea( bool ) ) );
        layout->addWidget( M_control_area_cb );

        top_layout->addLayout( layout );
    }

    group_box->setLayout( top_layout );
    return group_box;
}

/*-------------------------------------------------------------------*/
/*!

*/
QWidget *
ConfigDialog::createShowControls()
{
    QGroupBox * group_box = new QGroupBox( tr( "Show" ) );

    QVBoxLayout * top_layout = new QVBoxLayout();
    top_layout->setMargin( 1 );
    top_layout->setSpacing( 0 );

    {
        QHBoxLayout * layout = new QHBoxLayout();
        layout->setMargin( 0 );
        layout->setSpacing( 0 );

        M_anonymous_mode_cb = new QCheckBox( tr( "Anonymous Mode" ) );
        M_anonymous_mode_cb->setChecked( M_view_config.anonymousMode() );
        connect( M_anonymous_mode_cb, SIGNAL( clicked( bool ) ),
                 this, SLOT( clickAnonymousMode( bool ) ) );
        layout->addWidget( M_anonymous_mode_cb );

        top_layout->addLayout( layout );
    }
    {
        QHBoxLayout * layout = new QHBoxLayout();
        layout->setMargin( 0 );
        layout->setSpacing( 0 );

        M_show_score_board_cb = new QCheckBox( tr( "Score Board" ) );
        M_show_score_board_cb->setChecked( M_view_config.isShownScoreBoard() );
        connect( M_show_score_board_cb, SIGNAL( clicked( bool ) ),
                 this, SLOT( clickShowScoreBoard( bool ) ) );
        layout->addWidget( M_show_score_board_cb );
        //
        M_show_team_logo_cb = new QCheckBox( tr( "Team Logo" ) );
        M_show_team_logo_cb->setChecked( M_view_config.isShownTeamLogo() );
        connect( M_show_team_logo_cb, SIGNAL( clicked( bool ) ),
                 this, SLOT( clickShowTeamLogo( bool ) ) );
        layout->addWidget( M_show_team_logo_cb );
        //
        M_show_flags_cb = new QCheckBox( tr( "Flag" ) );
        M_show_flags_cb->setChecked( M_view_config.isShownFlags() );
        connect( M_show_flags_cb, SIGNAL( clicked( bool ) ),
                 this, SLOT( clickShowFlags( bool ) ) );
        layout->addWidget( M_show_flags_cb );

        top_layout->addLayout( layout );
    }
    {
        QHBoxLayout * layout = new QHBoxLayout();
        layout->setMargin( 0 );
        layout->setSpacing( 0 );

        M_show_ball_cb = new QCheckBox( tr( "Ball" ) );
        M_show_ball_cb->setChecked( M_view_config.isShownBall() );
        connect( M_show_ball_cb, SIGNAL( clicked( bool ) ),
                 this, SLOT( clickShowBall( bool ) ) );
        layout->addWidget( M_show_ball_cb );
        //
        M_show_players_cb = new QCheckBox( tr( "Player" ) );
        M_show_players_cb->setChecked( M_view_config.isShownPlayers() );
        connect( M_show_players_cb, SIGNAL( clicked( bool ) ),
                 this, SLOT( clickShowPlayers( bool ) ) );
        layout->addWidget( M_show_players_cb );
        //
        M_show_offside_line_cb = new QCheckBox( tr( "Offside Line" ) );
        M_show_offside_line_cb->setChecked( M_view_config.isShownOffsideLine() );
        connect( M_show_offside_line_cb, SIGNAL( clicked( bool ) ),
                 this, SLOT( clickShowOffsideLine( bool ) ) );
        layout->addWidget( M_show_offside_line_cb );

        top_layout->addLayout( layout );
    }

    group_box->setLayout( top_layout );
    return group_box;
}

/*-------------------------------------------------------------------*/
/*!

*/
QWidget *
ConfigDialog::createFocusControls()
{
    QGroupBox * group_box = new QGroupBox( tr( "Focus" ) );

    QHBoxLayout * layout = new QHBoxLayout();
    layout->setMargin( 1 );
    layout->setSpacing( 0 );

    {
        M_focus_ball_rb = new QRadioButton( tr( "Ball" ) );
        connect( M_focus_ball_rb, SIGNAL( clicked() ),
                 this, SLOT( clickFocusBall() ) );
        layout->addWidget( M_focus_ball_rb );
    }
    {
        M_focus_player_rb = new QRadioButton( tr( "Selected Player" ) );
        connect( M_focus_player_rb, SIGNAL( clicked() ),
                 this, SLOT( clickFocusPlayer() ) );
        layout->addWidget( M_focus_player_rb );
    }
    {
        M_focus_fix_rb = new QRadioButton( tr( "Fix" ) );
        connect( M_focus_fix_rb, SIGNAL( clicked() ),
                 this, SLOT( setFocusFix() ) );
        layout->addWidget( M_focus_fix_rb );
    }
    M_focus_fix_rb->setChecked( true );

    group_box->setLayout( layout );
    return group_box;
}

/*-------------------------------------------------------------------*/
/*!

*/
QWidget *
ConfigDialog::createPlayerSelectionControls()
{
    QGroupBox * group_box = new QGroupBox( tr( "Player Selection" ) );

    QVBoxLayout * top_layout = new QVBoxLayout();
    top_layout->setMargin( 1 );
    top_layout->setSpacing( 0 );

    // selection type
    {
        QHBoxLayout * layout = new QHBoxLayout();
        layout->setMargin( 0 );
        layout->setSpacing( 0 );
        {
            M_select_all_rb = new QRadioButton( tr( "Auto" ) );
            connect( M_select_all_rb, SIGNAL( clicked() ),
                     this, SLOT( clickSelectAutoAll() ) );
            layout->addWidget( M_select_all_rb );
        }
        {
            M_select_left_rb = new QRadioButton( tr( "Auto left" ) );
            connect( M_select_left_rb, SIGNAL( clicked() ),
                     this, SLOT( clickSelectAutoLeft() ) );
            layout->addWidget( M_select_left_rb );
        }
        {
            M_select_right_rb = new QRadioButton( tr( "Auto right" ) );
            connect( M_select_right_rb, SIGNAL( clicked() ),
                     this, SLOT( clickSelectAutoRight() ) );
            layout->addWidget( M_select_right_rb );
        }
        {
            M_select_fix_rb = new QRadioButton( tr( "Fix" ) );
            connect( M_select_all_rb, SIGNAL( clicked() ),
                     this, SLOT( clickSelectFix() ) );
            layout->addWidget( M_select_fix_rb );
        }
        {
            M_unselect_rb = new QRadioButton( tr( "Unselect" ) );
            connect( M_unselect_rb, SIGNAL( clicked() ),
                     this, SLOT( setUnselect() ) );
            layout->addWidget( M_unselect_rb );
        }
        M_unselect_rb->setChecked( true );

        top_layout->addLayout( layout );
    }

    {
        QHBoxLayout * layout = new QHBoxLayout();
        layout->setMargin( 0 );
        layout->setSpacing( 0 );
        //layout->addSpacing( 16 );

        M_player_choice = new QComboBox();
        M_player_choice->addItem( tr ( "No Selection" ) );
        for ( int i = 1; i <= 11; ++i )
        {
            M_player_choice->addItem( QString( "Left number: %1" ).arg( i ) );
        }
        for ( int i = 1; i <= 11; ++i )
        {
            M_player_choice->addItem( QString( "Right number: %1" ).arg( i ) );
        }
        M_player_choice->setCurrentIndex( 0 );
        int id_width = this->fontMetrics().width( tr( "Right Number: 11" ) );
        M_player_choice->setMaximumWidth( id_width + 40 );
        connect( M_player_choice, SIGNAL( currentIndexChanged( int ) ),
                 this, SLOT( choicePlayer( int ) ) );
        layout->addWidget( M_player_choice, 0, Qt::AlignLeft );

        layout->addSpacing( 16 );

        top_layout->addLayout( layout );
    }

    group_box->setLayout( top_layout );
    return group_box;
}

/*-------------------------------------------------------------------*/
/*!

*/
QWidget *
ConfigDialog::createFieldStyleControls()
{
    QGroupBox * group_box = new QGroupBox( tr( "Field Style" ) );

    QVBoxLayout * top_layout = new QVBoxLayout();
    top_layout->setMargin( 1 );
    top_layout->setSpacing( 0 );

    QHBoxLayout * layout = new QHBoxLayout();
    layout->setMargin( 1 );
    layout->setSpacing( 0 );

    {
        M_keepaway_mode_cb = new QCheckBox( tr( "Keepaway" ) );
        M_keepaway_mode_cb->setChecked( M_view_config.keepawayMode() );
        connect( M_keepaway_mode_cb, SIGNAL( clicked( bool ) ),
                 this, SLOT( clickKeepawayMode( bool ) ) );
        layout->addWidget( M_keepaway_mode_cb );
    }
    top_layout->addLayout( layout );


    group_box->setLayout( top_layout );
    return group_box;
}

/*-------------------------------------------------------------------*/
/*!

*/
QWidget *
ConfigDialog::createMiscControls()
{
    QGroupBox * group_box = new QGroupBox( tr( "Misc" ) );
    QVBoxLayout * top_layout = new QVBoxLayout();
    top_layout->setMargin( 1 );
    top_layout->setSpacing( 0 );

    //
    M_anti_aliasing_cb = new QCheckBox( tr( "Anti Aliasing" ) );
    M_anti_aliasing_cb->setChecked( M_view_config.antiAliasing() );
    connect( M_anti_aliasing_cb, SIGNAL( toggled( bool ) ),
             this, SLOT( clickAntiAliasing( bool ) ) );
    top_layout->addWidget( M_anti_aliasing_cb );

    group_box->setLayout( top_layout );
    return group_box;
}

/*-------------------------------------------------------------------*/
/*!

*/
QWidget *
ConfigDialog::createGridStepControls()
{
    QGroupBox * group_box = new QGroupBox( tr( "Grid Lines" ) );
    QVBoxLayout * top_layout = new QVBoxLayout();
    top_layout->setMargin( 1 );
    top_layout->setSpacing( 0 );

    M_show_grid_coord_cb = new QCheckBox( tr( "Grid Coordinate" ) );
    M_show_grid_coord_cb->setChecked( M_view_config.isShownGridCoord() );
    connect( M_show_grid_coord_cb, SIGNAL( toggled( bool ) ),
             this, SLOT( clickShowGridCoord( bool ) ) );
    top_layout->addWidget( M_show_grid_coord_cb );

    //
    QHBoxLayout * layout = new QHBoxLayout();
    //layout->setMargin( 0 );
    //layout->setSpacing( 0 );
    //
    layout->addWidget( new QLabel( tr( "Step:" ) ) );
    //
    M_grid_step_slider = new QSlider( Qt::Horizontal );
    M_grid_step_slider->setRange( 0, 1000 );
    M_grid_step_slider->setValue( 0 );
    M_grid_step_slider->setSingleStep( 1 );
    M_grid_step_slider->setPageStep( 1 );
    M_grid_step_slider->setMinimumSize( 300, 24 );
    connect( M_grid_step_slider, SIGNAL( sliderMoved( int ) ),
             this, SLOT( slideGridStep( int ) ) );
    layout->addWidget( M_grid_step_slider );

    //
    M_grid_step_text = new QLineEdit( tr( " 000.0" )  );
    M_grid_step_text->setValidator( new QDoubleValidator( 0.0, 200.0, 2, M_grid_step_text ) );
    M_grid_step_text->setMaximumSize( 48, 24 );
    connect( M_grid_step_text, SIGNAL( textChanged( const QString & ) ),
             this, SLOT( editGridStep( const QString & ) ) );
    layout->addWidget( M_grid_step_text );

    top_layout->addLayout( layout );
    group_box->setLayout( top_layout );
    return group_box;
}

/*-------------------------------------------------------------------*/
/*!

*/
QWidget *
ConfigDialog::createTraceControls()
{
    QGroupBox * group_box = new QGroupBox( tr( "Trace" ) );

    QVBoxLayout * top_layout = new QVBoxLayout();
    top_layout->setMargin( 0 );
    top_layout->setSpacing( 0 );

//     QHBoxLayout * first_layout = new QHBoxLayout();
//     first_layout->setMargin( 0 );
//     first_layout->setSpacing( 0 );
    {
        QGroupBox * ball_box = new QGroupBox( tr( "Ball" ) );

        QHBoxLayout * layout = new QHBoxLayout();
        layout->setSizeConstraint( QLayout::SetFixedSize );
        layout->setMargin( 0 );
        layout->setSpacing( 0 );

        M_ball_trace_start = new QLineEdit( tr( "0" ) );
        M_ball_trace_start->setValidator( new QIntValidator( 0, 20000, M_ball_trace_start ) );
        M_ball_trace_start->setMaximumSize( 36, 24 );
        connect( M_ball_trace_start, SIGNAL( textEdited( const QString & ) ),
                 this, SLOT( editBallTraceStart( const QString & ) ) );
        layout->addWidget( M_ball_trace_start, 0, Qt::AlignVCenter );

        layout->addWidget( new QLabel( tr( "-" ) ), 0, Qt::AlignVCenter );

        M_ball_trace_end = new QLineEdit( tr( "0" ) );
        M_ball_trace_end->setValidator( new QIntValidator( 0, 20000, M_ball_trace_end ) );
        M_ball_trace_end->setMaximumSize( 36, 24 );
        connect( M_ball_trace_end, SIGNAL( textEdited( const QString & ) ),
                 this, SLOT( editBallTraceEnd( const QString & ) ) );
        layout->addWidget( M_ball_trace_end, 0, Qt::AlignVCenter );

        QPushButton * all_btn = new QPushButton( tr( "All" ) );
        all_btn->setMaximumSize( 48, this->fontMetrics().height() + 12 );
        all_btn->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
        connect( all_btn, SIGNAL( clicked() ),
                 this, SLOT( clickBallTraceAll() ) );
        layout->addWidget( all_btn, 0, Qt::AlignVCenter );

        M_ball_trace_cb = new QCheckBox( tr( "Auto" ) );
        M_ball_trace_cb->setChecked( false );
        connect( M_ball_trace_cb, SIGNAL( clicked( bool ) ),
                 this, SLOT( clickAutoBallTrace( bool ) ) );
        layout->addWidget( M_ball_trace_cb, 0, Qt::AlignVCenter );

        ball_box->setLayout( layout );
        //first_layout->addWidget( ball_box );
        top_layout->addWidget( ball_box );
    }
    //first_layout->addSpacing( 2 );
    top_layout->addSpacing( 2 );
    {
        QGroupBox * player_box = new QGroupBox( tr( "Player" ) );

        QHBoxLayout * layout= new QHBoxLayout();
        layout->setSizeConstraint( QLayout::SetFixedSize );
        layout->setMargin( 0 );
        layout->setSpacing( 0 );

        M_player_trace_start = new QLineEdit( tr( "0" ) );
        M_player_trace_start->setValidator( new QIntValidator( 0, 20000, M_player_trace_start ) );
        M_player_trace_start->setMaximumSize( 36, 24 );
        connect( M_player_trace_start, SIGNAL( textEdited( const QString & ) ),
                 this, SLOT( editPlayerTraceStart( const QString & ) ) );
        layout->addWidget( M_player_trace_start, 0, Qt::AlignVCenter );

        layout->addWidget( new QLabel( tr( "-" ) ), 0, Qt::AlignVCenter );

        M_player_trace_end = new QLineEdit( tr( "0" ) );
        M_player_trace_end->setValidator( new QIntValidator( 0, 20000, M_player_trace_end ) );
        M_player_trace_end->setMaximumSize( 36, 24 );
        connect( M_ball_trace_end, SIGNAL( textEdited( const QString & ) ),
                 this, SLOT( editBallTraceEnd( const QString & ) ) );
        layout->addWidget( M_player_trace_end, 0, Qt::AlignVCenter );

        QPushButton * all_btn = new QPushButton( tr( "All" ) );
        all_btn->setMaximumSize( 48, this->fontMetrics().height() + 12 );
        all_btn->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
        connect( all_btn, SIGNAL( clicked() ),
                 this, SLOT( clickPlayerTraceAll() ) );
        layout->addWidget( all_btn, 0, Qt::AlignVCenter );

        M_player_trace_cb =  new QCheckBox( tr( "Auto" ) );
        M_player_trace_cb->setChecked( false );
        connect( M_player_trace_cb, SIGNAL( clicked( bool ) ),
                 this, SLOT( clickAutoPlayerTrace( bool ) ) );
        layout->addWidget( M_player_trace_cb, 0, Qt::AlignVCenter );

        player_box->setLayout( layout );
        //first_layout->addWidget( player_box );
        top_layout->addWidget( player_box );
    }

    //top_layout->addLayout( first_layout );
    top_layout->addSpacing( 1 );

    QHBoxLayout * second_layout = new QHBoxLayout();
    second_layout->setMargin( 0 );
    second_layout->setSpacing( 0 );
    {
        QGroupBox * trace_box = new QGroupBox( tr( "Auto Trace Period" ) );

        QHBoxLayout * layout = new QHBoxLayout();
        layout->setMargin( 0 );
        layout->setSpacing( 0 );

        layout->addWidget( new QLabel( tr( "Start:" ) ), 0, Qt::AlignVCenter );

        layout->addSpacing( 2 );

        M_auto_trace_start = new QLineEdit( tr( "-10" ) );
        M_auto_trace_start->setValidator( new QIntValidator( -20000, 20000, M_auto_trace_start ) );
        M_auto_trace_start->setMinimumSize( 36, this->fontMetrics().height() + 4 );
        M_auto_trace_start->setMaximumSize( 48, this->fontMetrics().height() + 4 );
        connect( M_auto_trace_start, SIGNAL( textEdited( const QString & ) ),
                 this, SLOT( editAutoTraceStart( const QString & ) ) );
        layout->addWidget( M_auto_trace_start, 0, Qt::AlignVCenter );

        layout->addSpacing( 6 );

        layout->addWidget( new QLabel( tr( "Period:" ) ), 0, Qt::AlignVCenter );

        layout->addSpacing( 2 );

        M_auto_trace_period = new QSpinBox();
        M_auto_trace_period->setValue( 10 );
        M_auto_trace_period->setRange( 1, 12000 );
        //M_auto_trace_period->setMaximumSize( 36, 24 );
        connect( M_auto_trace_period, SIGNAL( valueChanged( int ) ),
                 this, SLOT( changeAutoTracePeriod( int ) ) );
        layout->addWidget( M_auto_trace_period, 0, Qt::AlignVCenter );

        trace_box->setLayout( layout );

        second_layout->addWidget( trace_box, 1 );
    }

    second_layout->addSpacing( 12 );
    {
//         QHBoxLayout * layout = new QHBoxLayout();
//         layout->setMargin( 0 );
//         layout->setSpacing( 0 );

//         layout->addStretch( 1 );

        QPushButton * line_point_btn = new QPushButton( tr( "Line/Point" ) );
        line_point_btn->setMaximumSize( 80, this->fontMetrics().height() + 12 );
        connect( line_point_btn, SIGNAL( clicked() ),
                 this, SLOT( clickLinePointButton() ) );

        second_layout->addWidget( line_point_btn, 0, Qt::AlignLeft );
//         layout->addWidget( line_point_btn, 2 );

//         layout->addStretch( 1 );

//         second_layout->addLayout( layout, 1 );
    }
    second_layout->addSpacing( 6 );

    top_layout->addLayout( second_layout );

    group_box->setLayout( top_layout );
    return group_box;;
}

/*-------------------------------------------------------------------*/
/*!

*/
QWidget *
ConfigDialog::createInertiaMoveControls()
{
    QGroupBox * group_box = new QGroupBox( tr( "Inertia Move" ) );

    QHBoxLayout * top_layout = new QHBoxLayout();
    top_layout->setMargin( 0 );
    top_layout->setSpacing( 0 );

    top_layout->addWidget( new QLabel( tr( "Ball:" ) ) );

    top_layout->addSpacing( 2 );

    M_ball_future = new QSpinBox();
    //M_ball_future->setMinimumSize( 60, 24 );
    M_ball_future->setValue( 0 );
    M_ball_future->setRange( 0, 50 );
    connect( M_ball_future, SIGNAL( valueChanged( int ) ),
             this, SLOT( changeBallFutureCycle( int ) ) );
    top_layout->addWidget( M_ball_future );

    top_layout->addSpacing( 6 );

    top_layout->addWidget( new QLabel( tr( "Player:" ) ) );

    top_layout->addSpacing( 2 );

    M_player_future = new QSpinBox();
    //M_player_future->setMaximumSize( 60, 24 );
    M_player_future->setValue( 0 );
    M_player_future->setRange( 0, 10 );
    connect( M_player_future, SIGNAL( valueChanged( int ) ),
             this, SLOT( changePlayerFutureCycle( int ) ) );
    top_layout->addWidget( M_player_future );

    group_box->setLayout( top_layout );
    return group_box;
}

/*-------------------------------------------------------------------*/
/*!

*/
void
ConfigDialog::showEvent( QShowEvent * event )
{
    updateAll();
    event->accept();
}

/*-------------------------------------------------------------------*/
/*!

*/
void
ConfigDialog::updateAll()
{
    const Options & opt = Options::instance();

    M_enlarge_cb->setChecked( opt.isEnlarged() );
    M_ball_size_text->setText( QString::number( opt.ballSize() ) );
    M_player_size_text->setText( QString::number( opt.playerSize() ) );

    updateFieldScale();

    M_canvas_width_text
        ->setText( QString::number( opt.canvasWidth() ) );

    M_canvas_height_text
        ->setText( QString::number( opt.canvasHeight() ) );

    M_player_number_cb->setChecked( opt.isShownPlayerNumber() );
    M_hetero_number_cb->setChecked( opt.isShownHeteroNumber() );
    M_stamina_cb->setChecked( opt.isShownStamina() );
    M_view_cone_cb->setChecked( opt.isShownViewCone() );
    M_body_shadow_cb->setChecked( opt.isShownBodyShadow() );
    M_control_area_cb->setChecked( opt.isShownControlArea() );

    M_anonymous_mode_cb->setChecked( opt.anonymousMode() );
    M_show_score_board_cb->setChecked( opt.isShownScoreBoard() );
    M_show_team_logo_cb->setChecked( opt.isShownTeamLogo() );
    M_show_ball_cb->setChecked( opt.isShownBall() );
    M_show_players_cb->setChecked( opt.isShownPlayers() );
    M_show_flags_cb->setChecked( opt.isShownFlags() );
    M_show_offside_line_cb->setChecked( opt.isShownOffsideLine() );

    M_keepaway_mode_cb->setChecked( opt.keepawayMode() );

    M_anti_aliasing_cb->setChecked( opt.antiAliasing() );


    switch( opt.focusType() ) {
    case ViewConfig::FOCUS_BALL:
        M_focus_ball_rb->setChecked( true );
        break;
    case ViewConfig::FOCUS_PLAYER:
        M_focus_player_rb->setChecked( true );
        break;
    case ViewConfig::FOCUS_POINT:
        M_focus_fix_rb->setChecked( true );
        break;
    default:
        M_focus_fix_rb->setChecked( true );
        break;
    }

    switch ( opt.playerSelectType() ) {
    case ViewConfig::SELECT_AUTO_ALL:
        M_select_all_rb->setChecked( true );
        break;
    case ViewConfig::SELECT_AUTO_LEFT:
        M_select_left_rb->setChecked( true );
        break;
    case ViewConfig::SELECT_AUTO_RIGHT:
        M_select_right_rb->setChecked( true );
        break;
    case ViewConfig::SELECT_UNSELECT:
        M_unselect_rb->setChecked( true );
        break;
    default:
        M_unselect_rb->setChecked( true );
        break;
    }

    M_ball_trace_start->setText( QString::number( opt.ballTraceStart() ) );
    M_ball_trace_end->setText( QString::number( opt.ballTraceEnd() ) );
    M_ball_trace_cb->setChecked( opt.isBallAutoTrace() );

    M_player_trace_start->setText( QString::number( opt.playerTraceStart() ) );
    M_player_trace_end->setText( QString::number( opt.playerTraceEnd() ) );
    M_player_trace_cb->setChecked( opt.isPlayerAutoTrace() );

    M_auto_trace_start->setText( QString::number( opt.autoTraceStart() ) );
    M_auto_trace_period->setValue( opt.autoTracePeriod() );

    M_ball_future->setValue( opt.ballFutureCycle() );
    M_player_future->setValue( opty.playerFutureCycle() );
}

/*-------------------------------------------------------------------*/
/*!

*/
void
ConfigDialog::updateFieldScale()
{
    int iscale = static_cast< int >( rint( Options::instance().fieldScale() * 10.0 ) );
    if ( M_scale_slider->value() != iscale )
    {
        M_scale_slider->setValue( iscale );
    }

    char buf[16];
    std::snprintf( buf, 16, "%.2f", Options::instance().fieldScale() );
    M_scale_text->setText( QString::fromAscii( buf ) );
}

/*-------------------------------------------------------------------*/
/*!

*/
void
ConfigDialog::zoomIn()
{
    Options::instance().zoomIn();

    updateFieldScale();

    emit configured();
}

/*-------------------------------------------------------------------*/
/*!

*/
void
ConfigDialog::zoomOut()
{
    Options::instance().zoomOut();

    updateFieldScale();

    emit configured();
}

/*-------------------------------------------------------------------*/
/*!

*/
void
ConfigDialog::unzoom()
{
    Options::instance().unzoom();
    updateFieldScale();
    M_focus_fix_rb->setChecked( true );

    emit configured();
}

/*-------------------------------------------------------------------*/
/*!

*/
void
ConfigDialog::clickEnlarge( bool checked )
{
    if ( Options::instance().isEnlarged() != checked )
    {
        Options::instance().toggleEnlarge();
        emit configured();
    }
}

/*-------------------------------------------------------------------*/
/*!

*/
void
ConfigDialog::toggleEnlarge()
{
    Options::instance().toggleEnlarge();
    M_enlarge_cb->setChecked( Options::instance().isEnlarged() );

    emit configured();
}

/*-------------------------------------------------------------------*/
/*!

*/
void
ConfigDialog::editBallSize( const QString & text )
{
    bool ok = true;
    double value = text.toDouble( &ok );

    if ( ok && value > 0.0 )
    {
        Options::instance().setBallSize( value );

        emit configured();
    }
}

/*-------------------------------------------------------------------*/
/*!

*/
void
ConfigDialog::editPlayerSize( const QString & text )
{
    bool ok = true;
    double value = text.toDouble( &ok );

    if ( ok )
    {
        Options::instance().setPlayerSize( value );

        emit configured();
    }
}

/*-------------------------------------------------------------------*/
/*!

*/
void
ConfigDialog::slideFieldScale( int value )
{
    double scale = value * 0.1;

    if ( std::fabs( scale - Options::instance().fieldScale() ) >= 0.01 )
    {
        M_scale_text->setText( QString::number( scale ) );

        Options::instance().setFieldScale( scale );

        emit configured();
    }
}

/*-------------------------------------------------------------------*/
/*!

*/
void
ConfigDialog::editFieldScale( const QString & text )
{
    bool ok = true;
    double value = text.toDouble( &ok );

    if ( ok
         && std::fabs( value - Options::instance().fieldScale() ) >= 0.01 )
    {
        int ivalue = static_cast< int >( rint( value * 10.0 ) );
        M_scale_slider->setValue( ivalue );

        Options::instance().setFieldScale( value );

        emit configured();
    }
}

/*-------------------------------------------------------------------*/
/*!

*/
void
ConfigDialog::applyCanvasSize()
{
    bool ok_w = true;
    bool ok_h = true;
    int width = M_canvas_width_text->text().toInt( & ok_w );
    int height = M_canvas_height_text->text().toInt( & ok_h );

    if ( ok_w
         && ok_h
         && width > 0
         && height > 0 )
    {
        emit canvasResized( QSize( width, height ) );
    }
}

/*-------------------------------------------------------------------*/
/*!

*/
void
ConfigDialog::clickReverseSide( bool checked )
{
    if ( Options::instance().reverseSide() != checked )
    {
        Options::instance().toggleReverseSide();
        emit configured();
    }
}

/*-------------------------------------------------------------------*/
/*!

*/
void
ConfigDialog::toggleReverseSide()
{
    Options::instance().toggleReverseSide();
    M_reverse_side_cb->setChecked( Options::instance().reverseSide() );

    emit configured();
}

/*-------------------------------------------------------------------*/
/*!

*/
void
ConfigDialog::clickPlayerReverseDraw( bool checked )
{
    if ( Options::instance().playerReverseDraw() != checked )
    {
        Options::instance().togglePlayerReverseDraw();
        emit configured();
    }
}

/*-------------------------------------------------------------------*/
/*!

*/
void
ConfigDialog::togglePlayerReverseDraw()
{
    Options::instance().togglePlayerReverseDraw();
    M_player_reverse_draw_cb->setChecked( Options::instance().playerReverseDraw() );

    emit configured();
}

/*-------------------------------------------------------------------*/
/*!

*/
void
ConfigDialog::clickShowPlayerNumber( bool checked )
{
    if ( Options::instance().isShownPlayerNumber() != checked )
    {
        Options::instance().toggleShowPlayerNumber();
        emit configured();
    }
}

/*-------------------------------------------------------------------*/
/*!

*/
void
ConfigDialog::toggleShowPlayerNumber()
{
    Options::instance().toggleShowPlayerNumber();
    M_player_number_cb->setChecked( Options::instance().isShownPlayerNumber() );

    emit configured();
}

/*-------------------------------------------------------------------*/
/*!

*/
void
ConfigDialog::clickShowHeteroNumber( bool checked )
{
    if ( Options::instance().isShownHeteroNumber() != checked )
    {
        Options::instance().toggleShowHeteroNumber();
        emit configured();
    }
}

/*-------------------------------------------------------------------*/
/*!

*/
void
ConfigDialog::toggleShowHeteroNumber()
{
    Options::instance().toggleShowHeteroNumber();
    M_hetero_number_cb->setChecked( Options::instance().isShownHeteroNumber() );

    emit configured();
}

/*-------------------------------------------------------------------*/
/*!

*/
void
ConfigDialog::clickShowStamina( bool checked )
{
    if ( Options::instance().isShownStamina() != checked )
    {
        Options::instance().toggleShowStamina();
        emit configured();
    }
}

/*-------------------------------------------------------------------*/
/*!

*/
void
ConfigDialog::toggleShowStamina()
{
    Options::instance().toggleShowStamina();
    M_stamina_cb->setChecked( Options::instance().isShownStamina() );

    emit configured();
}

/*-------------------------------------------------------------------*/
/*!

*/
void
ConfigDialog::clickShowViewCone( bool checked )
{
    if ( Options::instance().isShownViewCone() != checked )
    {
        Options::instance().toggleShowViewCone();
        emit configured();
    }
}

/*-------------------------------------------------------------------*/
/*!

*/
void
ConfigDialog::toggleShowViewCone()
{
    Options::instance().toggleShowViewCone();
    M_view_cone_cb->setChecked( Options::instance().isShownViewCone() );

    emit configured();
}

/*-------------------------------------------------------------------*/
/*!

*/
void
ConfigDialog::clickShowBodyShadow( bool checked )
{
    if ( Options::instance().isShownBodyShadow() != checked )
    {
        Options::instance().toggleShowBodyShadow();
        emit configured();
    }
}

/*-------------------------------------------------------------------*/
/*!

*/
void
ConfigDialog::toggleShowBodyShadow()
{
    Options::instance().toggleShowBodyShadow();
    M_body_shadow_cb->setChecked( Options::instance().isShownBodyShadow() );

    emit configured();
}

/*-------------------------------------------------------------------*/
/*!

*/
void
ConfigDialog::clickShowControlArea( bool checked )
{
    if ( Options::instance().isShownControlArea() != checked )
    {
        Options::instance().toggleShowControlArea();
        emit configured();
    }
}

/*-------------------------------------------------------------------*/
/*!

*/
void
ConfigDialog::toggleShowControlArea()
{
    Options::instance().toggleShowControlArea();
    M_control_area_cb->setChecked( Options::instance().isShownControlArea() );

    emit configured();
}

/*-------------------------------------------------------------------*/
/*!

*/
void
ConfigDialog::clickAnonymousMode( bool checked )
{
    if ( Options::instance().anonymousMode() != checked )
    {
        Options::instance().toggleAnonymousMode();
        emit configured();
    }
}

/*-------------------------------------------------------------------*/
/*!

*/
void
ConfigDialog::toggleAnonymousMode()
{
    Options::instance().toggleAnonymousMode();
    M_show_score_board_cb->setChecked( Options::instance().anonymousMode() );

    emit configured();
}

/*-------------------------------------------------------------------*/
/*!

*/
void
ConfigDialog::clickShowScoreBoard( bool checked )
{
    if ( Options::instance().isShownScoreBoard() != checked )
    {
        Options::instance().toggleShowScoreBoard();
        emit configured();
    }
}

/*-------------------------------------------------------------------*/
/*!

*/
void
ConfigDialog::toggleShowScoreBoard()
{
    Options::instance().toggleShowScoreBoard();
    M_show_score_board_cb->setChecked( Options::instance().isShownScoreBoard() );

    emit configured();
}

/*-------------------------------------------------------------------*/
/*!

*/
void
ConfigDialog::clickShowTeamLogo( bool checked )
{
    if ( Options::instance().isShownTeamLogo() != checked )
    {
        Options::instance().toggleShowTeamLogo();
        emit configured();
    }
}

/*-------------------------------------------------------------------*/
/*!

*/
void
ConfigDialog::toggleShowTeamLogo()
{
    Options::instance().toggleShowTeamLogo();
    M_show_score_board_cb->setChecked( Options::instance().isShownTeamLogo() );

    emit configured();
}

/*-------------------------------------------------------------------*/
/*!

*/
void
ConfigDialog::clickShowBall( bool checked )
{
    if ( Options::instance().isShownBall() != checked )
    {
        Options::instance().toggleShowBall();
        emit configured();
    }
}

/*-------------------------------------------------------------------*/
/*!

*/
void
ConfigDialog::toggleShowBall()
{
    Options::instance().toggleShowBall();
    M_show_ball_cb->setChecked( Options::instance().isShownBall() );

    emit configured();
}

/*-------------------------------------------------------------------*/
/*!

*/
void
ConfigDialog::clickShowPlayers( bool checked )
{
    if ( Options::instance().isShownPlayers() != checked )
    {
        Options::instance().toggleShowPlayers();
        emit configured();
    }
}

/*-------------------------------------------------------------------*/
/*!

*/
void
ConfigDialog::toggleShowPlayers()
{
    Options::instance().toggleShowPlayers();
    M_show_players_cb->setChecked( Options::instance().isShownPlayers() );

    emit configured();
}

/*-------------------------------------------------------------------*/
/*!

*/
void
ConfigDialog::clickShowFlags( bool checked )
{
    if ( Options::instance().isShownFlags() != checked )
    {
        Options::instance().toggleShowFlags();
        emit configured();
    }
}

/*-------------------------------------------------------------------*/
/*!

*/
void
ConfigDialog::toggleShowFlags()
{
    Options::instance().toggleShowFlags();
    M_show_flags_cb->setChecked( Options::instance().isShownFlags() );

    emit configured();
}

/*-------------------------------------------------------------------*/
/*!

*/
void
ConfigDialog::clickShowOffsideLine( bool checked )
{
    if ( Options::instance().isShownOffsideLine() != checked )
    {
        Options::instance().toggleShowOffsideLine();
        emit configured();
    }
}

/*-------------------------------------------------------------------*/
/*!

*/
void
ConfigDialog::toggleShowOffsideLine()
{
    Options::instance().toggleShowOffsideLine();
    M_show_offside_line_cb->setChecked( Options::instance().isShownOffsideLine() );

    emit configured();
}

/*-------------------------------------------------------------------*/
/*!

*/
void
ConfigDialog::clickGrassNormal()
{
    if ( Options::instance().grassType() != ViewConfig::GRASS_NORMAL )
    {
        Options::instance().setGrassType( ViewConfig::GRASS_NORMAL );
        emit configured();
    }
}

/*-------------------------------------------------------------------*/
/*!

*/
void
ConfigDialog::clickGrassLine()
{
    if ( Options::instance().grassType() != ViewConfig::GRASS_LINES )
    {
        Options::instance().setGrassType( ViewConfig::GRASS_LINES );
        emit configured();
    }
}

/*-------------------------------------------------------------------*/
/*!

*/
void
ConfigDialog::clickGrassChecker()
{
    if ( Options::instance().grassType() != ViewConfig::GRASS_CHECKER )
    {
        Options::instance().setGrassType( ViewConfig::GRASS_CHECKER );
        emit configured();
    }
}

/*-------------------------------------------------------------------*/
/*!

*/
void
ConfigDialog::clickKeepawayMode( bool checked )
{
    if ( Options::instance().keepawayMode() != checked )
    {
        Options::instance().toggleKeepawayMode();
        emit configured();
    }
}

/*-------------------------------------------------------------------*/
/*!

*/
void
ConfigDialog::clickShowGridCoord( bool checked )
{
    if ( Options::instance().isShownGridCoord() != checked )
    {
        Options::instance().toggleShowGridCoord();
        emit configured();
    }
}

/*-------------------------------------------------------------------*/
/*!

*/
void
ConfigDialog::toggleKeepawayMode()
{
    Options::instance().toggleKeepawayMode();
    M_keepaway_mode_cb->setChecked( Options::instance().keepawayMode() );

    emit configured();
}

/*-------------------------------------------------------------------*/
/*!

*/
void
ConfigDialog::clickCursorHide( bool checked )
{
    if ( Options::instance().cursorHide() != checked )
    {
        Options::instance().toggleCursorHide();

        //emit configured();
    }
}

/*-------------------------------------------------------------------*/
/*!

*/
void
ConfigDialog::clickAntiAliasing( bool checked )
{
    if ( Options::instance().antiAliasing() != checked )
    {
        Options::instance().toggleAntiAliasing();

        emit configured();
    }
}

/*-------------------------------------------------------------------*/
/*!

*/
void
ConfigDialog::clickGradient( bool checked )
{
    if ( Options::instance().gradient() != checked )
    {
        Options::instance().toggleGradient();

        emit configured();
    }
}

/*-------------------------------------------------------------------*/
/*!

*/
void
ConfigDialog::slideGridStep( int value )
{
    M_grid_step_text->setText( QString::number( value * 0.1 ) );

    Options::instance().setGridStep( value );

    emit configured();
}

/*-------------------------------------------------------------------*/
/*!

*/
void
ConfigDialog::editGridStep( const QString & text )
{
    bool ok = true;
    double value = text.toDouble( & ok );

    if ( ok )
    {
        Options::instance().setGridStep( static_cast< int >( rint( value * 10.0 ) ) );

        emit configured();
    }
}

/*-------------------------------------------------------------------*/
/*!

*/
void
ConfigDialog::selectDrawType( int index )
{
    if ( static_cast< int >( Options::instance().drawType() ) != index )
    {
        Options::instance().setDrawType( static_cast< ViewConfig::DrawType >( index ) );
        emit configured();
    }
}

/*-------------------------------------------------------------------*/
/*!

*/
void
ConfigDialog::clickShowVoronoiDiagram( bool checked )
{
    if ( Options::instance().isShownVoronoiDiagram() != checked )
    {
        Options::instance().toggleShowVoronoiDiagram();
        emit configured();
    }
}

/*-------------------------------------------------------------------*/
/*!

*/
void
ConfigDialog::clickShowDelaunayTrianglation( bool checked )
{
    if ( Options::instance().isShownDelaunayTrianglation() != checked )
    {
        Options::instance().toggleShowDelaunayTrianglation();
        emit configured();
    }
}

/*-------------------------------------------------------------------*/
/*!

*/
void
ConfigDialog::selectCompGeomSide( int index )
{
    switch ( index ) {
    case 0:
        if ( Options::instance().voronoiTarget() != rcsc::NEUTRAL )
        {
            Options::instance().setVoronoiTarget( rcsc::NEUTRAL );
            emit configured();
        }
        break;
    case 1:
        if ( Options::instance().voronoiTarget() != rcsc::LEFT )
        {
            Options::instance().setVoronoiTarget( rcsc::LEFT );
            emit configured();
        }
        break;
    case 2:
        if ( Options::instance().voronoiTarget() != rcsc::RIGHT )
        {
            Options::instance().setVoronoiTarget( rcsc::RIGHT );
            emit configured();
        }
        break;
    default:
        std::cerr << "ConfigDialog::selectCompGeomSide() unsupported index "
                  << index
                  << std::endl;
        break;
    }
}

/*-------------------------------------------------------------------*/
/*!

*/
void
ConfigDialog::clickFocusBall()
{
    if ( Options::instance().focusType() != ViewConfig::FOCUS_BALL )
    {
        Options::instance().setFocusType( ViewConfig::FOCUS_BALL );
        emit configured();
    }
}

/*-------------------------------------------------------------------*/
/*!

*/
void
ConfigDialog::toggleFocusBall()
{
    if ( Options::instance().focusType() == ViewConfig::FOCUS_BALL )
    {
        setFocusFix();
    }
    else
    {
        Options::instance().setFocusType( ViewConfig::FOCUS_BALL );
        M_focus_ball_rb->setChecked( true );

        emit configured();
    }
}

/*-------------------------------------------------------------------*/
/*!

*/
void
ConfigDialog::clickFocusPlayer()
{
    if ( Options::instance().focusType() != ViewConfig::FOCUS_PLAYER )
    {
        Options::instance().setFocusType( ViewConfig::FOCUS_PLAYER );
        emit configured();
    }
}

/*-------------------------------------------------------------------*/
/*!

*/
void
ConfigDialog::toggleFocusPlayer()
{
    if ( Options::instance().focusType() == ViewConfig::FOCUS_PLAYER )
    {
        setFocusFix();
    }
    else
    {
        Options::instance().setFocusType( ViewConfig::FOCUS_PLAYER );
        M_focus_player_rb->setChecked( true );

        emit configured();
    }
}

/*-------------------------------------------------------------------*/
/*!

*/
void
ConfigDialog::setFocusFix()
{
    if ( Options::instance().focusType() != ViewConfig::FOCUS_POINT )
    {
        Options::instance().setFocusType( ViewConfig::FOCUS_POINT );
        M_focus_fix_rb->setChecked( true );

        emit configured();
    }
}

/*-------------------------------------------------------------------*/
/*!

*/
void
ConfigDialog::setFocusPoint( const QPoint & point )
{
    Options::instance().setFocusPoint( point.x(), point.y() );
    M_focus_fix_rb->setChecked( true );

    emit configured();
}

/*-------------------------------------------------------------------*/
/*!

*/
void
ConfigDialog::clickSelectAutoAll()
{
    if ( Options::instance().playerSelectType() != ViewConfig::SELECT_AUTO_ALL )
    {
        Options::instance().setPlayerSelectType( ViewConfig::SELECT_AUTO_ALL );
        emit configured();
    }
}

/*-------------------------------------------------------------------*/
/*!

*/
void
ConfigDialog::toggleSelectAutoAll()
{
    if ( Options::instance().playerSelectType() == ViewConfig::SELECT_AUTO_ALL )
    {
        setUnselect();
    }
    else
    {
        Options::instance().setPlayerSelectType( ViewConfig::SELECT_AUTO_ALL );
        M_select_all_rb->setChecked( true );

        emit configured();
    }
}

/*-------------------------------------------------------------------*/
/*!

*/
void
ConfigDialog::clickSelectAutoLeft()
{
    ViewConfig::PlayerSelectType type = ( Options::instance().reverseSide()
                                          ? ViewConfig::SELECT_AUTO_RIGHT
                                          : ViewConfig::SELECT_AUTO_LEFT );

    if ( Options::instance().playerSelectType() != type )
    {
        Options::instance().setPlayerSelectType( type );
        emit configured();
    }
}

/*-------------------------------------------------------------------*/
/*!

*/
void
ConfigDialog::toggleSelectAutoLeft()
{
    ViewConfig::PlayerSelectType type = ( Options::instance().reverseSide()
                                          ? ViewConfig::SELECT_AUTO_RIGHT
                                          : ViewConfig::SELECT_AUTO_LEFT );

    if ( Options::instance().playerSelectType() == type )
    {
        setUnselect();
    }
    else
    {
        Options::instance().setPlayerSelectType( type );
        M_select_left_rb->setChecked( true );

        emit configured();
    }
}

/*-------------------------------------------------------------------*/
/*!

*/
void
ConfigDialog::clickSelectAutoRight()
{
    ViewConfig::PlayerSelectType type = ( Options::instance().reverseSide()
                                          ? ViewConfig::SELECT_AUTO_LEFT
                                          : ViewConfig::SELECT_AUTO_RIGHT );

    if ( Options::instance().playerSelectType() != type )
    {
        Options::instance().setPlayerSelectType( type );
        emit configured();
    }
}

/*-------------------------------------------------------------------*/
/*!

*/
void
ConfigDialog::toggleSelectAutoRight()
{
    ViewConfig::PlayerSelectType type = ( Options::instance().reverseSide()
                                          ? ViewConfig::SELECT_AUTO_LEFT
                                          : ViewConfig::SELECT_AUTO_RIGHT );

    if ( Options::instance().playerSelectType() == type )
    {
        setUnselect();
    }
    else
    {
        Options::instance().setPlayerSelectType( type );
        M_select_right_rb->setChecked( true );

        emit configured();
    }
}

/*-------------------------------------------------------------------*/
/*!

*/
void
ConfigDialog::clickSelectFix()
{
    if ( Options::instance().playerSelectType() != ViewConfig::SELECT_FIX )
    {
        Options::instance().setPlayerSelectType( ViewConfig::SELECT_AUTO_RIGHT );
        emit configured();
    }
}

/*-------------------------------------------------------------------*/
/*!

*/
void
ConfigDialog::toggleSelectFix()
{
    if ( Options::instance().playerSelectType() == ViewConfig::SELECT_FIX )
    {
        setUnselect();
    }
    else
    {
        Options::instance().setPlayerSelectType( ViewConfig::SELECT_FIX );
        M_select_fix_rb->setChecked( true );

        emit configured();
    }
}

/*-------------------------------------------------------------------*/
/*!

*/
void
ConfigDialog::setUnselect()
{
    Options::instance().unselectPlayer();
    M_unselect_rb->setChecked( true );
    M_player_choice->setCurrentIndex( 0 );

    emit configured();
}

/*-------------------------------------------------------------------*/
/*!

*/
void
ConfigDialog::selectPlayer( int number )
{
    // left side:  number = [1,11]
    // right side:  number = [12,22]

    if ( number <= 0
         || 22 < number )
    {
        setUnselect();
        return;
    }

    int orig_number = number;

    if ( Options::instance().reverseSide() )
    {
        if ( number <= 11 )
        {
            number += 11;
        }
        else
        {
            number -= 11;
        }
    }

    Options::instance().setPlayerSelectType( ViewConfig::SELECT_FIX );
    if ( number <= 11 )
    {
        Options::instance().setSelectedNumber( rcsc::LEFT, number );
    }
    else
    {
        Options::instance().setSelectedNumber( rcsc::RIGHT, number - 11 );
    }

    M_player_choice->setCurrentIndex( orig_number );
    M_select_fix_rb->setChecked( true );

    emit configured();
}

/*-------------------------------------------------------------------*/
/*!

*/
void
ConfigDialog::choicePlayer( int number )
{
    int orig_number = number;
    if ( Options::instance().reverseSide() )
    {
        if ( number <= 11 )
        {
            number += 11;
        }
        else
        {
            number -= 11;
        }
    }

    if ( number <= 11
         && Options::instance().isSelectedPlayer( rcsc::LEFT, number ) )
    {
        return;
    }

    if ( number > 11
         && Options::instance().isSelectedPlayer( rcsc::RIGHT, number - 11 ) )
    {
        return;
    }

    selectPlayer( orig_number );
}

/*-------------------------------------------------------------------*/
/*!

*/
void
ConfigDialog::selectPlayerWithKey()
{
    QAction * action = qobject_cast< QAction * >( sender() );

    if ( ! action )
    {
        return;
    }

    int key = action->shortcut()[0];

    for ( int i = 1; i < 10; ++i )
    {
        if ( key == Qt::Key_0 + i )
        {
            selectPlayer( i );
            return;
        }
        else if (
#ifdef Q_WS_MAC
                 key == int( Qt::META + Qt::Key_0 + i )
#else
                 key == int( Qt::CTRL + Qt::Key_0 + i )
#endif
                 )
        {
            selectPlayer( i + 11 );
            return;
        }
    }

    if ( key == Qt::Key_0 )
    {
        selectPlayer( 10 );
        return;
    }

    if (
#ifdef Q_WS_MAC
         key == Qt::META + Qt::Key_0
#else
         key == Qt::CTRL + Qt::Key_0
#endif
         )
    {
        selectPlayer( 21 );
        return;
    }

    if ( key == Qt::Key_Minus )
    {
        selectPlayer( 11 );
        return;
    }

    if (
#ifdef Q_WS_MAC
        key == Qt::META + Qt::Key_Minus
#else
        key == Qt::CTRL + Qt::Key_Minus
#endif
        )
    {
        selectPlayer( 22 );
        return;
    }
}

/*-------------------------------------------------------------------*/
/*!

*/
void
ConfigDialog::editBallTraceStart( const QString & text )
{
    bool ok = true;
    long value = text.toLong( & ok );

    if ( ok )
    {
        Options::instance().setBallTraceStart( value );

        emit configured();
    }
}

/*-------------------------------------------------------------------*/
/*!

*/
void
ConfigDialog::editBallTraceEnd( const QString & text )
{
    bool ok = true;
    long value = text.toLong( & ok );

    if ( ok )
    {
        Options::instance().setBallTraceEnd( value );

        emit configured();
    }
}

/*-------------------------------------------------------------------*/
/*!

*/
void
ConfigDialog::clickBallTraceAll()
{
    const std::vector< MonitorViewPtr > & view
        = M_main_data.viewHolder().monitorViewCont();

    if ( ! view.empty() )
    {
        M_ball_trace_cb->setChecked( false );

        if ( Options::instance().ballTraceStart() == view.front()->cycle()
             && Options::instance().ballTraceEnd() == view.back()->cycle() )
        {
            M_ball_trace_start->setText( QString::number( 0 ) );
            M_ball_trace_end->setText( QString::number( 0 ) );

            Options::instance().setBallTraceStart( 0 );
            Options::instance().setBallTraceEnd( 0 );
        }
        else
        {
            M_ball_trace_start->setText( QString::number( view.front()->cycle() ) );
            M_ball_trace_end->setText( QString::number( view.back()->cycle() ) );

            Options::instance().setBallTraceStart( view.front()->cycle() );
            Options::instance().setBallTraceEnd( view.back()->cycle() );
        }

        if ( Options::instance().isBallAutoTrace() )
        {
            Options::instance().toggleBallAutoTrace();
        }

        emit configured();
    }
}

/*-------------------------------------------------------------------*/
/*!

*/
void
ConfigDialog::clickAutoBallTrace( bool checked )
{
    if ( Options::instance().isBallAutoTrace() != checked )
    {
        Options::instance().toggleBallAutoTrace();

        emit configured();
    }
}

/*-------------------------------------------------------------------*/
/*!

*/
void
ConfigDialog::editPlayerTraceStart( const QString & text )
{
    bool ok = true;
    long value = text.toLong( & ok );

    if ( ok )
    {
        Options::instance().setPlayerTraceStart( value );

        emit configured();
    }
}

/*-------------------------------------------------------------------*/
/*!

*/
void
ConfigDialog::editPlayerTraceEnd( const QString & text )
{
    bool ok = true;
    long value = text.toLong( & ok );

    if ( ok )
    {
        Options::instance().setPlayerTraceEnd( value );

        emit configured();
    }
}

/*-------------------------------------------------------------------*/
/*!

*/
void
ConfigDialog::clickPlayerTraceAll()
{
    const std::vector< MonitorViewPtr > & view
        = M_main_data.viewHolder().monitorViewCont();
    if ( ! view.empty() )
    {
        M_player_trace_cb->setChecked( false );


        if ( Options::instance().playerTraceStart() == view.front()->cycle()
             && Options::instance().playerTraceEnd() == view.back()->cycle() )
        {
            M_player_trace_start->setText( QString::number( 0 ) );
            M_player_trace_end->setText( QString::number( 0 ) );

            Options::instance().setPlayerTraceStart( 0 );
            Options::instance().setPlayerTraceEnd( 0 );
        }
        else
        {
            M_player_trace_start->setText( QString::number( view.front()->cycle() ) );
            M_player_trace_end->setText( QString::number( view.back()->cycle() ) );

            Options::instance().setPlayerTraceStart( view.front()->cycle() );
            Options::instance().setPlayerTraceEnd( view.back()->cycle() );
        }

        if ( Options::instance().isPlayerAutoTrace() )
        {
            Options::instance().togglePlayerAutoTrace();
        }

        emit configured();
    }
}

/*-------------------------------------------------------------------*/
/*!

*/
void
ConfigDialog::clickAutoPlayerTrace( bool checked )
{
    if ( Options::instance().isPlayerAutoTrace() != checked )
    {
        Options::instance().togglePlayerAutoTrace();

        emit configured();
    }
}

/*-------------------------------------------------------------------*/
/*!

*/
void
ConfigDialog::editAutoTraceStart( const QString & text )
{
    bool ok = true;
    long value = text.toLong( & ok );

    if ( ok )
    {
        Options::instance().setAutoTraceStart( value );

        emit configured();
    }
}

/*-------------------------------------------------------------------*/
/*!

*/
void
ConfigDialog::changeAutoTracePeriod( int value )
{
    if ( value >= 1 )
    {
        Options::instance().setAutoTracePeriod( value );

        emit configured();
    }
}

/*-------------------------------------------------------------------*/
/*!

*/
void
ConfigDialog::clickLinePointButton()
{
    Options::instance().toggleLineTrace();

    emit configured();
}

/*-------------------------------------------------------------------*/
/*!

*/
void
ConfigDialog::changeBallFutureCycle( int value )
{
    Options::instance().setBallFutureCycle( value );

    emit configured();
}

/*-------------------------------------------------------------------*/
/*!

*/
void
ConfigDialog::changePlayerFutureCycle( int value )
{
    Options::instance().setPlayerFutureCycle( value );

    emit configured();
}
