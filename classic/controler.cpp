/* -*- Mode: C++ -*-
 *Header:
 *File: window.C
 *Auther: Noda Itsuki
 *Date: 1996/12/1
 *EndHeader:
 */

/*
 *Copyright:

 Copyright (C) 1996-2000 Electrotechnical Laboratory.
 Itsuki Noda, Yasuo Kuniyoshi and Hitoshi Matsubara.
 Copyright (C) 2000-2007 RoboCup Soccer Server Maintainance Group.
 Patrick Riley, Tom Howard, Daniel Polani, Itsuki Noda,
 Mikhail Prokopenko, Jan Wendler

 This file is a part of SoccerServer.

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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#if !X_DISPLAY_MISSING

#include "controler.h"

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <cstring>


#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Xaw/Command.h>
#include <X11/Xaw/Form.h>
#include <X11/Xaw/Toggle.h>
#include <X11/Xaw/AsciiText.h>
#include <X11/Xaw/Scrollbar.h>
#include <X11/Shell.h>

#include "logplayer.h"
#include "fallback.h"

#include "ff.xbm"
#include "play.xbm"
#include "plus.xbm"
#include "stop.xbm"
#include "minus.xbm"
#include "rev.xbm"
#include "rew.xbm"
#include "rec.xbm"
#include "blank.xbm"
#include "quit.xbm"
#include "go.xbm"

extern Player player;

#ifdef X11R5
//# include "utility.h"
static Arg Args[16];
static int Argn;
#endif

#define	rootwin(x)	RootWindow(XtDisplay(x), XtWindow(x))

static
void
com_rewind()
{
    player.comRewind();
}

static
void
com_reverse()
{
    player.comReverse();
}

static
void
com_minus()
{
    player.comMinus();
}

static
void
com_stop()
{
    player.comStop();
}

static
void
com_play()
{
    player.comPlay();
}

static
void
com_forward()
{
    player.comForward();
}

static
void
com_plus()
{
    player.comPlus();
}

static
void
com_record()
{
    player.comRecord();
}

static
void
com_blank()
{
    player.comBlank();
}

static
void
com_jump()
{
    player.comJump();
}

static
void
com_quit()
{
    player.quit();
}


static XtActionsRec actions[] = {
    {"com_jump", (XtActionProc)com_jump}
};


/*
 *========================================================================
 *Part: Controler
 *========================================================================
 */

Controler::~Controler()
{
    //std::cerr << "delete controler" << std::endl;
}

void
Controler::init( int argc, char *argv[] )
{
    static String fallback_resources[] = FALLBACK_RESOURCES;

    top = XtAppInitialize( &app_context, "top", NULL, 0, &argc, argv,
                           fallback_resources, NULL, 0 );

    assign();

    XtRealizeWidget(top);
    buttons_reset();
}


