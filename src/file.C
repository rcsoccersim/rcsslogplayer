// -*-c++-*-

/*
 *Copyright:

 Copyright (C) 1996-2000 Electrotechnical Laboratory.
 Itsuki Noda, Yasuo Kuniyoshi and Hitoshi Matsubara.
 Copyright (C) 2000-2007 RoboCup Soccer Server Maintainance Group.

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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "file.h"

#ifdef HAVE_SSTREAM
#include <sstream>
#else
#include <strstream>
#endif


#ifdef HAVE_LIBZ
#include <zlib.h>
#else
#define Z_NO_COMPRESSION 0
#define Z_BEST_SPEED 1
#define Z_BEST_COMPRESSION 9
#define Z_DEFAULT_COMPRESSION (-1)

#define Z_DEFAULT_STRATEGY 0
#define Z_FILTERED 1
#define Z_HUFFMAN_ONLY 2
#endif

struct FileImpl {

    enum {
        DEFAULT_COMPRESSION = Z_DEFAULT_COMPRESSION,
        NO_COMPRESSION = Z_NO_COMPRESSION,
        BEST_SPEED = Z_BEST_SPEED,
        BEST_COMPRESSION = Z_BEST_COMPRESSION,
        COMPRESSION_MOD = 0x0010,
        DEFAULT_STRATEGY = Z_DEFAULT_STRATEGY * COMPRESSION_MOD,
        FILTERED = Z_FILTERED * COMPRESSION_MOD,
        HUFFMAN_ONLY = Z_HUFFMAN_ONLY * COMPRESSION_MOD,
        READ = 0x0100,
        WRITE = 0x0200,
        BINARY = 0x0800
    };


    FILE* fp;
#ifdef HAVE_LIBZ
    gzFile gzfp;
#endif
    bool f_open;
    bool gz_open;
    int _mode;

    FileImpl()
        : fp( NULL ),
#if HAVE_LIBX
          gzfp( NULL ),
#endif
          f_open( false ),
          gz_open( false ),
          _mode( 0 )
      { }
};


file::file()
    : M_impl( new FileImpl )
{

}

file::~file()
{
    close();
    if ( M_impl )
    {
        delete M_impl;
        M_impl = static_cast< FileImpl * >( 0 );
    }
}

void
file::open ( const char* path, int mode )
{
    M_impl->_mode = mode;
#ifdef HAVE_LIBZ
    if ( !filtered() && !huffmanOnly()
         && ( read() && write() )
         || ( write() && ( compression() == FileImpl::NO_COMPRESSION )  )
         )
    {
#endif
        // normal file
#ifdef HAVE_SSTREAM
        std::ostringstream mode_strm;
#else
        std::ostrstream mode_strm;
#endif

        if ( M_impl->_mode | FileImpl::READ == M_impl->_mode )
            mode_strm << "r";

        if ( M_impl->_mode | FileImpl::WRITE == M_impl->_mode )
            mode_strm << "w";

        if ( M_impl->_mode | FileImpl::BINARY == M_impl->_mode )
            mode_strm << "b";

#ifdef HAVE_SSTREAM
        M_impl->fp = fopen ( path, mode_strm.str().c_str() );
#else
        mode_strm << ends;
        M_impl->fp = std::fopen ( path, mode_strm.str() );
        mode_strm.freeze( false );
#endif

        if ( M_impl->fp == NULL )
            std::cerr << __FILE__ << ": " << __LINE__
                      << ": Error opening file: " << path << std::endl;
        else
        {
            M_impl->f_open = true;
        }
#ifdef HAVE_LIBZ
    }
    else if ( ( read() != write () && ( read() || write() ) )
              && ( ( filtered() && !huffmanOnly() )
                   || ( huffmanOnly() && !filtered() )
                   || ( !huffmanOnly() && !filtered() ) )
              )
    {
        // compressed file
#ifdef HAVE_SSTREAM
        std::ostringstream mode_strm;
#else
        std::ostrstream mode_strm;
#endif

        if ( M_impl->_mode | FileImpl::READ == M_impl->_mode )
            mode_strm << "rb";
        else
            mode_strm << "wb";

        mode_strm << compression();

        if ( filtered() )
            mode_strm << "f";
        else if ( huffmanOnly() )
            mode_strm << "h";

#ifdef HAVE_SSTREAM
        M_impl->gzfp = gzopen ( path, mode_strm.str().c_str() );
#else
        mode_strm << ends;
        M_impl->gzfp = gzopen ( path, mode_strm.str() );
        mode_strm.freeze( false );
#endif

        if ( M_impl->gzfp == NULL )
            std::cerr << __FILE__ << ": " << __LINE__
                      << ": Error opening file: " << path << std::endl;
        else
        {
            M_impl->gz_open = true;
        }
    }
    else
        std::cerr << __FILE__ << ": " << __LINE__
                  << ": Invalid mode\n";
#endif

}


void
file::open ( int fd, int mode )
{
    M_impl->_mode = mode;
#ifdef HAVE_LIBZ
    if ( !filtered() && !huffmanOnly()
         && ( read() && write() )
         || ( write() && compression() == FileImpl::NO_COMPRESSION )
         )
    {
#endif
        // normal file
#ifdef HAVE_SSTREAM
        std::ostringstream mode_strm;
#else
        std::ostrstream mode_strm;
#endif
        if ( M_impl->_mode | FileImpl::READ == M_impl->_mode )
            mode_strm << "r";

        if ( M_impl->_mode | FileImpl::WRITE == M_impl->_mode )
            mode_strm << "w";

        if ( M_impl->_mode | FileImpl::BINARY == M_impl->_mode )
            mode_strm << "b";

#ifdef HAVE_SSTREAM
        M_impl->fp = fdopen( fd, mode_strm.str().c_str() );
#else
        mode_strm << std::ends;
        M_impl->fp = fdopen( fd, mode_strm.str() );
        mode_strm.freeze( false );
#endif
        if ( M_impl->fp == NULL )
            std::cerr << __FILE__ << ": " << __LINE__
                      << ": Error opening file\n";
        else
            M_impl->f_open = true;
#ifdef HAVE_LIBZ
    }
    else if ( ( read () != write () && ( read () || write () ) )
              && ( ( filtered () && !huffmanOnly () )
                   || !huffmanOnly () && filtered () )
              )
    {
        // compressed file
#ifdef HAVE_SSTREAM
        std::ostringstream mode_strm;
#else
        std::ostrstream mode_strm;
#endif
        if ( M_impl->_mode | FileImpl::READ == M_impl->_mode )
            mode_strm << "rb";
        else
            mode_strm << "wb";

        mode_strm << compression ();

        if ( filtered () )
            mode_strm << "f";
        else if ( huffmanOnly () )
            mode_strm << "h";

#ifdef HAVE_SSTREAM
        M_impl->gzfp = gzdopen ( fd, mode_strm.str().c_str() );
#else
        mode_strm << std::ends;
        M_impl->gzfp = gzdopen ( fd, mode_strm.str() );
        mode_strm.freeze( false );
#endif
        if ( M_impl->gzfp == NULL )
            std::cerr << __FILE__ << ": " << __LINE__
                      << ": Error opening file\n";
        else
            M_impl->gz_open = true;
    }
    else
        std::cerr << __FILE__ << ": " << __LINE__
                  << ": Invalid mode\n";
#endif
}


void
file::setCompression ( int level, int strategy )
{
#ifdef HAVE_LIBZ
    gzsetparams ( M_impl->gzfp, level, strategy / FileImpl::COMPRESSION_MOD );
#endif
}

int
file::read ( void* buf, unsigned len )
{
#ifdef HAVE_LIBZ
    if ( M_impl->gz_open )
    {
        return gzread( M_impl->gzfp, buf, len );
    }
    else
#endif
        if ( M_impl->f_open )
        {
            return std::fread( buf, len, 1, M_impl->fp );
        }
        else
            return -1;
}

int
file::write( void* buf, unsigned len )
{
#ifdef HAVE_LIBZ
    if ( M_impl->gz_open )
        return ( gzwrite( M_impl->gzfp, buf, len ) );
    else
#endif
        if ( M_impl->f_open )
            return ( std::fwrite( buf, len, 1, M_impl->fp ) );
        else
            return -1;
}

int
file::flush( int type )
{
#ifdef HAVE_LIBZ
    if ( M_impl->gz_open )
        return ( gzflush( M_impl->gzfp, type ) );
    else
#endif
        if ( M_impl->f_open )
            return ( std::fflush( M_impl->fp ) );
        else
            return -1;
}

long
file::seek( long offset, int whence )
{
#ifdef HAVE_LIBZ
    if ( M_impl->gz_open )
        return ( gzseek( M_impl->gzfp, offset, whence ) );
    else
#endif
        if ( M_impl->f_open )
            return ( fseek( M_impl->fp, offset, whence ) );
        else
            return -1;
}

long
file::tell ()
{
#ifdef HAVE_LIBZ
    if ( M_impl->gz_open )
        return ( gztell( M_impl->gzfp ) );
    else
#endif
        if ( M_impl->f_open )
            return ( ftell( M_impl->fp ) );
        else
            return -1;
}

long
file::rewind()
{
#ifdef HAVE_LIBZ
    if ( M_impl->gz_open )
        return ( gzrewind( M_impl->gzfp ) );
    else
#endif
        if ( M_impl->f_open )
        {
            ::rewind( M_impl->fp );
            return ( tell() );
        }
        else
            return -1;
}

bool
file::eof()
{
#ifdef HAVE_LIBZ
    if ( M_impl->gz_open )
        return ( gzeof( M_impl->gzfp ) );
    else
#endif
        if ( M_impl->f_open )
            return ( feof( M_impl->fp ) );
        else
            return -1;
}

int
file::close()
{
#ifdef HAVE_LIBZ
    if ( M_impl->gz_open )
    {
        M_impl->gz_open = false;
        return ( gzclose( M_impl->gzfp ) );
    }
    else
#endif
        if ( M_impl->f_open )
        {
            M_impl->f_open = false;
            return ( feof( M_impl->fp ) );
        }
        else
            return -1;
}

int
file::compression ()
{
    return M_impl->_mode % FileImpl::COMPRESSION_MOD;
}

bool
file::filtered ()
{
    return ( M_impl->_mode | FileImpl::FILTERED ) == M_impl->_mode;
}

bool
file::huffmanOnly ()
{
    return ( M_impl->_mode | FileImpl::HUFFMAN_ONLY ) == M_impl->_mode;
}

bool
file::binary()
{
    return ( M_impl->_mode | FileImpl::BINARY ) == M_impl->_mode;
}

bool
file::read()
{
    return ( M_impl->_mode | FileImpl::READ ) == M_impl->_mode;
}

bool
file::write()
{
    return ( M_impl->_mode | FileImpl::WRITE ) == M_impl->_mode;
}

bool
file::open()
{
    return M_impl->f_open
#ifdef HAVE_LIBZ
        || M_impl->gz_open
#endif
        ;
}

void
file::clearErr()
{
    if ( M_impl->f_open )
        clearerr( M_impl->fp );
}
