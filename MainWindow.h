#ifndef MAINWINDOW_H
#define MAINWINDOW_H

//=================================================================================================
#include <QMainWindow>
//=================================================================================================
class QComboBox;
class QLineEdit;
class QTabWidget;
class QToolBar;
class QToolButton;
class QPlainTextEdit;
class QTextEdit;
class SDKWidget;
//=================================================================================================
typedef struct
{
  QString kKey;
  QString kAlias;
  QString kPassword;
  QString kName;
  QString kUnit;
  QString kOrganization;
  QString kCity;
  QString kState;
  QString kCountryCode;
} KeystoreInfo;
//=================================================================================================
class MainWindow : public QMainWindow
{
  Q_OBJECT

  QTabWidget *tabs;
  QToolBar *tbExport;
  QPlainTextEdit *outputText;
  QTextEdit *infoText;

  // Game settings
  QLineEdit *gameName;
  QLineEdit *gameSourceDir;
  QLineEdit *gameTargetDir;
  QComboBox *orientation;
  QToolButton *icon;

  // SDK settings
  SDKWidget *SDK;
  SDKWidget *NDK;
  SDKWidget *JDK;
  SDKWidget *ANT;

  // Keystore settings
  QLineEdit *kKey;
  QLineEdit *kAlias;
  QLineEdit *kPassword;
  QLineEdit *kName;
  QLineEdit *kUnit;
  QLineEdit *kOrganization;
  QLineEdit *kCity;
  QLineEdit *kState;
  QLineEdit *kCountryCode;

  // AdMob settings
  QLineEdit *aAppID;
  QComboBox *aPosition;
  QComboBox *aBanner;

  public:
    MainWindow( QWidget *parent = 0 );
    ~MainWindow() {}

  private:
    void createMenu();
    bool checkData();
    QWidget *gameSettings( QWidget *parent );
    QWidget *keyStore( QWidget *parent );
    QWidget *settings( QWidget *parent );
    QWidget *output( QWidget *parent );
    QWidget *info( QWidget *parent );

  private slots:
    void threadStarted();
    void threadFinished();
    void writeLog( const QString &msg );
    void selectIcon();
    void selectGameSourceDir();
    void selectGameTargetDir();
    void checkUpdates();
    void exportWindows();
    void exportAndroid();
    void exportLinux();
    void exportMac();
    void createNewKeystore();
    void createNewKeystoreProc( const KeystoreInfo &ki );
    void selectKeyFile();
    void readStandartOutput();
};
//=================================================================================================

#endif // MAINWINDOW_H
