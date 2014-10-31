#ifndef ANDROIDEXPORTER_H
#define ANDROIDEXPORTER_H

//=================================================================================================
#include "ExportThread.h"
//=================================================================================================
class QTextCodec;
//=================================================================================================
class AndroidExporter : public ExportThread
{
  Q_OBJECT

  QTextCodec *codec;

  public:
    explicit AndroidExporter( const ExportInfo &info, QObject *parent = 0 );

  protected:
    void run();

  private slots:
    void readStandartOutput();
};
//=================================================================================================

#endif // ANDROIDEXPORTER_H
