// -*-c++-*-

/*!
  \file image_save_dialog.cpp
  \brief Image save dialog class Source File.
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <QtGui>

#include "image_save_dialog.h"

#include "options.h"
#include "main_data.h"
#include "main_window.h"
#include "field_canvas.h"
#include "disp_holder.h"

#include <iostream>
#include <cassert>
#include <cstdlib>

/*-------------------------------------------------------------------*/
/*!

*/
ImageSaveDialog::ImageSaveDialog( MainWindow * main_window,
                                  FieldCanvas * field_canvas,
                                  MainData & main_data )
    : QDialog( main_window )
    , M_main_window( main_window )
    , M_field_canvas( field_canvas )
    , M_main_data( main_data )

{
    assert( main_window );

    this->setWindowTitle( tr( "Image Save" ) );

    createControls();

}

/*-------------------------------------------------------------------*/
/*!

*/
ImageSaveDialog::~ImageSaveDialog()
{
    //std::cerr << "delete ImageSaveDialog" << std::endl;
}

/*-------------------------------------------------------------------*/
/*!

*/
void
ImageSaveDialog::createControls()
{
    QVBoxLayout * layout = new QVBoxLayout();
    layout->setSizeConstraint( QLayout::SetFixedSize );

    layout->addWidget( createCycleSelectControls(),
                       0, Qt::AlignLeft );

    layout->addWidget( createFileNameControls(),
                       0, Qt::AlignLeft );

    layout->addWidget( createDirSelectControls(),
                       0, Qt::AlignLeft );

    layout->addLayout( createExecuteControls(),
                       0 );

    this->setLayout( layout );
}

/*-------------------------------------------------------------------*/
/*!

*/
QWidget *
ImageSaveDialog::createCycleSelectControls()
{
    QGroupBox * group_box = new QGroupBox( tr( "Cycle Range" ) );

    QHBoxLayout * layout = new QHBoxLayout();
    layout->setSpacing( 0 );

    int min_cycle = 0;
    int max_cycle = 1000000;
//     const std::vector< DispPtr > & disp_cont = M_main_data.dispHolder().dispInfoCont();
//     if ( ! disp_cont.empty() )
//     {
//         min_cycle = disp_cont.front()->show_.time_;
//         max_cycle = disp_cont.back()->show_.time_;
//     }

    layout->addWidget( new QLabel( tr( "Start: " ) ),
                       0, Qt::AlignVCenter );

    M_start_cycle = new QSpinBox();
    M_start_cycle->setRange( min_cycle, max_cycle );
    M_start_cycle->setValue( min_cycle );
    layout->addWidget( M_start_cycle,
                       0, Qt::AlignVCenter );

    layout->addWidget( new QLabel( tr( " - End: " ) ),
                       0, Qt::AlignVCenter );

    M_end_cycle = new QSpinBox();
    M_end_cycle->setRange( min_cycle, max_cycle );
    M_end_cycle->setValue( min_cycle );
    layout->addWidget( M_end_cycle,
                       0, Qt::AlignVCenter );

    layout->addSpacing( 4 );

    QPushButton * select_all_btn = new QPushButton( tr( "Select All" ) );
    select_all_btn->setMaximumSize( 80, this->fontMetrics().height() + 12 );
    connect( select_all_btn, SIGNAL( clicked() ),
             this, SLOT( selectAllCycle() ) );
    layout->addWidget( select_all_btn,
                       0, Qt::AlignVCenter );

    group_box->setLayout( layout );
    return group_box;
}

/*-------------------------------------------------------------------*/
/*!

*/
QWidget *
ImageSaveDialog::createFileNameControls()
{
    QGroupBox * group_box = new QGroupBox( tr( "File" ) );
    QVBoxLayout * top_layout = new QVBoxLayout();

    {
        QHBoxLayout * layout = new QHBoxLayout();
        layout->setSpacing( 0 );

        layout->addWidget( new QLabel( tr( "Name Prefix: " ) ),
                           0, Qt::AlignVCenter | Qt::AlignLeft );

        M_name_prefix = new QLineEdit( tr( "image-" ) );
        if ( ! Options::instance().gameLogFile().empty() )
        {
            QFileInfo file_info( QString::fromStdString( Options::instance().gameLogFile() ) );
            M_name_prefix->setText( file_info.baseName() + tr( "-" ) );
        }
        int min_width = M_name_prefix->fontMetrics().width( M_name_prefix->text() ) + 32;
        min_width = qMin( 300, min_width );
        min_width = qMax( 80, min_width );
        M_name_prefix->setMinimumWidth( min_width );
        M_name_prefix->setMaximumHeight( 24 );

        layout->addWidget( M_name_prefix,
                           0, Qt::AlignVCenter | Qt::AlignLeft );

        top_layout->addLayout( layout );
    }
    {
        QHBoxLayout * layout = new QHBoxLayout();
        layout->setSpacing( 0 );

        layout->addWidget( new QLabel( tr( " Format: " ) ),
                           0, Qt::AlignVCenter  | Qt::AlignLeft );

        const QString default_format( "PNG" );

        M_format_choice = new QComboBox();
        {
            int i = 0;
            int default_index = 0;
            int max_width = 0;
            Q_FOREACH( QByteArray format, QImageWriter::supportedImageFormats() )
            {
                QString text = tr( "%1" ).arg( QString( format ).toUpper() );
                int width = this->fontMetrics().width( text );
                if ( max_width < width )
                {
                    max_width = width;
                }

                if ( text == default_format )
                {
                    default_index = i;
                }
                ++i;
                M_format_choice->addItem( text );
            }
            M_format_choice->setCurrentIndex( default_index );
            M_format_choice->setMaximumWidth( max_width + 40 );
        }
        layout->addWidget( M_format_choice,
                           0, Qt::AlignVCenter | Qt::AlignLeft );

        top_layout->addLayout( layout );
    }

    group_box->setLayout( top_layout );
    return group_box;;
}

