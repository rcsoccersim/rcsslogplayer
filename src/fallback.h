/*
 *Header:
 *File: fallback.h (for C++)
 *Author: Noda Itsuki
 *Date: 1996/11/28
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


/*
 *===================================================================
 *Part: fallback resources
 *===================================================================
 */
#define FALLBACK_RESOURCES {\
			"*background: gray75",\
			"*time.label: time",\
			"*time.height: 20",\
			"*time.width: 184",\
			"*time.background: ivory2",\
			"*time.borderColor: ivory4",\
			"*status.label: status",\
			"*status.height: 20",\
			"*status.width: 214",\
			"*status.fromVert: time", \
			"*buttons.background: black",\
			"*rew.borderColor: green",\
			"*rev.borderColor: blue",\
			"*minus.borderColor: red",\
			"*stop.borderColor: red",\
			"*plus.borderColor: red",\
			"*play.borderColor: blue",\
			"*ff.borderColor: green",\
			"*quit.borderColor: black",\
			"*quit.background: navy",\
			"*quit.foreground: white",\
			"*record.background: black",\
			"*rec.foreground: red",\
			"*rec.borderColor: orange",\
			"*blank.borderColor: orange",\
			"*quit.fromHoriz: time",\
			"*buttons.fromVert: status",\
			"*rev.fromHoriz: rew",\
			"*minus.fromHoriz: rev",\
			"*stop.fromHoriz: minus",\
			"*plus.fromHoriz: stop",\
			"*play.fromHoriz: plus",\
			"*ff.fromHoriz: play",\
			"*record.fromVert: buttons",\
			"*rec.fromHoriz: blank",\
			"*jump.fromVert: buttons",\
			"*jump.horizDistance: 74",\
			"*jump.background: black",\
			"*input.editType: edit",\
			"*input.height: 20",\
			"*input.width: 104",\
			"*input.translations: #override\
				<Key>Return:	com_jump() \\n\
				Ctrl<Key>M:		com_jump()",\
			"*go.fromHoriz: input",\
			"*go.borderColor: purple",\
			NULL }

