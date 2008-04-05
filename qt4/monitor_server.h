// -*-c++-*-

/*!
  \file monitor_server.h
  \brief monitor server class Header File.
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

#ifndef RCSSLOGPLAYER_MONITOR_SERVER_H
#define RCSSLOGPLAYER_MONITOR_SERVER_H

#include <QObject>

#include <vector>
#include <string>

class QHostAddress;
class QUdpSocket;

class MainData;
class RemoteMonitor;

namespace rcss {
namespace rcg {
struct DispInfoT;
}
}

class MonitorServer
    : public QObject {

    Q_OBJECT

private:

    const MainData & M_main_data;

    const quint16 M_port;
    QUdpSocket * M_socket;

    std::vector< RemoteMonitor * > M_monitors;

    //! not used
    MonitorServer();
    MonitorServer( const MonitorServer & );
    MonitorServer & operator=( const MonitorServer & );
public:

    MonitorServer( QObject * parent,
                   const MainData & main_data,
                   const int port );

    ~MonitorServer();

    bool isConnected() const;

private:

    void parseMonitorInit( const char * msg,
                           const QHostAddress & addr,
                           const quint16 port );

    void sendInit( RemoteMonitor & monitor );

    void serializeDisp( const rcss::rcg::DispInfoT & disp,
                        std::string & msg );

public slots:

    void sendToClients();

private slots:

    void handleReceive();
    void removeDisconnectedClient();

};

#endif
