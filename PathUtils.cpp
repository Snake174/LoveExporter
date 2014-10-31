#include <QDir>

#include "PathUtils.h"

//=================================================================================================
PathUtils::PathUtils( QObject *parent ):
  QObject( parent )
{
}
//=================================================================================================
QString PathUtils::getExt( const QString &path )
{
  return path.mid( path.lastIndexOf(".") + 1 );
}
//=================================================================================================
QString PathUtils::getName( const QString &path )
{
  QString name = "";
  QString tmp = QDir::toNativeSeparators( path );
  name = tmp.mid( tmp.lastIndexOf( QDir::separator() ) + 1 );

  return name;
}
//=================================================================================================
QString PathUtils::nativePath( const QString &path )
{
  return QDir::toNativeSeparators( path );
}
//=================================================================================================