void
Controler::assign()
{
#ifdef X11R5
    main = XtCreateManagedWidget( "main", formWidgetClass, top, Args, 0 );

    time = XtCreateManagedWidget( "time", labelWidgetClass, main, Args, 0 );

    status = XtCreateManagedWidget( "status", labelWidgetClass, main, Args, 0 );

    quit = XtCreateManagedWidget( "quit", toggleWidgetClass, main, Args, 0 );
    Pixmap quit_pixmap = XCreateBitmapFromData( XtDisplay( quit ),
                                                rootwin( quit ),
                                               (const char*)quit_bits, quit_width, quit_height );
    mySetArg_1st( XtNbitmap, (XtArgVal)(XtArgVal)quit_pixmap );
    XtSetValues( quit, Args, Argn );
    XtAddCallback( quit, XtNcallback,
                   (XtCallbackProc)com_quit, (XtPointer)this );

    buttons = XtCreateManagedWidget( "buttons", formWidgetClass, main, Args, 0 );

    rew = XtCreateManagedWidget( "rew", toggleWidgetClass, buttons, Args, 0 );
    Pixmap rew_pixmap = XCreateBitmapFromData( XtDisplay( rew ), rootwin( rew ),
                                               (const char*)rew_bits, rew_width, rew_height );
    mySetArg_1st( XtNbitmap, (XtArgVal)(XtArgVal)rew_pixmap );
    XtSetValues( rew, Args, Argn );
    XtAddCallback( rew, XtNcallback,
                   (XtCallbackProc)com_rewind, (XtPointer)this );

    rev = XtCreateManagedWidget( "rev", toggleWidgetClass, buttons, Args, 0 );
    Pixmap rev_pixmap = XCreateBitmapFromData( XtDisplay( rev ), rootwin( rev ),
                                               (const char*)rev_bits, rev_width, rev_height );
    mySetArg_1st( XtNbitmap, (XtArgVal)rev_pixmap );
    XtSetValues( rev, Args, Argn );
    XtAddCallback( rev, XtNcallback,
                   (XtCallbackProc)com_reverse, (XtPointer)this );

    minus = XtCreateManagedWidget( "minus", toggleWidgetClass, buttons, Args, 0 );
    Pixmap minus_pixmap = XCreateBitmapFromData( XtDisplay( minus ), rootwin( minus ),
                                                 (const char*)minus_bits, minus_width, minus_height );
    mySetArg_1st( XtNbitmap, (XtArgVal)minus_pixmap );
    XtSetValues( minus, Args, Argn );
    XtAddCallback( minus, XtNcallback,
                   (XtCallbackProc)com_minus, (XtPointer)this );

    stop = XtCreateManagedWidget( "stop", toggleWidgetClass, buttons, Args, 0 );
    Pixmap stop_pixmap = XCreateBitmapFromData( XtDisplay( stop ), rootwin( stop ),
                                                (const char*)stop_bits, stop_width, stop_height );
    mySetArg_1st( XtNbitmap, (XtArgVal)stop_pixmap );
    XtSetValues( stop, Args, Argn );
    XtAddCallback( stop, XtNcallback,
                   (XtCallbackProc)com_stop, (XtPointer)this );

    plus = XtCreateManagedWidget( "plus", toggleWidgetClass, buttons, Args, 0 );
    Pixmap plus_pixmap = XCreateBitmapFromData( XtDisplay( plus ), rootwin( plus ),
                                                (const char*)plus_bits, plus_width, plus_height );
    mySetArg_1st( XtNbitmap, (XtArgVal)plus_pixmap );
    XtSetValues( plus, Args, Argn );
    XtAddCallback( plus, XtNcallback,
                   (XtCallbackProc)com_plus, (XtPointer)this );

    play = XtCreateManagedWidget( "play", toggleWidgetClass, buttons, Args, 0 );
    Pixmap play_pixmap = XCreateBitmapFromData( XtDisplay( play ), rootwin( play ),
                                               (const char*)play_bits, play_width, play_height);
    mySetArg_1st( XtNbitmap, (XtArgVal)play_pixmap );
    XtSetValues( play, Args, Argn );
    XtAddCallback( play, XtNcallback,
                   (XtCallbackProc)com_play, (XtPointer)this );

    ff = XtCreateManagedWidget( "ff", toggleWidgetClass, buttons, Args, 0 );
    Pixmap ff_pixmap = XCreateBitmapFromData( XtDisplay( ff ), rootwin( ff ),
                                              (const char*)ff_bits, ff_width, ff_height );
    mySetArg_1st( XtNbitmap, (XtArgVal)ff_pixmap );
    XtSetValues( ff, Args, Argn );
    XtAddCallback( ff, XtNcallback,
                   (XtCallbackProc)com_forward, (XtPointer)this );

    if ( Play->hasOutputFile() )
    {
        record_buttons = XtCreateManagedWidget( "record",
                                                formWidgetClass, main, Args, 0 );

        blank = XtCreateManagedWidget( "blank", toggleWidgetClass,
                                       record_buttons, Args, 0 );
        Pixmap blank_pixmap =
            XCreateBitmapFromData( XtDisplay( blank ), rootwin( blank ),
                                   (const char*)blank_bits, blank_width, blank_height );
        mySetArg_1st( XtNbitmap, (XtArgVal)blank_pixmap );
        XtSetValues( blank, Args, Argn );
        XtAddCallback( blank, XtNcallback,
                       (XtCallbackProc)com_blank, (XtPointer)this );

        rec = XtCreateManagedWidget( "rec", toggleWidgetClass,
                                     record_buttons, Args, 0 );
        Pixmap rec_pixmap = XCreateBitmapFromData( XtDisplay(rec), rootwin(rec),
                                                   (const char*)rec_bits, rec_width, rec_height );
        mySetArg_1st( XtNbitmap, (XtArgVal)rec_pixmap );
        XtSetValues( rec, Args, Argn );
        XtAddCallback( rec, XtNcallback,
                       (XtCallbackProc)com_record, (XtPointer)this );
    }
    jump = XtCreateManagedWidget( "jump", formWidgetClass, main, Args, 0 );

    mySetArg_1st( XtNeditType, "edit" );
    input = XtCreateManagedWidget( "input",
                                   asciiTextWidgetClass, jump, Args, Argn );

    go = XtCreateManagedWidget( "go", commandWidgetClass, jump, Args, 0 );
    Pixmap go_pixmap = XCreateBitmapFromData( XtDisplay(go), rootwin(go),
                                              (const char*)go_bits, go_width, go_height );
    mySetArg_1st( XtNbitmap, (XtArgVal)go_pixmap );
    XtSetValues( go, Args, Argn );
    XtAddCallback( go, XtNcallback,
                   (XtCallbackProc)com_jump, (XtPointer)this );

#else

    main = XtVaCreateManagedWidget( "main", formWidgetClass, top, NULL );

    time = XtVaCreateManagedWidget( "time", labelWidgetClass, main, NULL );

    status = XtVaCreateManagedWidget( "status", labelWidgetClass, main, NULL );

    quit = XtVaCreateManagedWidget( "quit", toggleWidgetClass, main, NULL );
    Pixmap quit_pixmap = XCreateBitmapFromData( XtDisplay( quit ), rootwin( quit ),
                                                (const char*)quit_bits, quit_width, quit_height );
    XtVaSetValues( quit, XtNbitmap, (XtArgVal)quit_pixmap, NULL );
    XtAddCallback( quit, XtNcallback,
                   (XtCallbackProc)com_quit, (XtPointer)this );

    buttons = XtVaCreateManagedWidget( "buttons", formWidgetClass, main, NULL );

    rew = XtVaCreateManagedWidget( "rew", toggleWidgetClass, buttons, NULL );
    Pixmap rew_pixmap = XCreateBitmapFromData( XtDisplay(rew), rootwin(rew),
                                               (const char*)rew_bits, rew_width, rew_height );
    XtVaSetValues( rew, XtNbitmap, (XtArgVal)rew_pixmap, NULL );
    XtAddCallback( rew, XtNcallback,
                   (XtCallbackProc)com_rewind, (XtPointer)this );

    rev = XtVaCreateManagedWidget( "rev", toggleWidgetClass, buttons, NULL );
    Pixmap rev_pixmap = XCreateBitmapFromData( XtDisplay(rev), rootwin(rev),
                                               (const char*)rev_bits, rev_width, rev_height );
    XtVaSetValues( rev, XtNbitmap, (XtArgVal)rev_pixmap, NULL );
    XtAddCallback( rev, XtNcallback,
                   (XtCallbackProc)com_reverse, (XtPointer)this );

    minus = XtVaCreateManagedWidget( "minus", toggleWidgetClass, buttons, NULL );
    Pixmap minus_pixmap = XCreateBitmapFromData( XtDisplay( minus ),rootwin( minus ),
                                                 (const char*)minus_bits, minus_width, minus_height );
    XtVaSetValues( minus, XtNbitmap, (XtArgVal)minus_pixmap, NULL );
    XtAddCallback( minus, XtNcallback,
                   (XtCallbackProc)com_minus, (XtPointer)this );

    stop = XtVaCreateManagedWidget( "stop", toggleWidgetClass, buttons, NULL );
    Pixmap stop_pixmap = XCreateBitmapFromData( XtDisplay( stop ), rootwin( stop ),
                                                (const char*)stop_bits, stop_width, stop_height );
    XtVaSetValues( stop, XtNbitmap, (XtArgVal)stop_pixmap, NULL );
    XtAddCallback( stop, XtNcallback,
                   (XtCallbackProc)com_stop, (XtPointer)this );

    plus = XtVaCreateManagedWidget( "plus", toggleWidgetClass, buttons, NULL );
    Pixmap plus_pixmap = XCreateBitmapFromData( XtDisplay( plus ), rootwin( plus ),
                                                (const char*)plus_bits, plus_width, plus_height );
    XtVaSetValues( plus, XtNbitmap, (XtArgVal)plus_pixmap, NULL );
    XtAddCallback( plus, XtNcallback,
                   (XtCallbackProc)com_plus, (XtPointer)this );

    play = XtVaCreateManagedWidget( "play", toggleWidgetClass, buttons, NULL );
    Pixmap play_pixmap = XCreateBitmapFromData( XtDisplay( play ), rootwin( play ),
                                                (const char*)play_bits, play_width, play_height );
    XtVaSetValues( play, XtNbitmap, (XtArgVal)play_pixmap, NULL );
    XtAddCallback( play, XtNcallback,
                   (XtCallbackProc)com_play, (XtPointer)this );

    ff = XtVaCreateManagedWidget( "ff", toggleWidgetClass, buttons, NULL );
    Pixmap ff_pixmap = XCreateBitmapFromData( XtDisplay( ff ), rootwin( ff ),
                                             (const char*)ff_bits, ff_width, ff_height );
    XtVaSetValues( ff, XtNbitmap, (XtArgVal)ff_pixmap, NULL );
    XtAddCallback( ff, XtNcallback,
                   (XtCallbackProc)com_forward, (XtPointer)this );

    if ( player.hasOutputFile() )
    {
        record_buttons = XtVaCreateManagedWidget( "record", formWidgetClass,
                                                  main, NULL );
        blank = XtVaCreateManagedWidget( "blank", toggleWidgetClass,
                                         record_buttons, NULL );
        Pixmap blank_pixmap
            = XCreateBitmapFromData( XtDisplay( blank ), rootwin( blank ),
                                     (const char*)blank_bits, blank_width, blank_height );
        XtVaSetValues( blank, XtNbitmap, (XtArgVal)blank_pixmap, NULL );
        XtAddCallback( blank, XtNcallback,
                       (XtCallbackProc)com_blank, (XtPointer)this );

        rec = XtVaCreateManagedWidget( "rec", toggleWidgetClass,
                                       record_buttons, NULL );
        Pixmap rec_pixmap = XCreateBitmapFromData( XtDisplay( rec ), rootwin( rec ),
                                                   (const char*)rec_bits, rec_width, rec_height);
        XtVaSetValues( rec, XtNbitmap, (XtArgVal)rec_pixmap, NULL );
        XtAddCallback( rec, XtNcallback,
                       (XtCallbackProc)com_record, (XtPointer)this );

    }

    jump = XtVaCreateManagedWidget( "jump", formWidgetClass, main, NULL );

    // TODO: cannot input the value on MacOS X
    input = XtVaCreateManagedWidget( "input", asciiTextWidgetClass, jump,
                                     XtNeditType, "edit", NULL );

    go = XtVaCreateManagedWidget( "go", commandWidgetClass, jump, NULL );
    Pixmap go_pixmap = XCreateBitmapFromData( XtDisplay( go ), rootwin( go ),
                                              (const char*)go_bits, go_width, go_height );
    XtVaSetValues( go, XtNbitmap, (XtArgVal)go_pixmap, NULL );

    XtAddCallback( go, XtNcallback,
                   (XtCallbackProc)com_jump, (XtPointer)this );
#endif
    XtAppAddActions( app_context, actions, XtNumber( actions ) );
}

