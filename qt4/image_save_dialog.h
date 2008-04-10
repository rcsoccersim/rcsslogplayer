// -*-c++-*-

/*!
  \file image_save_dialog.h
  \brief Field Canvas image save control dialog class Header File.
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

#ifndef RCSSLOGPLAYER_IMAGE_SAVE_DIALOG_H
#define RCSSLOGPLAYER_IMAGE_SAVE_DIALOG_H

#include <QDialog>

class QComboBox;
class QLineEdit;
class QProgressDialog;
class QShowEvent;
class QSpinBox;

class FieldCanvas;
class MainData;
class MainWindow;

class ImageSaveDialog
    : public QDialog {

    Q_OBJECT

private:

    MainWindow * M_main_window;
    FieldCanvas * M_field_canvas;

    MainData & M_main_data;

    QSpinBox * M_start_cycle;
    QSpinBox * M_end_cycle;

    QLineEdit * M_name_prefix;
    QComboBox * M_format_choice;

    QLineEdit * M_saved_dir;

public:

    ImageSaveDialog( MainWindow * main_window,
                     FieldCanvas * field_canvas,
                     MainData & main_data );

    ~ImageSaveDialog();

private:

    void createControls();

    QWidget * createCycleSelectControls();
    QWidget * createFileNameControls();
    QWidget * createDirSelectControls();
    QLayout * createExecuteControls();

    void saveImage( const int start_cycle,
                    const int end_cycle,
                    const QString & saved_dir,
                    const QString & name_prefix,
                    const QString & format_name );

protected:

    void showEvent( QShowEvent * event );


private slots:

    void selectSavedDir();

public slots:

    void selectAllCycle();
    void executeSave();

};

#endif
