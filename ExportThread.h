#ifndef EXPORTTHREAD_H
#define EXPORTTHREAD_H

//=================================================================================================
#include <QThread>
#include <QStringList>
#include <QDir>
//=================================================================================================
#define WRITE_BUFFER_SIZE (5242880) // 5Mb buffer
#define MAX_FILENAME 512
//=================================================================================================
typedef struct
{
  // Game settings
  QString gameName;
  QString gameSourceDir;
  QString gameTargetDir;
  QString orientation;
  QString zipDir;
  QString iconDir;
  QString api;

  // SDK
  QString SDK;
  QString NDK;
  QString JDK;
  QString ANT;

  // Keystore
  QString kKey;
  QString kAlias;
  QString kPassword;

  // AdMob
  QString aAppID;
  QString aPosition;
  QString aBanner;
} ExportInfo;
//=================================================================================================
class ExportThread : public QThread
{
  Q_OBJECT

  protected:
    ExportInfo exportInfo;
    QStringList dirsToRemove;
    QStringList filesToRemove;

  public:
    explicit ExportThread( const ExportInfo &info, QObject *parent = 0 );

  protected:
    QString checkSpaces( const QString &str ) const;
    void clearFiles();
    void clearDirs();
    void clearAll();
    bool removeDir( const QString &dirName );
    bool copyDir( const QString &src, const QString &dst );
    bool copyFile( const QString &src, const QString &dst );
    bool unzipAll( const QString &zipFile, const QString &dstDir, const QString &password = "" );
    bool unzipDir( const QString &zipFile, const QString &srcDir, const QString &dstDir, const QString &password = "" );
    bool unzipFile( const QString &zipFile, const QString &srcFile, const QString &dstDir, const QString &password = "" );
    bool zipFolder( const QString &zipFile, const QString &srcDir, bool root, const QString &password = "" );
    void recurseAddDir( const QDir &d, QStringList &list );
    bool createFile( const QString &fileName, const QStringList &content );
    QString readResFile( const QString &fileName );

  signals:
    void sendLog( const QString & );
};
//=================================================================================================

#endif // EXPORTTHREAD_H
