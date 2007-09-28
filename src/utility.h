/*
 *Header:
 *File: utility.h (for C++ & cc)
 *Author: Noda Itsuki
 *Date: 1995/02/24
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
#include <cmath>

/*
 *ModifyHistory
 *
 *EndModifyHistory
 */

#define EPS 1.0e-10
#define INFINITE 1.0e10
#define PI M_PI
//#define max(x,y) ( ((x) > (y)) ? (x) : (y) )
//#define min(x,y) ( ((x) > (y)) ? (y) : (x) )
#define Abs(x)	(((x) > 0.0) ? (x) : (-(x)) )
#define Atan(y,x) (((x==0.0) && (y==0.0)) ? (0.0) : atan2((y),(x)))
#define RAD2DEG	(180.0/PI)
#define DEG2RAD (PI/180.0)

#define Rad2Deg(a) 	((double)(a) * RAD2DEG)
#define Rad2IDeg(a)	((int)(Rad2Deg(a)))
#define Rad2IDegRound(a)	((int)rint(Rad2Deg(a)))
#define Deg2Rad(a)	((double)(a) * DEG2RAD)

#define Quantize(v,q)	((rint((v)/(q)))*(q))

#define Pow(x)	((x)*(x))

#define RANDOMBASE		1000
#define IRANDOMBASE		31

#define drand(h,l)	(((((h)-(l)) * ((double)(random()%RANDOMBASE) / (double)RANDOMBASE))) + (l))
#define	irand(x)	((random() / IRANDOMBASE) % (x))

#ifdef __cplusplus
extern "C" {
#endif

extern unsigned int randomize(void) ;
/*
extern double drand(double l, double h) ;
*/
extern double normalize_angle(double ang) ;

extern int lcm(int a, int b);

#ifdef __cplusplus
}
#endif

#ifdef X11R5
# define mySetArg_1st(slot,val)	XtSetArg(Args[0], slot, (val)); Argn = 1
# define mySetArg(slot,val)	XtSetArg(Args[Argn], slot, (val)); Argn++
#endif


#define nstonl(x) htonl((Int32)(((double)(Int16)ntohs(x) / SHOWINFO_SCALE) * SHOWINFO_SCALE2))
#define nltons(x) htons((Int16)(((double)(Int32)ntohl(x) / SHOWINFO_SCALE2) * SHOWINFO_SCALE))