/*-------------------------------------------------------------------*/
/*!

*/
QWidget *
ImageSaveDialog::createDirSelectControls()
{
    QGroupBox * group_box = new QGroupBox( tr( "Saved Directry" ) );

    QHBoxLayout * layout = new QHBoxLayout();

    QString dir_str;
    if ( ! Options::instance().gameLogFile().empty() )
    {
        QFileInfo file_info( QString::fromStdString( Options::instance().gameLogFile() ) );
        QDir file_dir = file_info.absoluteDir();
        if ( ! file_dir.exists() )
        {
            file_dir.mkdir( file_info.absolutePath() );
        }

        if ( file_dir.exists() )
        {
            dir_str = file_dir.absolutePath();
        }
    }
    else
    {
        dir_str = QDir::currentPath();
    }

    M_saved_dir = new QLineEdit( dir_str );
    M_saved_dir->setMinimumWidth( qMax( 360,
                                        M_saved_dir->fontMetrics().width( dir_str ) + 32 ) );
    layout->addWidget( M_saved_dir,
                       0, Qt::AlignVCenter );

    QPushButton * button = new QPushButton( tr( "..." ) );
    button->setMaximumSize( this->fontMetrics().width( tr( "..." ) ) + 12,
                            this->fontMetrics().height() + 12 );
    button->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
    connect( button, SIGNAL( clicked() ),
             this, SLOT( selectSavedDir() ) );
    layout->addWidget( button );

    group_box->setLayout( layout );

    return group_box;
}

/*-------------------------------------------------------------------*/
/*!

*/
QLayout *
ImageSaveDialog::createExecuteControls()
{
    QHBoxLayout * layout = new QHBoxLayout();

    QPushButton * button = new QPushButton( tr( "Create!" ) );
    connect( button, SIGNAL( clicked() ),
             this, SLOT( executeSave() ) );
    layout->addWidget( button,
                       10, Qt::AlignVCenter );

    QPushButton * cancel = new QPushButton( tr( "Cancel" ) );
    connect( cancel, SIGNAL( clicked() ),
             this, SLOT( reject() ) );
    layout->addWidget( cancel,
                       2, Qt::AlignVCenter );

    return layout;
}

/*-------------------------------------------------------------------*/
/*!

*/
void
ImageSaveDialog::showEvent( QShowEvent * event )
{
//     int min_cycle = 0;
//     int max_cycle = 1000000;
//     const std::vector< DispPtr > & disp_cont = M_main_data.dispHolder().dispInfoCont();
//     if ( ! disp_cont.empty() )
//     {
//         min_cycle = disp_cont.front()->show_.time_;
//         max_cycle = disp_cont.back()->show_.time_;
//     }
//     M_start_cycle->setRange( min_cycle, max_cycle );
//     M_end_cycle->setRange( min_cycle, max_cycle );

    event->accept();
}

/*-------------------------------------------------------------------*/
/*!

*/
void
ImageSaveDialog::selectAllCycle()
{
    int min_cycle = 0;
    int max_cycle = 0;

    const std::vector< DispPtr > & disp_cont = M_main_data.dispHolder().dispInfoCont();
    if ( ! disp_cont.empty() )
    {
        min_cycle = disp_cont.front()->show_.time_;
        max_cycle = disp_cont.back()->show_.time_;
    }

    M_start_cycle->setRange( min_cycle, max_cycle );
    M_end_cycle->setRange( min_cycle, max_cycle );

    M_start_cycle->setValue( min_cycle );
    M_end_cycle->setValue( max_cycle );
}

