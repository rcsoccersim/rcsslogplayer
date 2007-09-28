/* -*-c++-*-
 *Header:
 *File: file.h (for C++)
 *Author: Tom Howard
 *Date: 2001/9/31
 *EndHeader:
 */

/*
 *Copyright:

 Copyright (C) 1996-2000 Electrotechnical Laboratory.
 Itsuki Noda, Yasuo Kuniyoshi and Hitoshi Matsubara.
 Copyright (C) 2000-2007 RoboCup Soccer Server Maintainance Group.
 Patrick Riley, Tom Howard, Daniel Polani, Itsuki Noda,
 Mikhail Prokopenko, Jan Wendler

 This file is a part of RCSSLogPlayer.

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

#ifndef RCSSLOGPLAYER_FILE_H
#define RCSSLOGPLAYER_FILE_H

#include <iostream>
#include <cstdio>

struct FileImpl;

class file {
private:
    FileImpl* M_impl;

public:

    file();
    ~file();

    void open( const char* path, int mode );
    void open( int fd, int mode );

    void setCompression( int level, int strategy );

    int read( void* buf, unsigned len );
    int write( void* buf, unsigned len );

    int flush( int type = 0 );

    long seek( long offset, int whence );
    long tell();
    long rewind();

    bool eof();

    int close();

    int compression();
    bool filtered();
    bool huffmanOnly();
    bool binary();
    bool read();
    bool write();

    bool open();

    void clearErr();

};

#endif
