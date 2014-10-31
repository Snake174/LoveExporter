#ifndef LINUXEXPORTER_H
#define LINUXEXPORTER_H

//=================================================================================================
#include "ExportThread.h"
//=================================================================================================
class LinuxExporter : public ExportThread
{
  Q_OBJECT

  public:
    explicit LinuxExporter( const ExportInfo &info, QObject *parent = 0 );

  protected:
    void run();
};
//=================================================================================================

#endif // LINUXEXPORTER_H
