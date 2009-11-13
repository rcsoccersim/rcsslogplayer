TEMPLATE = app
QT += network
TARGET = rcsslogplayer
DESTDIR = ../bin

DEPENDPATH += . ../rcsslogplayer

INCLUDEPATH += . ..
win32 {
  INCLUDEPATH +=  ../../boost
}
unix {
  INCLUDEPATH += /opt/local/include
}
macx {
  INCLUDEPATH += /opt/local/include
}

LIBS += -L../lib -lrcssrcgparser
win32 {
  LIBS += ../../zlib123-dll/zlib1.dll -lwsock32
}
unix {
  LIBS += -L/opt/local/lib
  LIBS += -lboost_program_options-mt -lz
}
macx {
  LIBS += -L/opt/local/lib
  LIBS += -lboost_program_options-mt -lz
}

DEFINES += HAVE_LIBZ
win32 {
  DEFINES += HAVE_WINDOWS_H
}
unix {
  DEFINES += HAVE_NETINET_IN_H
  DEFINES += HAVE_BOOST_PROGRAM_OPTIONS
}
macx {
  DEFINES += HAVE_NETINET_IN_H
  DEFINES += HAVE_BOOST_PROGRAM_OPTIONS
}
DEFINES += PACKAGE="\\\"rcsslogplayer\\\""
DEFINES += PACKAGE_NAME="\\\"rcsslogplayer\\\"" VERSION="\\\"14.0.1\\\""

CONFIG += qt warn_on release
win32 {
  CONFIG += windows
}

#QMAKE_LFLAGS_DEBUG += -static
#QMAKE_LFLAGS_DEBUG += -pg
#QMAKE_CFLAGS_DEBUG += -pg -static -DQABSTRACTSOCKET_DEBUG
#QMAKE_CXXFLAGS += -static
#QMAKE_CXXFLAGS_DEBUG += -pg -static -DQABSTRACTSOCKET_DEBUG

# Input
HEADERS += \
	angle_deg.h \
	ball_painter.h \
	circle_2d.h \
	config_dialog.h \
	detail_dialog.h \
	disp_holder.h \
	draw_info_painter.h \
	field_canvas.h \
	field_painter.h \
  image_save_dialog.h \
	line_2d.h \
	log_player.h \
	log_player_tool_bar.h \
	log_slider_tool_bar.h \
	main_data.h \
	main_window.h \
	monitor_client.h \
	monitor_server.h \
	options.h \
	player_painter.h \
	player_type_dialog.h \
	remote_monitor.h \
	score_board_painter.h \
	team_graphic.h \
	team_graphic_painter.h \
	vector_2d.h

SOURCES += \
	angle_deg.cpp \
	ball_painter.cpp \
	circle_2d.cpp \
	config_dialog.cpp \
	detail_dialog.cpp \
	disp_holder.cpp \
	draw_info_painter.cpp \
	field_canvas.cpp \
	field_painter.cpp \
  image_save_dialog.cpp \
	line_2d.cpp \
	log_player.cpp \
	log_player_tool_bar.cpp \
	log_slider_tool_bar.cpp \
	main.cpp \
	main_data.cpp \
	main_window.cpp \
	monitor_client.cpp \
	monitor_server.cpp \
	options.cpp \
	player_painter.cpp \
	player_type_dialog.cpp \
	remote_monitor.cpp \
	score_board_painter.cpp \
	team_graphic.cpp \
	team_graphic_painter.cpp \
	vector_2d.cpp

nodist_rcsslogplayer_SOURCES = \
	moc_config_dialog.cpp \
	moc_detail_dialog.cpp \
	moc_field_canvas.cpp \
  moc_image_save_dialog.cpp \
	moc_log_player.cpp \
	moc_log_player_tool_bar.cpp \
	moc_log_slider_tool_bar.cpp \
	moc_main_window.cpp \
	moc_monitor_client.cpp \
	moc_monitor_server.cpp \
	moc_player_type_dialog.cpp \
	moc_remote_monitor.cpp

win32 {
RC_FILE = rcsslogplayer.rc
}
macx {
ICON = ../icons/rcss.icns
}