void
Controler::display_time( int t )
{
    char buf[256];
    snprintf( buf, sizeof( buf ), "time: %5d", t );
#ifndef X11R5
    XtVaSetValues( time, XtNlabel, buf, NULL );
#else
    mySetArg_1st( XtNlabel, buf );
    XtSetValues( time, Args, Argn );
#endif
}

void
Controler::display_status()
{
    std::string msg = player.statusString();
#ifndef X11R5
    XtVaSetValues( status, XtNlabel, msg.c_str(), NULL );
#else
    mySetArg_1st( XtNlabel, msg.c_str() );
    XtSetValues( status, Args, Argn );
#endif
}

void
Controler::buttons_reset()
{
    if ( player.state() == Player::STATE_REW )
    {
        rew_button_reset_true();
    }
    else
    {
        rew_button_reset_false();
    }

    if ( player.state() == Player::STATE_REVERSE )
    {
        rev_button_reset_true();
    }
    else
    {
        rev_button_reset_false();
    }

    minus_button_reset();

    if ( player.state() == Player::STATE_STOP
         || player.state() == Player::STATE_WAIT
         || player.state() == Player::STATE_JUMP
         || player.state() == Player::STATE_NOT_JUMP )
    {
        stop_button_reset_true();
    }
    else
    {
        stop_button_reset_false();
    }

    plus_button_reset();

    if ( player.state() == Player::STATE_PLAY )
    {
        play_button_reset_true();
    }
    else
    {
        play_button_reset_false();
    }

    if ( player.state() == Player::STATE_FOR )
    {
        for_button_reset_true();
    }
    else
    {
        for_button_reset_false();
    }

    if ( player.hasOutputFile() )
    {
        if ( player.state() != Player::STATE_BLANK )
        {
            blank_button_reset();
        }
    }
}

