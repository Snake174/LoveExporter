#ifndef MACEXPORTER_H
#define MACEXPORTER_H

//=================================================================================================
#include "ExportThread.h"
//=================================================================================================
class MacExporter : public ExportThread
{
  Q_OBJECT

  public:
    explicit MacExporter( const ExportInfo &info, QObject *parent = 0 );

  protected:
    void run();
};
//=================================================================================================

#endif // MACEXPORTER_H
