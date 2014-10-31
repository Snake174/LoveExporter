#ifndef WINDOWSEXPORTER_H
#define WINDOWSEXPORTER_H

//=================================================================================================
#include "ExportThread.h"
//=================================================================================================
class QTextCodec;
//=================================================================================================
#ifdef WIN32
  #include <windows.h>
  #include <io.h>
  #include <stdio.h>
  #include <stdlib.h>
  #include <fcntl.h>
#endif
//=================================================================================================
#ifdef WIN32
#pragma pack( push, 2 )
typedef struct
{
  BYTE Width;
  BYTE Height;
  BYTE Colors;
  BYTE Reserved;
  WORD Planes;
  WORD BitCount;
  DWORD ImageSize;
} IcoItemHeader;
//=================================================================================================
typedef struct
{
  IcoItemHeader Header;
  DWORD Offset;
} IcoItem;
//=================================================================================================
typedef struct
{
  WORD Reserved;
  WORD Typ;
  WORD ItemCount;
  IcoItem Items[1];
} IcoHeader;
//=================================================================================================
typedef struct
{
  IcoItemHeader Header;
  WORD Id;
} GroupIconDirItem;
//=================================================================================================
typedef struct
{
  WORD Reserved;
  WORD Typ;
  WORD ItemCount;
  GroupIconDirItem Items[1];
} GroupIconDir;
#pragma pack( pop )
#endif
//=================================================================================================
class WindowsExporter : public ExportThread
{
  Q_OBJECT

  QTextCodec *codec;

  public:
    explicit WindowsExporter( const ExportInfo &info, QObject *parent = 0 );

  protected:
    void run();

  private slots:
    void readStandartOutput();
};
//=================================================================================================

#endif // WINDOWSEXPORTER_H