void
Controler::rew_button_reset_true()
{
#ifdef X11R5
    mySetArg_1st( XtNstate, True );
    XtSetValues( rew, Args, Argn );
#else
    XtVaSetValues( rew, XtNstate, True, NULL );
#endif
}

void
Controler::rew_button_reset_false()
{
#ifdef X11R5
    mySetArg_1st( XtNstate, False );
    XtSetValues( rew, Args, Argn );
#else
    XtVaSetValues( rew, XtNstate, False, NULL );
#endif
}

void
Controler::rev_button_reset_true()
{
#ifdef X11R5
    mySetArg_1st( XtNstate, True );
    XtSetValues( rev, Args, Argn );
#else
    XtVaSetValues( rev, XtNstate, True, NULL );
#endif
}

void
Controler::rev_button_reset_false()
{
#ifdef X11R5
    mySetArg_1st( XtNstate, False );
    XtSetValues( rev, Args, Argn );
#else
    XtVaSetValues( rev, XtNstate, False, NULL );
#endif
}

void
Controler::minus_button_reset()
{
#ifdef X11R5
    mySetArg_1st( XtNstate, False );
    XtSetValues( minus, Args, Argn );
#else
    XtVaSetValues( minus, XtNstate, False, NULL );
#endif
}

void Controler::stop_button_reset_true()
{
#ifdef X11R5
    mySetArg_1st( XtNstate, True );
    XtSetValues( stop, Args, Argn );
#else
    XtVaSetValues( stop, XtNstate, True, NULL );
#endif
}

