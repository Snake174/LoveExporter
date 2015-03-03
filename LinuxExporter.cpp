#include <QApplication>

#include "PathUtils.h"
#include "LinuxExporter.h"

//=================================================================================================
LinuxExporter::LinuxExporter( const ExportInfo &info, QObject *parent ) :
  ExportThread( info, parent )
{
}
//=================================================================================================
void LinuxExporter::run()
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
  QString zipLibsDir = PathUtils::nativePath( exportInfo.zipDir );

  sendLog("<b>Get build info</b> - <font color=#00CC00>OK</font>");
  sendLog("<b>Create build folder</b> - <font color=#00CC00>OK</font>");

  // Распаковка необходимых файлов
  bool copyStatus = copyDir( zipLibsDir, gameTargetDir );

  if (!copyStatus)
  {
    sendLog("<b>Copy libraries</b> - <font color=#CC0000>FAIL</font>");
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
      clearAll();
      return;
    }
  }

  sendLog("<b>Copy libraries</b> - <font color=#00CC00>OK</font>");

  // Создание game.love
  copyStatus = zipFolder( PathUtils::nativePath( gameTargetDir + "bin/game.love" ), gameSourceDir, false );

  if (!copyStatus)
  {
    sendLog("<b>Create game.love file</b> - <font color=#CC0000>FAIL</font>");
    clearAll();
    return;
  }

  sendLog("<b>Create game.love file</b> - <font color=#00CC00>OK</font>");
}
//=================================================================================================
