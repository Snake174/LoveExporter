#include <QApplication>
#include <QLabel>
#include <QToolButton>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QProgressBar>
#include <QUrl>

#include "DownloadManager.h"
#include "SDKWidget.h"

//=================================================================================================
SDKWidget::SDKWidget( const QString &title, const QString &url, QWidget *parent ):
  QWidget( parent ),
  downloadTitle( title ),
  downloadURL( url )
{
  dm = new DownloadManager( this );
  connect( dm, SIGNAL(finished()), this, SLOT(downloadFinished()) );
  connect( dm, SIGNAL(progress(int,int)), this, SLOT(setProgress(int,int)) );

  QHBoxLayout *hl = new QHBoxLayout( this );

  path = new QLineEdit( this );

  tb = new QToolButton( this );
  tb->setText("...");
  tb->setToolTip( tr("Choose SDK location") );
  connect( tb, SIGNAL(clicked()), this, SLOT(chooseDir()) );

  tbDownload = new QToolButton( this );
  tbDownload->setIconSize( QSize( 16, 16 ) );
  tbDownload->setIcon( QIcon(":icons/download.png") );
  tbDownload->setToolTip( QString( tr("Download (%1)") ).arg( downloadURL ) );
  connect( tbDownload, SIGNAL(clicked()), this, SLOT(downloadSDK()) );

  if (!title.compare("JDK"))
    tbDownload->setEnabled( false );

  progress = new QProgressBar( this );
  progress->setVisible( false );

  //hl->addWidget( new QLabel( downloadTitle, this ) );
  //hl->addSpacing( 10 );
  hl->addWidget( path );
  hl->addWidget( progress );
  hl->addSpacing( 10 );
  hl->addWidget( tb );
  hl->addWidget( tbDownload );
  hl->setMargin(5);
  hl->setSpacing(3);
}
//=================================================================================================
void SDKWidget::chooseDir()
{
  QFileDialog fd;
  fd.setFileMode( QFileDialog::DirectoryOnly );

  QString location = fd.getExistingDirectory( 0, tr("Select Location"), qApp->applicationDirPath() );

  if (!location.isEmpty())
    path->setText( location );
}
//=================================================================================================
void SDKWidget::downloadSDK()
{
  tb->setEnabled( false );
  tbDownload->setEnabled( false );
  path->setVisible( false );
  progress->setVisible( true );
  dm->append( QUrl( downloadURL ) );
}
//=================================================================================================
void SDKWidget::downloadFinished()
{
  tb->setEnabled( true );
  tbDownload->setEnabled( true );
  path->setVisible( true );
  progress->setVisible( false );
}
//=================================================================================================
void SDKWidget::setProgress(int cur, int total)
{
  progress->setRange( 0, total );
  progress->setValue( cur );
}
//=================================================================================================