void
Controler::stop_button_reset_false()
{
#ifdef X11R5
    mySetArg_1st( XtNstate, False );
    XtSetValues( stop, Args, Argn );
#else
    XtVaSetValues( stop, XtNstate, False, NULL );
#endif
}

void
Controler::plus_button_reset()
{
#ifdef X11R5
    mySetArg_1st( XtNstate, False );
    XtSetValues( plus, Args, Argn );
#else
    XtVaSetValues( plus, XtNstate, False, NULL );
#endif
}

void
Controler::play_button_reset_true()
{
#ifdef X11R5
    mySetArg_1st( XtNstate, True );
    XtSetValues( play, Args, Argn );
#else
    XtVaSetValues( play, XtNstate, True, NULL );
#endif
}

void
Controler::play_button_reset_false()
{
#ifdef X11R5
    mySetArg_1st( XtNstate, False );
    XtSetValues( play, Args, Argn );
#else
    XtVaSetValues( play, XtNstate, False, NULL );
#endif
}

void
Controler::for_button_reset_true()
{
#ifdef X11R5
    mySetArg_1st( XtNstate, True );
    XtSetValues( ff, Args, Argn );
#else
    XtVaSetValues( ff, XtNstate, True, NULL );
#endif
}

void
Controler::for_button_reset_false()
{
#ifdef X11R5
    mySetArg_1st( XtNstate, False );
    XtSetValues( ff, Args, Argn );
#else
    XtVaSetValues( ff, XtNstate, False, NULL );
#endif
}