/*-------------------------------------------------------------------*/
/*!

*/
void
ImageSaveDialog::selectSavedDir()
{
    QString dir
        = QFileDialog::getExistingDirectory( this,
                                             tr( "Choose a saved directory" ),
                                             M_saved_dir->text(),
                                             QFileDialog::ShowDirsOnly
                                             | QFileDialog::DontResolveSymlinks);
    if ( ! dir.isEmpty() )
    {
        M_saved_dir->setText( dir );
    }
}

/*-------------------------------------------------------------------*/
/*!

*/
void
ImageSaveDialog::executeSave()
{
    if ( M_saved_dir->text().isEmpty() )
    {
        QMessageBox::warning( this,
                              tr( "Error" ),
                              tr( "Empty directry name!" ) );
        return;
    }

    QString format = M_format_choice->currentText();

    saveImage( M_start_cycle->value(),
               M_end_cycle->value(),
               M_saved_dir->text(),
               M_name_prefix->text(),
               M_format_choice->currentText() );
}

/*-------------------------------------------------------------------*/
/*!

*/
void
ImageSaveDialog::saveImage( const int start_cycle,
                            const int end_cycle,
                            const QString & saved_dir,
                            const QString & name_prefix,
                            const QString & format_name )
{
    if ( M_main_data.dispHolder().dispInfoCont().empty() )
    {
        QMessageBox::warning( this,
                              tr( "Error" ),
                              tr( "No Display Info!" ) );
        reject();
        return;
    }

    QString format = format_name.toLower();

    const int backup_index = M_main_data.index();

    const int first = M_main_data.dispHolder().getIndexOf( start_cycle );
    const int last = M_main_data.dispHolder().getIndexOf( end_cycle );

    if ( first > last )
    {
        QMessageBox::warning( this,
                              tr( "Error" ),
                              tr( "Invalid cycle range!" ) );
        return;
    }

    // create file path base
    QString file_path = saved_dir;

    if ( ! file_path.endsWith( QChar( '/' ) ) )
    {
        file_path += tr( "/" );
    }

    // check directory
    {
        QDir dir( file_path );
        if ( ! dir.exists()
             && ! dir.mkpath( file_path ) )
        {
            QMessageBox::warning( this,
                                  tr( "Error" ),
                                  tr( "Failed to create an image save directory!" ) );
            return;
        }
    }

    M_main_window->setEnabled( false );

    {
        QString name_prefix_trim = name_prefix;
        while ( ! name_prefix_trim.isEmpty()
                && name_prefix_trim.endsWith( QChar( '/' ) ) )
        {
            name_prefix_trim.remove( name_prefix_trim.length() - 1, 1 );
        }

        file_path += name_prefix_trim;
    }

    QString file_ext = tr( "." ) + format;

    QImage image( M_field_canvas->size(), QImage::Format_RGB32 );

    std::cerr << "Saved image resolution = "
              << M_field_canvas->size().width() << " x "
              << M_field_canvas->size().height() << std::endl;

    QPainter painter( &image );

    // show progress dialog
    QProgressDialog progress_dialog( this );
    progress_dialog.setWindowTitle( tr( "Image Save Progress" ) );
    progress_dialog.setRange( first, last );
    progress_dialog.setValue( first );
    progress_dialog.setLabelText( file_path + tr( "00000" ) + file_ext );

    bool confirm = true;
    bool success = true;
    int counter = 0;;

    // main loop
    for ( int i = first; i <= last; ++i, ++counter )
    {
        // full file path
        char count[16];
        snprintf( count, 16, "%05d", i );

        QString file_path_all = file_path;
        file_path_all += QString::fromAscii( count );
        file_path_all += file_ext;

        if ( confirm
             && QFile::exists( file_path_all ) )
        {
            int result
                = QMessageBox::question( this,
                                         tr( "Overwrite?" ),
                                         tr( "There already exists a file called %1.\n Overwrite?")
                                         .arg( file_path_all ),
                                         QMessageBox::No,
                                         QMessageBox::Yes,
                                         QMessageBox::YesAll );
            if ( result == QMessageBox::No )
            {
                progress_dialog.cancel();
                success = false;
                break;
            }
            else if ( result == QMessageBox::YesAll )
            {
                confirm = false;
            }
        }

        progress_dialog.setValue( i );
        progress_dialog.setLabelText( file_path_all );

        if ( counter == 20 )
        {
            counter = 0;
            qApp->processEvents();
            if ( progress_dialog.wasCanceled() )
            {
                success = false;
                break;
            }
        }

        M_main_data.setIndex( i );
        M_field_canvas->draw( painter );

        if ( ! image.save( file_path_all, format.toAscii() ) )
        {
            QMessageBox::critical( this,
                                   tr( "Error" ),
                                   tr( "Failed to save image file " )
                                   + file_path_all );
            success = false;
            break;
        }
    }

    M_main_data.setIndex( backup_index );

    M_main_window->setEnabled( true );

    if ( success )
    {
        accept();
    }
}
