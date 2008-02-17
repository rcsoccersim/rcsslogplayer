/*
 *Header:
 *File: controler.h (for C++)
 *Author: Noda Itsuki
 *Date: 1998/ 1/24
 *EndHeader:
 */

/*
 *Copyright:

    Copyright (C) 1996-2000 Electrotechnical Laboratory.
    	Itsuki Noda, Yasuo Kuniyoshi and Hitoshi Matsubara.
    Copyright (C) 2000, 2001 RoboCup Soccer Server Maintainance Group.
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


#ifndef RCSSLOGPLAYER_CONTROLER_H
#define RCSSLOGPLAYER_CONTROLER_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#if !X_DISPLAY_MISSING
#include <X11/Intrinsic.h>

/*
 *===================================================================
 *Part: Controler Class
 *===================================================================
 */

class Controler {
public:
		XtAppContext	app_context;			/* application context */
		Widget			top;					/* top widget */
		Widget			main;					/* main widget */
		Widget			status;				/* status widget */
		Widget			buttons;				/* buttons widget */
		Widget			time;					/* time widget */
		Widget			quit;
		Widget			rew;
		Widget			rev;
		Widget			minus;
		Widget			stop;
		Widget			plus;
		Widget			play;
		Widget			ff;
		Widget			record_buttons;		/* record buttons widget */
		Widget			rec;					/* record button widget */
		Widget			blank;					/* record button widget */
		Widget			jump;
		Widget			input;
		Widget			go;
		Widget			speed;

    ~Controler();

		void			init(int argc, char **argv);
		void			assign();
		void			display_time(int);
		void			display_status();
		void			change_recmode();
		void			buttons_reset();
		void			rew_button_reset_true();
		void			rew_button_reset_false();
		void			rev_button_reset_true();
		void			rev_button_reset_false();
		void			minus_button_reset();
		void			stop_button_reset_true();
		void			stop_button_reset_false();
		void			plus_button_reset();
		void			play_button_reset_true();
		void			play_button_reset_false();
		void			for_button_reset_true();
		void			for_button_reset_false();
		void			blank_button_reset();
		void run();
};

#endif //!X_DISPLAY_MISSING

#endif
