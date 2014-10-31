#ifndef SDKWIDGET_H
#define SDKWIDGET_H

//=================================================================================================
#include <QWidget>
#include <QLineEdit>
//=================================================================================================
class QToolButton;
class QProgressBar;
class DownloadManager;
//=================================================================================================
class SDKWidget : public QWidget
{
  Q_OBJECT

  QString downloadTitle;
  QString downloadURL;
  DownloadManager *dm;

  QToolButton *tb;
  QToolButton *tbDownload;
  QProgressBar *progress;

  public:
    explicit SDKWidget( const QString &title , const QString &url, QWidget *parent = 0 );

    QLineEdit *path;

  private slots:
    void chooseDir();
    void downloadSDK();
    void downloadFinished();
    void setProgress( int cur, int total );
};
//=================================================================================================

#endif // SDKWIDGET_H
