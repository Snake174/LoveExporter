#ifndef PATHUTILS_H
#define PATHUTILS_H

//=================================================================================================
#include <QObject>
//=================================================================================================
class PathUtils : public QObject
{
  Q_OBJECT

  public:
    explicit PathUtils( QObject *parent = 0 );
    static QString getExt( const QString &path );
    static QString getName( const QString &path );
    static QString nativePath( const QString &path );
};
//=================================================================================================

#endif // PATHUTILS_H
