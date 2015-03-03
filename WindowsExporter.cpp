#include <QApplication>
#include <QDir>
#include <QProcess>
#include <QTextCodec>

#include "PathUtils.h"
#include "WindowsExporter.h"

//=================================================================================================
WindowsExporter::WindowsExporter( const ExportInfo &info, QObject *parent ):
  ExportThread( info, parent )
{
  //QTextCodec::setCodecForLocale( QTextCodec::codecForName("UTF-8") );
  //QTextCodec::setCodecForCStrings( QTextCodec::codecForName("UTF-8") );
  //QTextCodec::setCodecForTr( QTextCodec::codecForName("UTF-8") );

  codec = QTextCodec::codecForLocale();
}
//=================================================================================================
void WindowsExporter::run()
{
  clearAll();

  // Название игры
  QString gameName = exportInfo.gameName;

  // Каталог с исходниками игры
  QString gameSourceDir = exportInfo.gameSourceDir;

  if (!gameSourceDir.endsWith( PathUtils::nativePath("/") ))
    gameSourceDir.append( PathUtils::nativePath("/") );

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
  QString zipDir = PathUtils::nativePath( exportInfo.zipDir );

  sendLog("<b>Get build info</b> - <font color=#00CC00>OK</font>");
  sendLog("<b>Create build folder</b> - <font color=#00CC00>OK</font>");

  // Распаковка необходимых файлов
  bool copyStatus = copyDir( zipDir, gameTargetDir );

  if (!copyStatus)
  {
    sendLog("<b>Copy libraries</b> - <font color=#CC0000>FAIL</font>");
    dirsToRemove.append( gameTargetDir + "icon" );
    clearAll();
    return;
  }

  for (int i = 0; i < exportInfo.excludes.count(); ++i)
  {
    if (!QDir( gameTargetDir + exportInfo.excludes.at(i) ).exists())
      QDir().mkpath( gameTargetDir + exportInfo.excludes.at(i) );

    copyStatus = copyDir( gameSourceDir + exportInfo.excludes.at(i), gameTargetDir + exportInfo.excludes.at(i) );

    if (!copyStatus)
    {
      sendLog("<b>Copy libraries</b> - <font color=#CC0000>FAIL</font>");
      dirsToRemove.append( gameTargetDir + "icon" );
      clearAll();
      return;
    }
  }

  sendLog("<b>Copy libraries</b> - <font color=#00CC00>OK</font>");

  // Смена иконки приложения
  #ifdef WIN32
    IcoHeader ico;
    GroupIconDir infoIcon;

    FILE *f = fopen( PathUtils::nativePath( exportInfo.iconDir ).toLatin1().constData(), "rb" );

    if (f)
    {
      fseek( f, 0, SEEK_END );
      long fileSize = ftell(f);
      fseek( f, 0, SEEK_SET );

      char *dataIcon = new char[ fileSize ];

      if (dataIcon)
      {
        memset( dataIcon, 0, fileSize );
        fread( dataIcon, fileSize, 1, f );
      }

      fclose(f);

      memcpy( &ico, dataIcon, sizeof( IcoHeader ) );

      HANDLE H = BeginUpdateResourceW( PathUtils::nativePath( gameTargetDir + "love.exe" ).toStdWString().c_str(), 0 );

      if (H)
      {
        ico.ItemCount = 1;

        DWORD newGroupIconDirSize = sizeof( GroupIconDir ) * ico.ItemCount;
        infoIcon.Reserved = ico.Reserved;
        infoIcon.Typ = ico.Typ;
        infoIcon.ItemCount = ico.ItemCount;

        for (int i = 0; i < ico.ItemCount; ++i)
        {
          memcpy( (void *)&infoIcon.Items[i].Header, (void *)&ico.Items[i].Header, sizeof( IcoItemHeader ) );
          infoIcon.Items[i].Id = i + 1;
          UpdateResourceW( H, RT_ICON, MAKEINTRESOURCE( i + 1 ), 1033, (LPVOID)&dataIcon[ ico.Items[i].Offset ], ico.Items[i].Header.ImageSize );
        }

        UpdateResourceW( H, RT_GROUP_ICON, MAKEINTRESOURCE(1), 1033, (LPVOID)&infoIcon, newGroupIconDirSize );
        EndUpdateResourceW( H, FALSE );

        sendLog("<b>Change game icon</b> - <font color=#00CC00>OK</font>");
      }

      delete[] dataIcon;
      dataIcon = 0;
    }
  #endif

  // Создание game.love
  copyStatus = zipFolder( gameTargetDir + "game.love", gameSourceDir, false );

  if (!copyStatus)
  {
    sendLog("<b>Create game.love file</b> - <font color=#CC0000>FAIL</font>");
    dirsToRemove.append( gameTargetDir + "icon" );
    clearAll();
    return;
  }

  sendLog("<b>Create game.love file</b> - <font color=#00CC00>OK</font>");

  // Распаковка скриптов
  #ifdef WIN32
    QString scriptSrcFile = PathUtils::nativePath("data/tools/build-windows.bat");
    QString scriptDstFile = gameTargetDir + "build-windows.bat";
  #else
    QString scriptSrcFile = PathUtils::nativePath("data/tools/build-windows.sh");
    QString scriptDstFile = gameTargetDir + "build-windows.sh";
  #endif

  copyStatus = copyFile( scriptSrcFile, scriptDstFile );

  if (!copyStatus)
  {
    sendLog("<b>Copy scripts</b> - <font color=#CC0000>FAIL</font>");
    dirsToRemove.append( gameTargetDir + "icon" );
    clearAll();
    return;
  }

  copyFile( PathUtils::nativePath("data/tools/ResHacker.exe"), gameTargetDir + "ResHacker.exe" );

  sendLog("<b>Copy scripts</b> - <font color=#00CC00>OK</font>");

  sendLog("<b>Create executable file:</b>");

  // Создание исполняемого файла
  QProcess *p = new QProcess();
  p->setWorkingDirectory( gameTargetDir );
  p->setProcessChannelMode( QProcess::MergedChannels );
  p->setEnvironment( QProcess::systemEnvironment() );

  QStringList params;
  params << "love.exe";
  params << "game.love";
  params << gameName + ".exe";

  #ifndef WIN32
    params << PathUtils::nativePath( exportInfo.iconDir );
  #endif

  connect( p, SIGNAL(readyReadStandardOutput()), this, SLOT(readStandartOutput()) );
  p->start( scriptDstFile, params );
  p->waitForFinished();

  // Удаление лишних файлов
  filesToRemove.append( gameTargetDir + "love.exe" );
  filesToRemove.append( gameTargetDir + "game.love" );
  filesToRemove.append( scriptDstFile );
  filesToRemove.append( gameTargetDir + "ResHacker.exe" );
  filesToRemove.append( gameTargetDir + "ResHacker.ini" );
  filesToRemove.append( gameTargetDir + "ResHacker.log" );

  dirsToRemove.append( gameTargetDir + "icon" );

  clearAll();
  sendLog("<b>Clear temp files</b> - <font color=#00CC00>OK</font>");
}
//=================================================================================================
void WindowsExporter::readStandartOutput()
{
  QProcess *p = dynamic_cast<QProcess *>( sender() );

  if (p)
    sendLog( codec->toUnicode( p->readAllStandardOutput() ) );
}
//=================================================================================================
