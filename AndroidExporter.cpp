#include <QApplication>
#include <QProcess>
#include <QTextCodec>
#include <QDir>

#include "PathUtils.h"
#include "AndroidExporter.h"

//=================================================================================================
AndroidExporter::AndroidExporter( const ExportInfo &info, QObject *parent ):
  ExportThread( info, parent )
{
  codec = QTextCodec::codecForLocale();
}
//=================================================================================================
void AndroidExporter::run()
{
  clearAll();

  // Название игры
  QString gameName = exportInfo.gameName;

  // Каталог с исходниками игры
  QString gameSourceDir = exportInfo.gameSourceDir;

  // Каталог, куда помещается готовая игра
  QString gameTargetDir = "";

  if (exportInfo.gameTargetDir.isEmpty())
    gameTargetDir = PathUtils::nativePath( qApp->applicationDirPath() + "/" + gameName + "/" );
  else
  {
    if (exportInfo.gameTargetDir.endsWith( PathUtils::nativePath("/") ))
      gameTargetDir = PathUtils::nativePath( exportInfo.gameTargetDir + gameName + "/" );
    else
      gameTargetDir = PathUtils::nativePath( exportInfo.gameTargetDir + "/" + gameName + "/" );
  }

  QDir().mkpath( gameTargetDir );

  // Необходимые файлы для игры
  QString zipLibsDir = PathUtils::nativePath( exportInfo.zipDir );

  sendLog("<b>Get build info</b> - <font color=#00CC00>OK</font>");
  sendLog("<b>Create build folder</b> - <font color=#00CC00>OK</font>");

  // Распаковка необходимых файлов
  bool copyStatus = copyDir( PathUtils::nativePath("data/android/love-android-sdl2"), gameTargetDir );

  if (!copyStatus)
  {
    sendLog("<b>Copy libraries</b> - <font color=#CC0000>FAIL</font>");
    clearAll();
    return;
  }

  copyStatus = copyDir( zipLibsDir, gameTargetDir + "libs" );

  if (!copyStatus)
  {
    sendLog("<b>Copy libraries</b> - <font color=#CC0000>FAIL</font>");
    clearAll();
    return;
  }

  QString srcDir = PathUtils::nativePath("data/android/new/org");

  if (!exportInfo.api.compare("10"))
    srcDir = PathUtils::nativePath("data/android/old/org");

  copyStatus = copyDir( srcDir, PathUtils::nativePath( gameTargetDir + "src/org" ) );

  if (!copyStatus)
  {
    sendLog("<b>Copy libraries</b> - <font color=#CC0000>FAIL</font>");
    clearAll();
    return;
  }

  sendLog("<b>Copy libraries</b> - <font color=#00CC00>OK</font>");

  // Создание game.love
  copyStatus = zipFolder( PathUtils::nativePath( gameTargetDir + "assets/game.love" ), gameSourceDir, false );

  if (!copyStatus)
  {
    sendLog("<b>Create game.love file</b> - <font color=#CC0000>FAIL</font>");
    clearAll();
    return;
  }

  sendLog("<b>Create game.love file</b> - <font color=#00CC00>OK</font>");

  // Create default.properties
  if (!createFile( gameTargetDir + "default.properties", QStringList() << "target=android-" + exportInfo.api ))
  {
    sendLog("<b>Create default.properties file</b> - <font color=#CC0000>FAIL</font>");
    clearAll();
    return;
  }

  sendLog("<b>Create default.properties file</b> - <font color=#00CC00>OK</font>");

  // Create project.properties
  if (!createFile( gameTargetDir + "project.properties", QStringList() << "target=android-" + exportInfo.api ))
  {
    sendLog("<b>Create project.properties file</b> - <font color=#CC0000>FAIL</font>");
    clearAll();
    return;
  }

  sendLog("<b>Create project.properties file</b> - <font color=#00CC00>OK</font>");

  // Create AndroidManifest.xml
  QString AndroidManifest = "";

  if (!exportInfo.api.compare("10"))
  {
    AndroidManifest = readResFile("AndroidManifestOld.xml");
    AndroidManifest
      .replace( "%GAME_NAME%", gameName )
      .replace( "%ORIENTATION%", exportInfo.orientation );
  }
  else
  {
    AndroidManifest = readResFile("AndroidManifestNew.xml");

    if (!exportInfo.api.compare("12"))
    {
      AndroidManifest
        .replace( "%GAME_NAME%", gameName )
        .replace( "%ORIENTATION%", exportInfo.orientation )
        .replace( "android:configChanges=\"orientation|screenSize\"", "" )
        .replace( "%API%", exportInfo.api );
    }
    else
    {
      AndroidManifest
        .replace( "%GAME_NAME%", gameName )
        .replace( "%ORIENTATION%", exportInfo.orientation )
        .replace( "%API%", exportInfo.api );
    }
  }

  if (!createFile( gameTargetDir + "AndroidManifest.xml", QStringList() << AndroidManifest ))
  {
    sendLog("<b>Create AndroidManifest.xml file</b> - <font color=#CC0000>FAIL</font>");
    clearAll();
    return;
  }

  sendLog("<b>Create AndroidManifest.xml file</b> - <font color=#00CC00>OK</font>");

  // Распаковка скриптов
  #ifdef WIN32
    QString scriptSrcFile = PathUtils::nativePath("data/tools/build-android.bat");
    QString scriptDstFile = gameTargetDir + "build-android.bat";
  #else
    QString scriptSrcFile = PathUtils::nativePath("data/tools/build-android.sh");
    QString scriptDstFile = gameTargetDir + "build-android.sh";
  #endif

  copyStatus = copyFile( scriptSrcFile, scriptDstFile );

  if (!copyStatus)
  {
    sendLog("<b>Copy scripts</b> - <font color=#CC0000>FAIL</font>");
    clearAll();
    return;
  }

  sendLog("<b>Copy scripts</b> - <font color=#00CC00>OK</font>");

  sendLog("<b>Create executable file:</b>");

  // Создание исполняемого файла
  {
    QProcess *p = new QProcess();
    p->setWorkingDirectory( gameTargetDir );
    p->setProcessChannelMode( QProcess::MergedChannels );
    p->setEnvironment( QProcess::systemEnvironment() );

    QStringList params;
    params << PathUtils::nativePath( exportInfo.ANT );
    params << PathUtils::nativePath( exportInfo.JDK );
    params << PathUtils::nativePath( exportInfo.SDK );
    params << PathUtils::nativePath( exportInfo.SDK );
    params << PathUtils::nativePath( exportInfo.NDK );

    connect( p, SIGNAL(readyReadStandardOutput()), this, SLOT(readStandartOutput()) );
    p->start( scriptDstFile, params );
    p->waitForFinished();
  }

  copyStatus = copyFile( PathUtils::nativePath( gameTargetDir + "bin/game-release-unsigned.apk" ), gameTargetDir + gameName + "_.apk" );

  if (!copyStatus)
  {
    sendLog("<b>Create apk file</b> - <font color=#CC0000>FAIL</font>");
    clearAll();
    return;
  }

  sendLog("<b>Create apk file</b> - <font color=#00CC00>OK</font>");

  // Подпись файла
  sendLog("<b>Sign apk file:</b>");

  {
    QProcess *p = new QProcess();
    p->setWorkingDirectory( gameTargetDir );
    p->setEnvironment( QProcess::systemEnvironment() );
    p->setProcessChannelMode( QProcess::MergedChannels );

    QStringList params;
    params << "-verbose";
    params << "-sigalg";
    params << "MD5withRSA";
    params << "-digestalg";
    params << "SHA1";
    params << "-keystore";
    params << PathUtils::nativePath( PathUtils::nativePath( exportInfo.kKey ) );
    params << PathUtils::nativePath( gameTargetDir + gameName + "_.apk" );
    params << exportInfo.kAlias;

    connect( p, SIGNAL(readyReadStandardOutput()), this, SLOT(readStandartOutput()) );
    p->start( PathUtils::nativePath( exportInfo.JDK + "/bin/jarsigner" ), params );
    p->waitForReadyRead();
    p->write( QString( exportInfo.kPassword + "\n" ).toLatin1().constData() );
    p->waitForBytesWritten();
    p->waitForFinished();
  }

  // Выравнивание файла
  sendLog("<b>Align apk file:</b>");

  {
    QProcess *p = new QProcess();
    p->setWorkingDirectory( gameTargetDir );
    p->setEnvironment( QProcess::systemEnvironment() );
    p->setProcessChannelMode( QProcess::MergedChannels );

    QStringList params;
    params << "-v";
    params << "4";
    params << PathUtils::nativePath( gameTargetDir + gameName + "_.apk" );
    params << PathUtils::nativePath( gameTargetDir + gameName + ".apk" );

    connect( p, SIGNAL(readyReadStandardOutput()), this, SLOT(readStandartOutput()) );
    p->start( PathUtils::nativePath( exportInfo.SDK + "/tools/zipalign" ), params );
    p->waitForFinished();
  }

  filesToRemove.append( scriptDstFile );
  filesToRemove.append( gameTargetDir + "AndroidManifest.xml" );
  filesToRemove.append( gameTargetDir + "ant.properties" );
  filesToRemove.append( gameTargetDir + "build.properties" );
  filesToRemove.append( gameTargetDir + "build.xml" );
  filesToRemove.append( gameTargetDir + "default.properties" );
  filesToRemove.append( gameTargetDir + "proguard-project.txt" );
  filesToRemove.append( gameTargetDir + "project.properties" );
  filesToRemove.append( gameTargetDir + gameName + "_.apk" );

  dirsToRemove.append( gameTargetDir + "assets" );
  dirsToRemove.append( gameTargetDir + "jni" );
  dirsToRemove.append( gameTargetDir + "libs" );
  dirsToRemove.append( gameTargetDir + "res" );
  dirsToRemove.append( gameTargetDir + "src" );
  dirsToRemove.append( gameTargetDir + "bin" );
  dirsToRemove.append( gameTargetDir + "gen" );

  clearAll();
  sendLog("<b>Clear temp files</b> - <font color=#00CC00>OK</font>");
}
//=================================================================================================
void AndroidExporter::readStandartOutput()
{
  QProcess *p = dynamic_cast<QProcess *>( sender() );

  if (p)
    sendLog( codec->toUnicode( p->readAllStandardOutput() ) );
}
//=================================================================================================
