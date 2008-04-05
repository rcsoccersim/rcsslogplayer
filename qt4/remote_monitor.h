// -*-c++-*-

/*!
  \file remote_monitor.h
  \brief remote monitor class Header File.
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

#ifndef RCSSLOGPLAYER_REMOTE_MONITOR_H
#define RCSSLOGPLAYER_REMOTE_MONITOR_H

#include <QObject>
#include <QHostAddress>

#include <string>

namespace rcss {
namespace rcg {
struct DispInfoT;
}
}

class QUdpSocket;

class RemoteMonitor
    : public QObject {

    Q_OBJECT

private:

    bool M_alive;

    QHostAddress M_addr; //!< destination address
    quint16 M_port; //!< destination port
    QUdpSocket * M_socket;

    int M_version; // protocol version

    //! not used
    RemoteMonitor();
    RemoteMonitor( const RemoteMonitor & );
    RemoteMonitor & operator=( const RemoteMonitor & );
public:

    RemoteMonitor( QObject * parent,
                   const int version );

    ~RemoteMonitor();

    bool connect( const QHostAddress & addr,
                  const quint16 port );
    void disconnect();

    bool isConnected() const;

    const
    QHostAddress & addr() const
      {
          return M_addr;
      }

    quint16 port() const
      {
          return M_port;
      }

    int version() const
      {
          return M_version;
      }

    int send( const char * msg,
              const std::size_t len );

    int send( const rcss::rcg::DispInfoT & disp );

    void undedicatedRecv( const char * msg,
                          const int len );

private:

    void serializeDisp( const rcss::rcg::DispInfoT & disp,
                        std::string & msg );

    void processMsg( const char * msg,
                     const int len );

private slots:

    void handleReceive();

};

#endif