void
Controler::blank_button_reset()
{
#ifdef X11R5
    mySetArg_1st( XtNstate, False );
    XtSetValues( blank, Args, Argn );
#else
    XtVaSetValues( blank, XtNstate, False, NULL );
#endif
}

void
Controler::run()
{
    XtAppMainLoop( app_context );
}


//////////////////////////////////////////////////////////



void
Player::comRewind()
{
    M_to_time = 0;

    if ( M_state != STATE_REW )
    {
        M_limit = PLAY_CYCLE/2;
    }
    else
    {
        M_limit = std::max( TIMEDELTA, M_limit/2 ); //M_limit - TIMEDELTA );
    }

    if ( M_state != STATE_WAIT )
    {
        M_state = STATE_REW;
    }

    M_controler->display_status();
    M_controler->buttons_reset();
}

void
Player::comReverse()
{
    if ( M_state != STATE_WAIT )
    {
        M_state = STATE_REVERSE;
    }
    M_to_time = 0;
    M_controler->display_status();
    M_controler->buttons_reset();
}

void
Player::comMinus()
{
    if ( M_state != STATE_WAIT )
    {
        M_state = STATE_MINUS;
    }
    M_to_time = 0;
    M_controler->display_status();
    M_controler->buttons_reset();
}

void
Player::comStop()
{
    if ( M_state != STATE_WAIT )
    {
        M_state = STATE_STOP;
    }
    M_to_time = END;
    M_controler->display_status();
    M_controler->buttons_reset();
}

void
Player::comPlay()
{
    if ( M_state != STATE_WAIT )
    {
        M_state = STATE_PLAY;
    }
    M_to_time = END;
    M_controler->display_status();
    M_controler->buttons_reset();
}

void
Player::comPlus()
{
    if ( M_state != STATE_WAIT )
    {
        M_state = STATE_PLUS;
    }
    M_to_time = END;
    M_controler->display_status();
    M_controler->buttons_reset();
}

void
Player::comForward()
{
    M_to_time = END;

    if ( M_state != STATE_FOR )
    {
        M_limit = PLAY_CYCLE/2;
    }
    else
    {
        M_limit = std::max( TIMEDELTA, M_limit/2 ); //M_limit - TIMEDELTA );
    }

    if ( M_state != STATE_WAIT )
    {
        M_state = STATE_FOR;
    }

    M_controler->display_status();
    M_controler->buttons_reset();
}

void
Player::comRecord()
{
    if ( M_rec_state == REC_OFF )
    {
        M_rec_state = REC_ON;
    }
    else if ( M_rec_state == REC_ON )
    {
        M_rec_state = REC_OFF;
    }
}

void
Player::comBlank()
{
    if ( M_state != STATE_WAIT )
    {
        M_state = STATE_BLANK;
    }

    M_controler->display_status();
    M_controler->buttons_reset();
}

void
Player::comJump()
{
    String str;
    char *s, *t;

#ifdef X11R5
    mySetArg_1st(XtNstring, &str);
    XtGetValues( M_controler->input, Args, Argn );
#else
    XtVaGetValues( M_controler->input, XtNstring, &str, NULL );
#endif

    s = t = (char *)str;
    for ( ; ; ++s )
    {
        if ( *s == '\0' || s == (char)NULL )
        {
            break;
        }

        if (*s == '\n') t = s + 1;
    }

    if ( M_state != STATE_WAIT )
    {
        if ( std::sscanf( t, "%d", &M_to_time ) == 1 )
        {
            std::cerr << "scanned to_time=" << M_to_time << std::endl;
            M_state = STATE_JUMP;
        }
        else if ( ! std::strncmp( t, "end", std::strlen( "end" ) ) )
        {
            M_to_time = END;
            M_state = STATE_JUMP;
        }
        else
        {
            M_state = STATE_NOT_JUMP;
        }

        M_controler->display_status();
        M_controler->buttons_reset();
    }
#ifdef X11R5
    mySetArg_1st( XtNstring, "" );
    XtSetValues( M_controler->input, Args, Argn );
#else
    XtVaSetValues( M_controler->input, XtNstring, "", NULL );
#endif
}

#endif //!X_DISPLAY_MISSING
