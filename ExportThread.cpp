#include <QApplication>
#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QDebug>

#include "libs/quazip-0.7/quazip.h"
#include "libs/quazip-0.7/quazipfile.h"
#include "PathUtils.h"
#include "ExportThread.h"

//=================================================================================================
ExportThread::ExportThread( const ExportInfo &info, QObject *parent ):
  QThread( parent ),
  exportInfo( info )
{
}
//=================================================================================================
QString ExportThread::checkSpaces( const QString &str ) const
{
  QString res = PathUtils::nativePath( str );

  #ifdef WIN32
    if (res.contains(" "))
      return "\"" + res + "\"";
    else
      return res;
  #else
    if (res.contains(" "))
      return res.replace( " ", "\ " );
    else
      return res;
#endif
}
//=================================================================================================
void ExportThread::clearFiles()
{
  for (int i = 0; i < filesToRemove.count(); ++i)
    QFile( PathUtils::nativePath( filesToRemove.at(i) ) ).remove();

  filesToRemove.clear();
}
//=================================================================================================
void ExportThread::clearDirs()
{
  for (int i = 0; i < dirsToRemove.count(); ++i)
    removeDir( PathUtils::nativePath( dirsToRemove.at(i) ) );

  dirsToRemove.clear();
}
//=================================================================================================
void ExportThread::clearAll()
{
  clearFiles();
  clearDirs();
}
//=================================================================================================
bool ExportThread::removeDir( const QString &dirName )
{
  QDir dir( PathUtils::nativePath( dirName ) );

  if (dir.exists( dirName ))
  {
    foreach (QFileInfo info, dir.entryInfoList( QDir::NoDotAndDotDot | QDir::System | QDir::Hidden  | QDir::AllDirs | QDir::Files, QDir::DirsFirst ))
    {
      if (info.isDir())
        removeDir( PathUtils::nativePath( info.absoluteFilePath() ) );
      else
        QFile::remove( PathUtils::nativePath( info.absoluteFilePath() ) );
    }

    dir.rmdir( PathUtils::nativePath( dirName ) );

    return true;
  }

  return false;
}
//=================================================================================================
bool ExportThread::copyDir( const QString &src, const QString &dst )
{
  QDir dir( PathUtils::nativePath( src ) );

  if (!dir.exists())
    return false;

  QDir dst_dir( PathUtils::nativePath( dst ) );

  if (!dst_dir.exists())
    QDir().mkpath( PathUtils::nativePath( dst ) );

  foreach (QString d, dir.entryList( QDir::Dirs | QDir::NoDotAndDotDot ))
  {
    QString dst_path = PathUtils::nativePath( dst + "/" + d );
    dir.mkpath( dst_path );
    copyDir( PathUtils::nativePath( src + "/" + d ), dst_path );
  }

  foreach (QString f, dir.entryList( QDir::Files ))
    QFile::copy( PathUtils::nativePath( src + "/" + f ), PathUtils::nativePath( dst + "/" + f ) );

  return true;
}
//=================================================================================================
bool ExportThread::copyFile( const QString &src, const QString &dst )
{
  return QFile( src ).copy( dst );
}
//=================================================================================================
bool ExportThread::unzipAll( const QString &zipFile, const QString &dstDir , const QString &password )
{
  QuaZip zip( zipFile );

  if (zip.open( QuaZip::mdUnzip ))
  {
    for (bool more = zip.goToFirstFile(); more; more = zip.goToNextFile())
    {
      QString fileName = zip.getCurrentFileName();

      if (PathUtils::nativePath( fileName ).endsWith( PathUtils::nativePath("/") ))
        QDir().mkpath( PathUtils::nativePath( dstDir + "/" + fileName ) );
      else
      {
        QuaZipFile zFile( zip.getZipName(), fileName );

        if (!zFile.open( QIODevice::ReadOnly, password.isEmpty() ? NULL : password.toLatin1().constData() ))
        {
          zip.close();
          return false;
        }

        QByteArray ba = zFile.readAll();
        zFile.close();

        FILE *out = NULL;

        if (dstDir.endsWith( PathUtils::nativePath("/") ))
          out = fopen( PathUtils::nativePath( dstDir + fileName ).toLatin1().constData(), "wb" );
        else
          out = fopen( PathUtils::nativePath( dstDir + "/" + fileName ).toLatin1().constData(), "wb" );

        if (out == NULL)
        {
          zip.close();
          return false;
        }

        fwrite( ba.constData(), ba.size(), 1, out );
        fclose( out );
      }
    }

    zip.close();

    return true;
  }

  return false;
}
//=================================================================================================
bool ExportThread::unzipDir( const QString &zipFile, const QString &srcDir, const QString &dstDir, const QString &password )
{
  QuaZip zip( zipFile );

  if (zip.open( QuaZip::mdUnzip ))
  {
    QString sourceDir = PathUtils::nativePath( srcDir ).endsWith( PathUtils::nativePath("/") ) ? srcDir : PathUtils::nativePath( srcDir + "/" );

    while (sourceDir.startsWith( PathUtils::nativePath("/") ))
      sourceDir = sourceDir.mid(1);

    int srcDirDelimPos = sourceDir.lastIndexOf( PathUtils::nativePath("/"), -2 );

    QString destinationDir = PathUtils::nativePath( dstDir ).endsWith( PathUtils::nativePath("/") ) ? dstDir : PathUtils::nativePath( dstDir + "/" );

    for (bool more = zip.goToFirstFile(); more; more = zip.goToNextFile())
    {
      QString target = PathUtils::nativePath( zip.getCurrentFileName() );

      if (target.mid( 0, sourceDir.length() ).compare( sourceDir ))
        continue;

      if (srcDirDelimPos > 0)
        target = target.mid( srcDirDelimPos + 1 );

      if (target.endsWith( PathUtils::nativePath("/") ))
        QDir().mkpath( PathUtils::nativePath( destinationDir + target ) );
      else
      {
        QuaZipFile zFile( zip.getZipName(), zip.getCurrentFileName() );

        if (!zFile.open( QIODevice::ReadOnly, password.isEmpty() ? NULL : password.toLatin1().constData() ))
        {
          zip.close();
          return false;
        }

        QByteArray ba = zFile.readAll();
        zFile.close();

        FILE *out = fopen( PathUtils::nativePath( destinationDir + target ).toLatin1().constData(), "wb" );

        if (out == NULL)
        {
          zip.close();
          return false;
        }

        fwrite( ba.constData(), ba.size(), 1, out );
        fclose( out );
      }
    }

    zip.close();

    return true;
  }

  return false;
}
//=================================================================================================
bool ExportThread::unzipFile( const QString &zipFile, const QString &srcFile, const QString &dstDir, const QString &password )
{
  QuaZip zip( zipFile );

  if (zip.open( QuaZip::mdUnzip ))
  {
    QString destinationDir = PathUtils::nativePath( dstDir ).endsWith( PathUtils::nativePath("/") ) ? dstDir : PathUtils::nativePath( dstDir + "/" );

    for (bool more = zip.goToFirstFile(); more; more = zip.goToNextFile())
    {
      QString target = PathUtils::nativePath( zip.getCurrentFileName() );

      if (target.compare( PathUtils::nativePath( srcFile ) ))
        continue;

      if (!target.endsWith( PathUtils::nativePath("/") ))
      {
        QuaZipFile zFile( zip.getZipName(), zip.getCurrentFileName() );

        if (!zFile.open( QIODevice::ReadOnly, password.isEmpty() ? NULL : password.toLatin1().constData() ))
        {
          zip.close();
          return false;
        }

        QByteArray ba = zFile.readAll();
        zFile.close();

        int srcFileDelimPos = srcFile.lastIndexOf( PathUtils::nativePath("/") );

        if (srcFileDelimPos > 0)
          target = target.mid( srcFileDelimPos + 1 );

        FILE *out = fopen( PathUtils::nativePath( destinationDir + target ).toLatin1().constData(), "wb" );

        if (out == NULL)
        {
          zip.close();
          return false;
        }

        fwrite( ba.constData(), ba.size(), 1, out );
        fclose( out );

        zip.close();

        return true;
      }
    }

    zip.close();
  }

  return false;
}
//=================================================================================================
bool ExportThread::zipFolder( const QString &zipFile, const QString &srcDir, bool root , const QString &password )
{
  if (!QDir( srcDir ).exists())
    return false;

  QuaZip zip( zipFile );

  if (!zip.open(QuaZip::mdCreate))
    return false;

  QFile inFile;

  // Получаем список файлов и папок рекурсивно
  QStringList sl;
  recurseAddDir( srcDir, sl );

  // Создаем массив состоящий из QFileInfo объектов
  QFileInfoList files;

  foreach (QString fn, sl)
    files << QFileInfo( fn );

  QuaZipFile outFile( &zip );

  char c;

  foreach (QFileInfo fileInfo, files)
  {
    if (!fileInfo.isFile())
      continue;

    // Если файл в поддиректории, то добавляем имя этой поддиректории к именам файлов
    // например: fileInfo.filePath() = "D:\Work\Sources\SAGO\svn\sago\Release\tmp_DOCSWIN\Folder\123.opn"
    // тогда после удаления части строки fileNameWithSubFolders будет равен "Folder\123.opn" и т.д.
    QString fileNameWithRelativePath = fileInfo.filePath().remove( 0, QDir( srcDir ).absolutePath().length() + 1 );

    inFile.setFileName( fileInfo.filePath() );

    if (!inFile.open( QIODevice::ReadOnly ))
      return false;

    if (!outFile.open( QIODevice::WriteOnly, QuaZipNewInfo( fileNameWithRelativePath, fileInfo.filePath() ) ))
      return false;

    while (inFile.getChar( &c ) && outFile.putChar(c));

    if (outFile.getZipError() != UNZ_OK)
      return false;

    outFile.close();

    if (outFile.getZipError() != UNZ_OK)
      return false;

    inFile.close();
  }

  zip.close();

  if (zip.getZipError() != 0)
    return false;

  return true;
}
//=================================================================================================
void ExportThread::recurseAddDir( const QDir &d, QStringList &list )
{
  QStringList qsl = d.entryList( QDir::NoDotAndDotDot | QDir::Dirs | QDir::Files );

  foreach (QString file, qsl)
  {
    QFileInfo finfo( QString("%1/%2").arg( d.path() ).arg( file ) );

    if (finfo.isSymLink())
      return;

    if (finfo.isDir() && !exportInfo.excludes.contains( finfo.baseName() ))
    {
      QString dirname = finfo.fileName();
      QDir sd( finfo.filePath() );

      recurseAddDir( sd, list );
    }
    else
      list << PathUtils::nativePath( finfo.filePath() );
  }
}
//=================================================================================================
bool ExportThread::createFile( const QString &fileName, const QStringList &content )
{
  QFile dp( PathUtils::nativePath( fileName ) );

  if (!dp.open( QIODevice::WriteOnly ))
    return false;

  QTextStream tsdp( &dp );
  tsdp.setAutoDetectUnicode( true );
  tsdp.setGenerateByteOrderMark( false );

  for (int i = 0; i < content.count(); ++i)
    tsdp << content.at(i);

  dp.close();

  return true;
}
//=================================================================================================
QString ExportThread::readResFile( const QString &fileName )
{
  QString res = "";

  QFile f( QString(":icons/%1").arg( fileName ) );

  if (!f.open( QIODevice::ReadOnly ))
    return res;

  QTextStream ts( &f );
  ts.setAutoDetectUnicode( true );
  ts.setGenerateByteOrderMark( false );
  res = ts.readAll();
  f.close();

  return res;
}
//=================================================================================================
