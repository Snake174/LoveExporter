#include <QApplication>
#include <QAction>
#include <QComboBox>
#include <QCheckBox>
#include <QDir>
#include <QFrame>
#include <QFile>
#include <QFileDialog>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMenu>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QTextEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QTabWidget>
#include <QTextStream>
#include <QToolBar>
#include <QToolButton>
#include <QThread>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QDebug>
#include <QImageWriter>
#include <QPainter>
#include <QProcess>

#ifdef Qt5
  #include <QtConcurrent/QtConcurrentRun>
#else
  #include <QtConcurrentRun>
#endif

#include "PathUtils.h"
#include "SDKWidget.h"
#include "WindowsExporter.h"
#include "AndroidExporter.h"
#include "LinuxExporter.h"
#include "MacExporter.h"
#include "MainWindow.h"

//=================================================================================================
MainWindow::MainWindow( QWidget *parent ):
  QMainWindow( parent )
{
  createMenu();

  tabs = new QTabWidget( this );
  tabs->addTab( gameSettings( tabs ), QIcon(":icons/game.png"), tr("Game") );
  tabs->addTab( keyStore( tabs ), QIcon(":icons/key.png"), tr("KeyStore") );
  tabs->addTab( settings( tabs ), QIcon(":icons/settings.png"), tr("Settings") );
  tabs->addTab( output( tabs ), QIcon(":icons/log.png"), tr("Log") );
  tabs->addTab( info( tabs ), QIcon(":icons/info.png"), tr("Info") );

  setCentralWidget( tabs );
  setMinimumSize( 640, 480 );
  setWindowTitle( QString("LOVE Exporter [v%1.%2]").arg( MAJOR_VERSION ).arg( MINOR_VERSION ) );
}
//=================================================================================================
void MainWindow::createMenu()
{
  tbExport = new QToolBar( tr("Export"), this );
  tbExport->setIconSize( QSize( 32, 32 ) );
  tbExport->setContextMenuPolicy( Qt::NoContextMenu );

  // Windows
  QToolButton *tbWindows = new QToolButton( this );
  tbWindows->setIcon( QIcon(":icons/windows.png") );
  tbWindows->setIconSize( QSize( 32, 32 ) );
  tbWindows->setToolTip( tr("Create Windows executable file") );
  tbWindows->setFocusPolicy( Qt::NoFocus );

  QMenu *tbWindowsMenu = new QMenu( tbWindows );

  // Windows x86
  QAction *aWin32 = new QAction( QIcon(":icons/windows.png"), "x86", this );
  aWin32->setObjectName("data/windows/x86");
  connect( aWin32, SIGNAL(triggered()), this, SLOT(exportWindows()) );

  // Windows x64
  QAction *aWin64 = new QAction( QIcon(":icons/windows.png"), "x64", this );
  aWin64->setObjectName("data/windows/x64");
  connect( aWin64, SIGNAL(triggered()), this, SLOT(exportWindows()) );

  tbWindowsMenu->addAction( aWin32 );
  tbWindowsMenu->addAction( aWin64 );

  tbWindows->setMenu( tbWindowsMenu );
  connect( tbWindows, SIGNAL(clicked()), tbWindows, SLOT(showMenu()) );

  // Android
  QToolButton *tbAndroid = new QToolButton( this );
  tbAndroid->setIcon( QIcon(":icons/android.png") );
  tbAndroid->setIconSize( QSize( 32, 32 ) );
  tbAndroid->setToolTip( tr("Create Android executable file") );
  tbAndroid->setFocusPolicy( Qt::NoFocus );

  QMenu *tbAndroidMenu = new QMenu( tbAndroid );

  // Android 2.3.3
  QAction *a233 = new QAction( QIcon(":icons/android.png"), "2.3.3", this );
  a233->setObjectName("data/android/2.3.3-10/libs");
  a233->setWhatsThis("10");
  connect( a233, SIGNAL(triggered()), this, SLOT(exportAndroid()) );

  // Android 3.1
  QAction *a31 = new QAction( QIcon(":icons/android.png"), "3.1", this );
  a31->setObjectName("data/android/3.1-12/libs");
  a31->setWhatsThis("12");
  connect( a31, SIGNAL(triggered()), this, SLOT(exportAndroid()) );

  // Android 4.0.3
  QAction *a403 = new QAction( QIcon(":icons/android.png"), "4.0.3", this );
  a403->setObjectName("data/android/4.0.3-15/libs");
  a403->setWhatsThis("15");
  connect( a403, SIGNAL(triggered()), this, SLOT(exportAndroid()) );

  tbAndroidMenu->addAction( a233 );
  tbAndroidMenu->addAction( a31 );
  tbAndroidMenu->addAction( a403 );

  tbAndroid->setMenu( tbAndroidMenu );
  connect( tbAndroid, SIGNAL(clicked()), tbAndroid, SLOT(showMenu()) );

  // Linux
  QToolButton *tbLinux = new QToolButton( this );
  tbLinux->setIcon( QIcon(":icons/linux.png") );
  tbLinux->setIconSize( QSize( 32, 32 ) );
  tbLinux->setToolTip( tr("Create Linux executable file") );
  tbLinux->setObjectName("data/linux");
  tbLinux->setFocusPolicy( Qt::NoFocus );
  connect( tbLinux, SIGNAL(clicked()), this, SLOT(exportLinux()) );

  // Mac OS X
  QToolButton *tbMac = new QToolButton( this );
  tbMac->setIcon( QIcon(":icons/mac.png") );
  tbMac->setIconSize( QSize( 32, 32 ) );
  tbMac->setToolTip( tr("Create MacOSX x64 executable file") );
  tbMac->setObjectName("data/mac");
  tbMac->setFocusPolicy( Qt::NoFocus );
  connect( tbMac, SIGNAL(clicked()), this, SLOT(exportMac()) );
  tbMac->setEnabled( false );

  // Updates
  QToolButton *tbUpdate = new QToolButton( this );
  tbUpdate->setIcon( QIcon(":icons/update.png") );
  tbUpdate->setIconSize( QSize( 32, 32 ) );
  tbUpdate->setToolTip( tr("Check Updates") );
  tbUpdate->setFocusPolicy( Qt::NoFocus );
  connect( tbUpdate, SIGNAL(clicked()), this, SLOT(checkUpdates()) );
  tbUpdate->setEnabled( false );

  QFrame *fr = new QFrame( tbExport );
  fr->setFocusPolicy( Qt::NoFocus );
  QHBoxLayout *hl = new QHBoxLayout( fr );
  hl->addStretch(1);

  tbExport->addWidget( tbWindows );
  tbExport->addWidget( tbAndroid );
  tbExport->addWidget( tbLinux );
  tbExport->addWidget( tbMac );
  tbExport->addWidget( fr );
  tbExport->addWidget( tbUpdate );
  tbExport->setContextMenuPolicy( Qt::PreventContextMenu );

  addToolBar( tbExport );
}
//=================================================================================================
bool MainWindow::checkData()
{
  if (gameName->text().trimmed().isEmpty())
  {
    QMessageBox::warning( this, tr("Warning"), tr("Enter game name") );
    gameName->setFocus();
    tabs->setCurrentIndex(0);
    return false;
  }

  if (gameSourceDir->text().trimmed().isEmpty())
  {
    QMessageBox::warning( this, tr("Warning"), tr("Select game source dir") );
    gameSourceDir->setFocus();
    tabs->setCurrentIndex(0);
    return false;
  }

  return true;
}
//=================================================================================================
void MainWindow::checkConf( int t )
{
  QString srcDir = gameSourceDir->text().trimmed();

  if (!srcDir.endsWith( PathUtils::nativePath("/") ))
    srcDir.append( PathUtils::nativePath("/") );

  QString str;

  if (t == Export::Windows)
    str = "  %1 = %2\r\n";
  else
    str = "  %1 = %2\n";

  QFile conf( PathUtils::nativePath( srcDir + "conf.lua" ) );

  if (!conf.exists())
  {
    if (!conf.open( QIODevice::WriteOnly ))
      return;

    QTextStream ts( &conf );
    ts.setAutoDetectUnicode( true );
    ts.setGenerateByteOrderMark( false );

    if (t == Export::Windows)
      ts << "function love.conf(t)\r\n";
    else
      ts << "function love.conf(t)\n";

    ts << str.arg( sgIdentity->objectName() ).arg( !sgIdentity->text().trimmed().isEmpty() ? "\"" + sgIdentity->text().trimmed() + "\"" : "nil" );
    ts << str.arg( sgVersion->objectName() ).arg( "\"" + sgVersion->text().trimmed() + "\"" );
    ts << str.arg( sgConsole->objectName() ).arg( sgConsole->isChecked() ? "true" : "false" );
    ts << str.arg( swTitle->objectName() ).arg( "\"" + swTitle->text().trimmed() + "\"" );
    //ts << str.arg("t.window.icon").arg("\"LoveWindowIcon.png\"");
    ts << str.arg( swWidth->objectName() ).arg( swWidth->value() );
    ts << str.arg( swHeight->objectName() ).arg( swHeight->value() );
    ts << str.arg( swBorderless->objectName() ).arg( swBorderless->isChecked() ? "true" : "false" );
    ts << str.arg( swResizable->objectName() ).arg( swResizable->isChecked() ? "true" : "false" );
    ts << str.arg( swMinWidth->objectName() ).arg( swMinWidth->value() );
    ts << str.arg( swMinHeight->objectName() ).arg( swMinHeight->value() );
    ts << str.arg( swFullscreen->objectName() ).arg( swFullscreen->isChecked() ? "true" : "false" );
    ts << str.arg( swFullscreenType->objectName() ).arg( "\"" + swFullscreenType->currentText() + "\"" );
    ts << str.arg( swVSync->objectName() ).arg( swVSync->isChecked() ? "true" : "false" );
    ts << str.arg( swFSSA->objectName() ).arg( swFSSA->value() );
    ts << str.arg( swDisplay->objectName() ).arg( swDisplay->currentText() );
    ts << str.arg( swHDPI->objectName() ).arg( swHDPI->isChecked() ? "true" : "false" );
    ts << str.arg( swSRGB->objectName() ).arg( swSRGB->isChecked() ? "true" : "false" );
    ts << str.arg( smAudio->objectName() ).arg( smAudio->isChecked() ? "true" : "false" );
    ts << str.arg( smEvent->objectName() ).arg( smEvent->isChecked() ? "true" : "false" );
    ts << str.arg( smGraphics->objectName() ).arg( smGraphics->isChecked() ? "true" : "false" );
    ts << str.arg( smImage->objectName() ).arg( smImage->isChecked() ? "true" : "false" );
    ts << str.arg( smJoystick->objectName() ).arg( smJoystick->isChecked() ? "true" : "false" );
    ts << str.arg( smKeyboard->objectName() ).arg( smKeyboard->isChecked() ? "true" : "false" );
    ts << str.arg( smMath->objectName() ).arg( smMath->isChecked() ? "true" : "false" );
    ts << str.arg( smMouse->objectName() ).arg( smMouse->isChecked() ? "true" : "false" );
    ts << str.arg( smPhysics->objectName() ).arg( smPhysics->isChecked() ? "true" : "false" );
    ts << str.arg( smSound->objectName() ).arg( smSound->isChecked() ? "true" : "false" );
    ts << str.arg( smSystem->objectName() ).arg( smSystem->isChecked() ? "true" : "false" );
    ts << str.arg( smTimer->objectName() ).arg( smTimer->isChecked() ? "true" : "false" );
    ts << str.arg( smWindow->objectName() ).arg( smWindow->isChecked() ? "true" : "false" );
    ts << str.arg( smThread->objectName() ).arg( smThread->isChecked() ? "true" : "false" );
    ts << "end";

    conf.close();
  }
}
//=================================================================================================
QWidget *MainWindow::gameSettings( QWidget *parent )
{
  QWidget *wgt = new QWidget( parent );
  QGridLayout *gl = new QGridLayout( wgt );

  gameName = new QLineEdit( wgt );
  gameName->setText("LoveGame");

  gameSourceDir = new QLineEdit( wgt );
  gameSourceDir->setReadOnly( true );

  QToolButton *tbSelectGameSourceDir = new QToolButton( wgt );
  tbSelectGameSourceDir->setText("...");
  connect( tbSelectGameSourceDir, SIGNAL(clicked()), this, SLOT(selectGameSourceDir()) );

  gameTargetDir = new QLineEdit( wgt );

  QToolButton *tbSelectGameTargetDir = new QToolButton( wgt );
  tbSelectGameTargetDir->setText("...");
  connect( tbSelectGameTargetDir, SIGNAL(clicked()), this, SLOT(selectGameTargetDir()) );

  orientation = new QComboBox( wgt );
  orientation->addItem( tr("Landscape"), "landscape" );
  orientation->addItem( tr("Portrait"), "portrait" );

  icon = new QToolButton( wgt );
  icon->setIconSize( QSize( 64, 64 ) );
  icon->setIcon( QIcon(":icons/ic_launcher.png") );
  icon->setWhatsThis(":icons/ic_launcher.png");
  connect( icon, SIGNAL(clicked()), this, SLOT(selectIcon()) );

  gl->addWidget( new QLabel( tr("Game Name"), wgt ), 0, 0 );
  gl->addWidget( gameName, 0, 1, 1, 2 );

  gl->addWidget( new QLabel( tr("Source Dir"), wgt ), 1, 0 );
  gl->addWidget( gameSourceDir, 1, 1 );
  gl->addWidget( tbSelectGameSourceDir, 1, 2 );

  gl->addWidget( new QLabel( tr("Target Dir"), wgt ), 2, 0 );
  gl->addWidget( gameTargetDir, 2, 1 );
  gl->addWidget( tbSelectGameTargetDir, 2, 2 );

  gl->addWidget( new QLabel( tr("Orientation"), wgt ), 3, 0 );
  gl->addWidget( orientation, 3, 1, 1, 2 );

  gl->addWidget( new QLabel( tr("Icon"), wgt ), 4, 0 );
  gl->addWidget( icon, 4, 1, 1, 2, Qt::AlignCenter );

  gl->setAlignment( Qt::AlignTop );

  return wgt;
}
//=================================================================================================
QWidget *MainWindow::keyStore( QWidget *parent )
{
  QWidget *wgt = new QWidget( parent );
  QGridLayout *gl = new QGridLayout( wgt );

  QToolButton *tbSetKeyDir = new QToolButton( wgt );
  tbSetKeyDir->setText("...");
  connect( tbSetKeyDir, SIGNAL(clicked()), this, SLOT(selectKeyFile()) );

  kKey = new QLineEdit( wgt );
  kAlias = new QLineEdit( wgt );

  kPassword = new QLineEdit( wgt );
  kPassword->setEchoMode( QLineEdit::Password );

  QGroupBox *gb = new QGroupBox( tr("Create KeyStore"), wgt );
  QGridLayout *glg = new QGridLayout( gb );

  kName = new QLineEdit( gb );
  kUnit = new QLineEdit( gb );
  kOrganization = new QLineEdit( gb );
  kCity = new QLineEdit( gb );
  kState = new QLineEdit( gb );

  kCountryCode = new QLineEdit( gb );
  kCountryCode->setMaxLength(2);

  QPushButton *pbCreate = new QPushButton( tr("Create"), gb );
  connect( pbCreate, SIGNAL(clicked()), this, SLOT(createNewKeystore()) );

  glg->addWidget( new QLabel( tr("Name"), wgt ), 0, 0 );
  glg->addWidget( kName, 0, 1 );
  glg->addWidget( new QLabel( tr("Unit"), wgt ), 1, 0 );
  glg->addWidget( kUnit, 1, 1 );
  glg->addWidget( new QLabel( tr("Organization"), wgt ), 2, 0 );
  glg->addWidget( kOrganization, 2, 1 );
  glg->addWidget( new QLabel( tr("City"), wgt ), 3, 0 );
  glg->addWidget( kCity, 3, 1 );
  glg->addWidget( new QLabel( tr("State"), wgt ), 4, 0 );
  glg->addWidget( kState, 4, 1 );
  glg->addWidget( new QLabel( tr("Country Code"), wgt ), 5, 0 );
  glg->addWidget( kCountryCode, 5, 1 );
  glg->addWidget( pbCreate, 6, 1, 1, 2 );
  glg->setAlignment( Qt::AlignTop );

  gb->setVisible( false );

  QToolButton *tbCreateKey = new QToolButton( wgt );
  tbCreateKey->setIconSize( QSize( 16, 16 ) );
  tbCreateKey->setIcon( QIcon(":icons/key.png") );
  tbCreateKey->setToolTip( tr("Create new keystore") );
  tbCreateKey->setCheckable( true );
  connect( tbCreateKey, SIGNAL(toggled(bool)), gb, SLOT(setVisible(bool)) );

  gl->addWidget( new QLabel( tr("Key"), wgt ), 0, 0 );
  gl->addWidget( kKey, 0, 1 );
  gl->addWidget( tbSetKeyDir, 0, 2 );
  gl->addWidget( tbCreateKey, 0, 3 );
  gl->addWidget( new QLabel( tr("Alias"), wgt ), 1, 0 );
  gl->addWidget( kAlias, 1, 1, 1, 3 );
  gl->addWidget( new QLabel( tr("Password"), wgt ), 2, 0 );
  gl->addWidget( kPassword, 2, 1, 1, 3 );
  gl->addWidget( gb, 3, 0, 1, 4 );
  gl->setAlignment( Qt::AlignTop );

  return wgt;
}
//=================================================================================================
QWidget *MainWindow::settings( QWidget *parent )
{
  QWidget *wgt = new QWidget( parent );
  QVBoxLayout *vl = new QVBoxLayout( wgt );

  QTabWidget *tabs = new QTabWidget( wgt );


  // SDK
  QFrame *gbSDK = new QFrame( tabs );
  QGridLayout *gl = new QGridLayout( gbSDK );

  #ifdef WIN32
    QString urlSDK = "http://dl.google.com/android/installer_r23.0.2-windows.exe";
    QString urlNDK = "http://dl.google.com/android/ndk/android-ndk-r9d-windows-x86.zip";
    QString urlJDK = "http://download.oracle.com/otn/java/jdk/6u45-b06/jdk-6u45-windows-i586.exe";
    QString urlANT = "https://drive.google.com/file/d/0B2Rm8VGinE84c0pheVdjS3JmWlU"; //"http://apache-mirror.rbc.ru/pub/apache//ant/binaries/apache-ant-1.9.4-bin.zip";
  #else
    QString urlSDK = "http://dl.google.com/android/android-sdk_r23.0.2-linux.tgz";
    QString urlNDK = "http://dl.google.com/android/ndk/android-ndk32-r10b-linux-x86.tar.bz2";
    QString urlJDK = "http://download.oracle.com/otn/java/jdk/6u45-b06/jdk-6u45-linux-i586.bin";
    QString urlANT = "http://apache-mirror.rbc.ru/pub/apache//ant/binaries/apache-ant-1.9.4-bin.zip";
  #endif

  SDK = new SDKWidget( "Android SDK", urlSDK, gbSDK );
  SDK->path->setText( qgetenv("ANDROID_SDK") );

  NDK = new SDKWidget( "Android NDK", urlNDK, gbSDK );
  NDK->path->setText( qgetenv("ANDROID_NDK") );

  JDK = new SDKWidget( "JDK", urlJDK, gbSDK );
  JDK->path->setText( qgetenv("JAVA_HOME") );

  ANT = new SDKWidget( "ANT", urlANT, gbSDK );
  ANT->path->setText( qgetenv("ANT_HOME") );

  gl->addWidget( new QLabel( tr("Android SDK"), gbSDK ), 0, 0 );
  gl->addWidget( SDK, 0, 1 );
  gl->addWidget( new QLabel( tr("Android NDK"), gbSDK ), 1, 0 );
  gl->addWidget( NDK, 1, 1 );
  gl->addWidget( new QLabel( tr("JDK 6"), gbSDK ), 2, 0 );
  gl->addWidget( JDK, 2, 1 );
  gl->addWidget( new QLabel( tr("ANT"), gbSDK ), 3, 0 );
  gl->addWidget( ANT, 3, 1 );
  gl->setAlignment( Qt::AlignTop );


  // AdMob
  QFrame *gbAdMob = new QFrame( tabs );
  QGridLayout *gla = new QGridLayout( gbAdMob );

  aAppID = new QLineEdit( gbAdMob );

  aPosition = new QComboBox( wgt );
  aPosition->addItem( tr("Top"), "ALIGN_PARENT_TOP" );
  aPosition->addItem( tr("Bottom"), "ALIGN_PARENT_BOTTOM" );

  aBanner = new QComboBox( wgt );
  aBanner->addItem( tr("320 x 50"), "BANNER" );
  aBanner->addItem( tr("320 x 100"), "LARGE_BANNER" );
  aBanner->addItem( tr("300 x 250"), "MEDIUM_RECTANGLE" );
  aBanner->addItem( tr("468 x 60"), "FULL_BANNER" );
  aBanner->addItem( tr("728 x 90"), "LEADERBOARD" );

  gla->addWidget( new QLabel( tr("App ID"), gbAdMob ), 0, 0 );
  gla->addWidget( aAppID, 0, 1 );
  gla->addWidget( new QLabel( tr("Position"), gbAdMob ), 1, 0 );
  gla->addWidget( aPosition, 1, 1 );
  gla->addWidget( new QLabel( tr("Banner"), gbAdMob ), 2, 0 );
  gla->addWidget( aBanner, 2, 1 );
  gla->setAlignment( Qt::AlignTop );


  // Game settings
  QTabWidget *tabGame = new QTabWidget( tabs );

  // System
  QFrame *fSystem = new QFrame( tabGame );
  QGridLayout *gSystem = new QGridLayout( fSystem );

  sgIdentity = new QLineEdit( fSystem );
  sgIdentity->setToolTip( tr("The name of the save directory") );
  sgIdentity->setObjectName("t.identity");

  sgVersion = new QLineEdit( "0.9.1", fSystem );
  sgVersion->setToolTip( tr("The LÖVE version this game was made for") );
  sgVersion->setObjectName("t.version");
  sgVersion->setEnabled( false );

  sgConsole = new QCheckBox( fSystem );
  sgConsole->setToolTip( tr("Attach a console (Windows only)") );
  sgConsole->setObjectName("t.console");

  gSystem->addWidget( new QLabel( tr("Identity"), fSystem ), 0, 0 );
  gSystem->addWidget( sgIdentity, 0, 1 );
  gSystem->addWidget( new QLabel( tr("Version"), fSystem ), 1, 0 );
  gSystem->addWidget( sgVersion, 1, 1 );
  gSystem->addWidget( new QLabel( tr("Console"), fSystem ), 2, 0 );
  gSystem->addWidget( sgConsole, 2, 1 );
  gSystem->setAlignment( Qt::AlignTop );

  // Window
  QFrame *fWindow = new QFrame( tabGame );
  QGridLayout *gWindow = new QGridLayout( fWindow );

  swTitle = new QLineEdit( "LoveGame", fWindow );
  swTitle->setToolTip( tr("The window title") );
  swTitle->setObjectName("t.window.title");
  connect( gameName, SIGNAL(textChanged(QString)), swTitle, SLOT(setText(QString)) );

  //t.window.icon = nil                -- Filepath to an image to use as the window's icon (string)

  swWidth = new QSpinBox( fWindow );
  swWidth->setButtonSymbols( QSpinBox::NoButtons );
  swWidth->setMinimum(1);
  swWidth->setMaximum( 99999 );
  swWidth->setValue( 800 );
  swWidth->setToolTip( tr("The window width") );
  swWidth->setObjectName("t.window.width");

  swHeight = new QSpinBox( fWindow );
  swHeight->setButtonSymbols( QSpinBox::NoButtons );
  swHeight->setMinimum(1);
  swHeight->setMaximum( 99999 );
  swHeight->setValue( 600 );
  swHeight->setToolTip( tr("The window height") );
  swHeight->setObjectName("t.window.height");

  swBorderless = new QCheckBox( fWindow );
  swBorderless->setToolTip( tr("Remove all border visuals from the window") );
  swBorderless->setObjectName("t.window.borderless");

  swResizable = new QCheckBox( fWindow );
  swResizable->setToolTip( tr("Let the window be user-resizable") );
  swResizable->setObjectName("t.window.resizable");

  swMinWidth = new QSpinBox( fWindow );
  swMinWidth->setButtonSymbols( QSpinBox::NoButtons );
  swMinWidth->setMinimum(1);
  swMinWidth->setMaximum( 99999 );
  swMinWidth->setValue(1);
  swMinWidth->setToolTip( tr("Minimum window width if the window is resizable") );
  swMinWidth->setObjectName("t.window.minwidth");

  swMinHeight = new QSpinBox( fWindow );
  swMinHeight->setButtonSymbols( QSpinBox::NoButtons );
  swMinHeight->setMinimum(1);
  swMinHeight->setMaximum( 99999 );
  swMinHeight->setValue(1);
  swMinHeight->setToolTip( tr("Minimum window height if the window is resizable") );
  swMinHeight->setObjectName("t.window.minheight");

  swFullscreen = new QCheckBox( fWindow );
  swFullscreen->setToolTip( tr("Enable fullscreen") );
  swFullscreen->setObjectName("t.window.fullscreen");

  swFullscreenType = new QComboBox( fWindow );
  swFullscreenType->addItem("normal");
  swFullscreenType->addItem("desktop");
  swFullscreenType->setToolTip( tr("Standard fullscreen or desktop fullscreen mode") );
  swFullscreenType->setObjectName("t.window.fullscreentype");

  swVSync = new QCheckBox( fWindow );
  swVSync->setToolTip( tr("Enable vertical sync") );
  swVSync->setObjectName("t.window.vsync");
  swVSync->setChecked( true );

  swFSSA = new QSpinBox( fWindow );
  swFSSA->setButtonSymbols( QSpinBox::NoButtons );
  swFSSA->setMinimum(0);
  swFSSA->setMaximum( 32 );
  swFSSA->setValue(0);
  swFSSA->setToolTip( tr("The number of samples to use with multi-sampled antialiasing") );
  swFSSA->setObjectName("t.window.fsaa");

  swDisplay = new QComboBox( fWindow );
  swDisplay->addItem("1");
  swDisplay->addItem("2");
  swDisplay->setToolTip( tr("Index of the monitor to show the window in") );
  swDisplay->setObjectName("t.window.display");

  swHDPI = new QCheckBox( fWindow );
  swHDPI->setToolTip( tr("Enable high-dpi mode for the window on a Retina display") );
  swHDPI->setObjectName("t.window.highdpi");

  swSRGB = new QCheckBox( fWindow );
  swSRGB->setToolTip( tr("Enable sRGB gamma correction when drawing to the screen") );
  swSRGB->setObjectName("t.window.srgb");

  gWindow->addWidget( new QLabel( tr("Title"), fWindow ), 0, 0 );
  gWindow->addWidget( swTitle, 0, 1 );
  gWindow->addWidget( new QLabel( tr("Width"), fWindow ), 1, 0 );
  gWindow->addWidget( swWidth, 1, 1 );
  gWindow->addWidget( new QLabel( tr("Height"), fWindow ), 2, 0 );
  gWindow->addWidget( swHeight, 2, 1 );
  gWindow->addWidget( new QLabel( tr("Borderless"), fWindow ), 3, 0 );
  gWindow->addWidget( swBorderless, 3, 1 );
  gWindow->addWidget( new QLabel( tr("Resizable"), fWindow ), 4, 0 );
  gWindow->addWidget( swResizable, 4, 1 );
  gWindow->addWidget( new QLabel( tr("Min width"), fWindow ), 5, 0 );
  gWindow->addWidget( swMinWidth, 5, 1 );
  gWindow->addWidget( new QLabel( tr("Min height"), fWindow ), 6, 0 );
  gWindow->addWidget( swMinHeight, 6, 1 );
  gWindow->addWidget( new QLabel( tr("FullScreen"), fWindow ), 7, 0 );
  gWindow->addWidget( swFullscreen, 7, 1 );
  gWindow->addWidget( new QLabel( tr("FullScreen type"), fWindow ), 8, 0 );
  gWindow->addWidget( swFullscreenType, 8, 1 );
  gWindow->addWidget( new QLabel( tr("VSync"), fWindow ), 9, 0 );
  gWindow->addWidget( swVSync, 9, 1 );
  gWindow->addWidget( new QLabel( tr("FSSA"), fWindow ), 10, 0 );
  gWindow->addWidget( swFSSA, 10, 1 );
  gWindow->addWidget( new QLabel( tr("Display"), fWindow ), 11, 0 );
  gWindow->addWidget( swDisplay, 11, 1 );
  gWindow->addWidget( new QLabel( tr("HDPI"), fWindow ), 12, 0 );
  gWindow->addWidget( swHDPI, 12, 1 );
  gWindow->addWidget( new QLabel( tr("sRGB"), fWindow ), 13, 0 );
  gWindow->addWidget( swSRGB, 13, 1 );

  gWindow->setAlignment( Qt::AlignTop );


  // Modules
  QFrame *fModules = new QFrame( tabGame );
  QGridLayout *gModules = new QGridLayout( fModules );

  smAudio = new QCheckBox( tr("audio"), fModules );
  smAudio->setToolTip(  tr("Enable the audio module") );
  smAudio->setObjectName("t.modules.audio");
  smAudio->setChecked( true );

  smEvent = new QCheckBox( tr("event"), fModules );
  smEvent->setToolTip(  tr("Enable the event module") );
  smEvent->setObjectName("t.modules.event");
  smEvent->setChecked( true );

  smGraphics = new QCheckBox( tr("graphics"), fModules );
  smGraphics->setToolTip(  tr("Enable the graphics module") );
  smGraphics->setObjectName("t.modules.graphics");
  smGraphics->setChecked( true );

  smImage = new QCheckBox( tr("image"), fModules );
  smImage->setToolTip(  tr("Enable the image module") );
  smImage->setObjectName("t.modules.image");
  smImage->setChecked( true );

  smJoystick = new QCheckBox( tr("joystick"), fModules );
  smJoystick->setToolTip(  tr("Enable the joystick module") );
  smJoystick->setObjectName("t.modules.joystick");

  smKeyboard = new QCheckBox( tr("keyboard"), fModules );
  smKeyboard->setToolTip(  tr("Enable the keyboard module") );
  smKeyboard->setObjectName("t.modules.keyboard");
  smKeyboard->setChecked( true );

  smMath = new QCheckBox( tr("math"), fModules );
  smMath->setToolTip(  tr("Enable the math module") );
  smMath->setObjectName("t.modules.math");
  smMath->setChecked( true );

  smMouse = new QCheckBox( tr("mouse"), fModules );
  smMouse->setToolTip(  tr("Enable the mouse module") );
  smMouse->setObjectName("t.modules.mouse");
  smMouse->setChecked( true );

  smPhysics = new QCheckBox( tr("physics"), fModules );
  smPhysics->setToolTip(  tr("Enable the physics module") );
  smPhysics->setObjectName("t.modules.physics");

  smSound = new QCheckBox( tr("sound"), fModules );
  smSound->setToolTip(  tr("Enable the sound module") );
  smSound->setObjectName("t.modules.sound");
  smSound->setChecked( true );

  smSystem = new QCheckBox( tr("system"), fModules );
  smSystem->setToolTip(  tr("Enable the system module") );
  smSystem->setObjectName("t.modules.system");
  smSystem->setChecked( true );

  smTimer = new QCheckBox( tr("timer"), fModules );
  smTimer->setToolTip(  tr("Enable the timer module") );
  smTimer->setObjectName("t.modules.timer");
  smTimer->setChecked( true );

  smWindow = new QCheckBox( tr("window"), fModules );
  smWindow->setToolTip(  tr("Enable the window module") );
  smWindow->setObjectName("t.modules.window");
  smWindow->setChecked( true );

  smThread = new QCheckBox( tr("thread"), fModules );
  smThread->setToolTip(  tr("Enable the thread module") );
  smThread->setObjectName("t.modules.thread");

  gModules->addWidget( smAudio, 0, 0 );
  gModules->addWidget( smEvent, 0, 1 );
  gModules->addWidget( smGraphics, 1, 0 );
  gModules->addWidget( smImage, 1, 1 );
  gModules->addWidget( smJoystick, 2, 0 );
  gModules->addWidget( smKeyboard, 2, 1 );
  gModules->addWidget( smMath, 3, 0 );
  gModules->addWidget( smMouse, 3, 1 );
  gModules->addWidget( smPhysics, 4, 0 );
  gModules->addWidget( smSound, 4, 1 );
  gModules->addWidget( smSystem, 5, 0 );
  gModules->addWidget( smTimer, 5, 1 );
  gModules->addWidget( smWindow, 6, 0 );
  gModules->addWidget( smThread, 6, 1 );
  gModules->setAlignment( Qt::AlignTop );


  tabGame->addTab( fSystem, QIcon(""), tr("System") );
  tabGame->addTab( fWindow, QIcon(""), tr("Window") );
  tabGame->addTab( fModules, QIcon(""), tr("Modules") );
  tabGame->setTabPosition( QTabWidget::South );


  // Excludes
  seExcludes = new QListWidget( tabs );
  seExcludes->setEnabled( false );

  connect( gameSourceDir, SIGNAL(textChanged(QString)), this, SLOT(updateExcludes(QString)) );

  tabs->addTab( gbSDK, QIcon(""), tr("SDK") );
  tabs->addTab( gbAdMob, QIcon(""), tr("AdMob") );
  tabs->addTab( tabGame, QIcon(""), tr("Game") );
  tabs->addTab( seExcludes, QIcon(""), tr("Excludes") );

  vl->addWidget( tabs );
  vl->setAlignment( Qt::AlignTop );

  return wgt;
}
//=================================================================================================
QWidget *MainWindow::output( QWidget *parent )
{
  QWidget *wgt = new QWidget( parent );
  QVBoxLayout *vl = new QVBoxLayout( wgt );

  outputText = new QPlainTextEdit( wgt );
  outputText->setReadOnly( true );
  outputText->setWordWrapMode( QTextOption::NoWrap );

  vl->addWidget( outputText );
  vl->setAlignment( Qt::AlignTop );

  return wgt;
}
//=================================================================================================
QWidget *MainWindow::info( QWidget *parent )
{
  QWidget *wgt = new QWidget( parent );
  QVBoxLayout *vl = new QVBoxLayout( wgt );

  infoText = new QTextEdit( wgt );
  infoText->setReadOnly( true );
  infoText->setWordWrapMode( QTextOption::NoWrap );

  QFile f(":icons/info.txt");
  f.open( QIODevice::ReadOnly );
  QTextStream ts( &f );
  ts.setAutoDetectUnicode( true );
  ts.setGenerateByteOrderMark( false );
  infoText->setHtml( ts.readAll() );
  f.close();

  vl->addWidget( infoText );
  vl->setAlignment( Qt::AlignTop );

  return wgt;
}
//=================================================================================================
void MainWindow::threadStarted()
{
  tbExport->setEnabled( false );
  outputText->clear();
  outputText->appendHtml("<b>Export started</b><br>");
  tabs->setCurrentIndex(3);
}
//=================================================================================================
void MainWindow::threadFinished()
{
  outputText->appendHtml("<br><b>Export finished</b>");
  tbExport->setEnabled( true );
}
//=================================================================================================
void MainWindow::writeLog( const QString &msg )
{
  outputText->appendHtml( msg );
}
//=================================================================================================
void MainWindow::selectIcon()
{
  QString iconFile = QFileDialog::getOpenFileName( this, QString( tr("Game Icon") ),
                                                   qApp->applicationDirPath(), QString( tr("Image Files (*.png)") ) );

  if (!iconFile.isEmpty())
  {
    icon->setIcon( QIcon( iconFile ) );
    icon->setWhatsThis( iconFile );
  }
}
//=================================================================================================
void MainWindow::selectGameSourceDir()
{
  QFileDialog fd( this );
  fd.setFileMode( QFileDialog::DirectoryOnly );

  QString gameLocation = fd.getExistingDirectory( this, tr("Game Source Dir"), qApp->applicationDirPath() );

  if (!gameLocation.isEmpty())
    gameSourceDir->setText( gameLocation );
}
//=================================================================================================
void MainWindow::selectGameTargetDir()
{
  QFileDialog fd( this );
  fd.setFileMode( QFileDialog::DirectoryOnly );

  QString gameLocation = fd.getExistingDirectory( this, tr("Game target Dir"), qApp->applicationDirPath() );

  if (!gameLocation.isEmpty())
    gameTargetDir->setText( gameLocation );
}
//=================================================================================================
void MainWindow::checkUpdates()
{
}
//=================================================================================================
void MainWindow::exportWindows()
{
  if (!checkData())
    return;

  checkConf( Export::Windows );

  QString gameTargetDirStr = gameTargetDir->text().trimmed();

  if (gameTargetDirStr.isEmpty())
    gameTargetDirStr = PathUtils::nativePath( qApp->applicationDirPath() + "/" + gameName->text().trimmed() + "/" );
  else
  {
    if (gameTargetDirStr.endsWith( PathUtils::nativePath("/") ))
      gameTargetDirStr = PathUtils::nativePath( gameTargetDirStr + gameName->text().trimmed() + "/" );
    else
      gameTargetDirStr = PathUtils::nativePath( gameTargetDirStr + "/" + gameName->text().trimmed() + "/" );
  }

  ExportInfo info;

  // Иконка приложения
  QString iconDir = PathUtils::nativePath( gameTargetDirStr + "icon/" );

  if (!QDir( iconDir ).exists())
    QDir().mkpath( iconDir );

  QImage image( icon->whatsThis() );

  QPainter p( &image );
  p.setRenderHint( QPainter::Antialiasing );
  p.drawImage( 0, 0, image );
  p.end();

  QImageWriter w;
  w.setFileName( PathUtils::nativePath( iconDir + "icon.ico" ) );

  if (w.write( image ))
    info.iconDir = PathUtils::nativePath( iconDir + "icon.ico" );
  else
    info.iconDir = "";

  QAction *act = dynamic_cast<QAction *>( sender() );

  info.gameName = gameName->text().trimmed();
  info.gameSourceDir = gameSourceDir->text().trimmed();
  info.gameTargetDir = gameTargetDir->text().trimmed();
  info.zipDir = act->objectName();

  for (int i = 0; i < seExcludes->count(); ++i)
  {
    QListWidgetItem *it = seExcludes->item(i);

    if (it->checkState() == Qt::Checked)
      info.excludes.append( it->text() );
  }

  WindowsExporter *we = new WindowsExporter( info, this );
  connect( we, SIGNAL(started()), this, SLOT(threadStarted()) );
  connect( we, SIGNAL(finished()), this, SLOT(threadFinished()) );
  connect( we, SIGNAL(sendLog(QString)), this, SLOT(writeLog(QString)) );
  we->start();
}
//=================================================================================================
void MainWindow::exportAndroid()
{
  if (!checkData())
    return;

  if (SDK->path->text().trimmed().isEmpty())
  {
    QMessageBox::warning( this, tr("Warning"), tr("Set Android SDK path") );
    SDK->setFocus();
    tabs->setCurrentIndex(2);
    return;
  }

  if (NDK->path->text().trimmed().isEmpty())
  {
    QMessageBox::warning( this, tr("Warning"), tr("Set Android NDK path") );
    NDK->setFocus();
    tabs->setCurrentIndex(2);
    return;
  }

  if (JDK->path->text().trimmed().isEmpty())
  {
    QMessageBox::warning( this, tr("Warning"), tr("Set Java JDK path") );
    JDK->setFocus();
    tabs->setCurrentIndex(2);
    return;
  }

  if (ANT->path->text().trimmed().isEmpty())
  {
    QMessageBox::warning( this, tr("Warning"), tr("Set Ant path") );
    ANT->setFocus();
    tabs->setCurrentIndex(2);
    return;
  }

  if (kKey->text().trimmed().isEmpty())
  {
    QMessageBox::warning( this, tr("Warning"), tr("Select keystore file") );
    kKey->setFocus();
    tabs->setCurrentIndex(1);
    return;
  }

  if (kAlias->text().trimmed().isEmpty())
  {
    QMessageBox::warning( this, tr("Warning"), tr("Set keystore alias") );
    kAlias->setFocus();
    tabs->setCurrentIndex(1);
    return;
  }

  if (kPassword->text().trimmed().isEmpty())
  {
    QMessageBox::warning( this, tr("Warning"), tr("Enter password") );
    kPassword->setFocus();
    tabs->setCurrentIndex(1);
    return;
  }

  //checkConf( Export::Other );

  QString gameTargetDirStr = gameTargetDir->text().trimmed();

  if (gameTargetDirStr.isEmpty())
    gameTargetDirStr = PathUtils::nativePath( qApp->applicationDirPath() + "/" + gameName->text().trimmed() + "/" );
  else
  {
    if (gameTargetDirStr.endsWith( PathUtils::nativePath("/") ))
      gameTargetDirStr = PathUtils::nativePath( gameTargetDirStr + gameName->text().trimmed() + "/" );
    else
      gameTargetDirStr = PathUtils::nativePath( gameTargetDirStr + "/" + gameName->text().trimmed() + "/" );
  }

  // Иконка приложения
  QString iconDir = PathUtils::nativePath( gameTargetDirStr + "res/" );

  QDir().mkpath( PathUtils::nativePath( iconDir + "drawable-hdpi" ) );
  QDir().mkpath( PathUtils::nativePath( iconDir + "drawable-mdpi" ) );
  QDir().mkpath( PathUtils::nativePath( iconDir + "drawable-xhdpi" ) );
  QDir().mkpath( PathUtils::nativePath( iconDir + "drawable-xxhdpi" ) );

  QImage tmpImage( icon->whatsThis() );

  // HDPI
  QImage imgHDPI( QSize( 72, 72 ), QImage::Format_ARGB32 );
  imgHDPI.fill( Qt::transparent );

  QPainter pHDPI( &imgHDPI );
  pHDPI.setRenderHint( QPainter::Antialiasing );
  pHDPI.drawImage( QRect( 0, 0, 72, 72 ), tmpImage, QRect( 0, 0, tmpImage.width(), tmpImage.height() ) );
  pHDPI.end();

  QImageWriter wHDPI;
  wHDPI.setFileName( PathUtils::nativePath( iconDir + "drawable-hdpi/ic_launcher.png" ) );

  if (!wHDPI.write( imgHDPI ))
    QMessageBox::critical( 0, tr("Save Image"), wHDPI.errorString() );

  // MDPI
  QImage imgMDPI( QSize( 42, 42 ), QImage::Format_ARGB32 );
  imgMDPI.fill( Qt::transparent );

  QPainter pMDPI( &imgMDPI );
  pMDPI.setRenderHint( QPainter::Antialiasing );
  pMDPI.drawImage( QRect( 0, 0, 42, 42 ), tmpImage, QRect( 0, 0, tmpImage.width(), tmpImage.height() ) );
  pMDPI.end();

  QImageWriter wMDPI;
  wMDPI.setFileName( PathUtils::nativePath( iconDir + "drawable-mdpi/ic_launcher.png" ) );
  wMDPI.write( imgMDPI );

  // XHDPI
  QImage imgXHDPI( QSize( 96, 96 ), QImage::Format_ARGB32 );
  imgXHDPI.fill( Qt::transparent );

  QPainter pXHDPI( &imgXHDPI );
  pXHDPI.setRenderHint( QPainter::Antialiasing );
  pXHDPI.drawImage( QRect( 0, 0, 96, 96 ), tmpImage, QRect( 0, 0, tmpImage.width(), tmpImage.height() ) );
  pXHDPI.end();

  QImageWriter wXHDPI;
  wXHDPI.setFileName( PathUtils::nativePath( iconDir + "drawable-xhdpi/ic_launcher.png" ) );
  wXHDPI.write( imgXHDPI );

  // XXHDPI
  QImage imgXXHDPI( QSize( 144, 144 ), QImage::Format_ARGB32 );
  imgXXHDPI.fill( Qt::transparent );

  QPainter pXXHDPI( &imgXXHDPI );
  pXXHDPI.setRenderHint( QPainter::Antialiasing );
  pXXHDPI.drawImage( QRect( 0, 0, 144, 144 ), tmpImage, QRect( 0, 0, tmpImage.width(), tmpImage.height() ) );
  pXXHDPI.end();

  QImageWriter wXXHDPI;
  wXXHDPI.setFileName( PathUtils::nativePath( iconDir + "drawable-xxhdpi/ic_launcher.png" ) );
  wXXHDPI.write( imgXXHDPI );

  QAction *act = dynamic_cast<QAction *>( sender() );

  ExportInfo info;

  info.iconDir = PathUtils::nativePath( iconDir );
  info.gameName = gameName->text().trimmed();
  info.gameSourceDir = gameSourceDir->text().trimmed();
  info.gameTargetDir = gameTargetDir->text().trimmed();
  info.zipDir = act->objectName();
  info.api = act->whatsThis();
  info.orientation = orientation->itemData( orientation->currentIndex() ).toString();

  info.SDK = SDK->path->text().trimmed();
  info.NDK = NDK->path->text().trimmed();
  info.JDK = JDK->path->text().trimmed();
  info.ANT = ANT->path->text().trimmed();

  info.kKey = kKey->text().trimmed();
  info.kAlias = kAlias->text().trimmed();
  info.kPassword = kPassword->text().trimmed();

  AndroidExporter *ae = new AndroidExporter( info, this );
  connect( ae, SIGNAL(started()), this, SLOT(threadStarted()) );
  connect( ae, SIGNAL(finished()), this, SLOT(threadFinished()) );
  connect( ae, SIGNAL(sendLog(QString)), this, SLOT(writeLog(QString)) );
  ae->start();
}
//=================================================================================================
void MainWindow::exportLinux()
{
  if (!checkData())
    return;

  checkConf( Export::Other );

  QString gameTargetDirStr = gameTargetDir->text().trimmed();

  if (gameTargetDirStr.isEmpty())
    gameTargetDirStr = PathUtils::nativePath( qApp->applicationDirPath() + "/" + gameName->text().trimmed() + "/" );
  else
  {
    if (gameTargetDirStr.endsWith( PathUtils::nativePath("/") ))
      gameTargetDirStr = PathUtils::nativePath( gameTargetDirStr + gameName->text().trimmed() + "/" );
    else
      gameTargetDirStr = PathUtils::nativePath( gameTargetDirStr + "/" + gameName->text().trimmed() + "/" );
  }

  QDir().mkpath( gameTargetDirStr );

  QToolButton *act = dynamic_cast<QToolButton *>( sender() );

  ExportInfo info;
  info.gameName = gameName->text().trimmed();
  info.gameSourceDir = gameSourceDir->text().trimmed();
  info.gameTargetDir = gameTargetDir->text().trimmed();
  info.zipDir = act->objectName();

  for (int i = 0; i < seExcludes->count(); ++i)
  {
    QListWidgetItem *it = seExcludes->item(i);

    if (it->checkState() == Qt::Checked)
      info.excludes.append( it->text() );
  }

  LinuxExporter *le = new LinuxExporter( info, this );
  connect( le, SIGNAL(started()), this, SLOT(threadStarted()) );
  connect( le, SIGNAL(finished()), this, SLOT(threadFinished()) );
  connect( le, SIGNAL(sendLog(QString)), this, SLOT(writeLog(QString)) );
  le->start();
}
//=================================================================================================
void MainWindow::exportMac()
{
  if (!checkData())
    return;

  checkConf( Export::Other );

  QToolButton *act = dynamic_cast<QToolButton *>( sender() );

  ExportInfo info;
  info.gameName = gameName->text().trimmed();
  info.gameSourceDir = gameSourceDir->text().trimmed();
  info.gameTargetDir = gameTargetDir->text().trimmed();
  info.zipDir = act->objectName();

  for (int i = 0; i < seExcludes->count(); ++i)
  {
    QListWidgetItem *it = seExcludes->item(i);

    if (it->checkState() == Qt::Checked)
      info.excludes.append( it->text() );
  }

  MacExporter *me = new MacExporter( info, this );
  connect( me, SIGNAL(started()), this, SLOT(threadStarted()) );
  connect( me, SIGNAL(finished()), this, SLOT(threadFinished()) );
  connect( me, SIGNAL(sendLog(QString)), this, SLOT(writeLog(QString)) );
  me->start();
}
//=================================================================================================
void MainWindow::createNewKeystore()
{
  if (kAlias->text().trimmed().isEmpty())
  {
    QMessageBox::warning( this, tr("Warning"), tr("Set keystore alias") );
    kAlias->setFocus();
    tabs->setCurrentIndex(1);
    return;
  }

  if (kPassword->text().trimmed().isEmpty())
  {
    QMessageBox::warning( this, tr("Warning"), tr("Set keystore password") );
    kPassword->setFocus();
    tabs->setCurrentIndex(1);
    return;
  }

  QString keyName = QFileDialog::getSaveFileName( this, tr("Save key"),
                                                  qApp->applicationDirPath(),
                                                  QString( tr("Keystore (*.keystore)") ) );

  if (keyName.trimmed().isEmpty())
    return;

  if (!keyName.endsWith(".keystore"))
    keyName.append(".keystore");

  kKey->setText( keyName );

  KeystoreInfo ki;
  ki.kKey = PathUtils::nativePath( kKey->text().trimmed() );
  ki.kAlias = kAlias->text().trimmed();
  ki.kPassword = kPassword->text().trimmed();
  ki.kName = kName->text().trimmed();
  ki.kUnit = kUnit->text().trimmed();
  ki.kOrganization = kOrganization->text().trimmed();
  ki.kCity = kCity->text().trimmed();
  ki.kState = kState->text().trimmed();
  ki.kCountryCode = kCountryCode->text().trimmed().toUpper();

  QtConcurrent::run( this, &MainWindow::createNewKeystoreProc, ki );
}
//=================================================================================================
void MainWindow::createNewKeystoreProc( const KeystoreInfo &ki )
{
  QProcess *p = new QProcess();
  p->setWorkingDirectory( qApp->applicationDirPath() );
  p->setEnvironment( QProcess::systemEnvironment() );
  p->setProcessChannelMode( QProcess::MergedChannels );

  QStringList params;
  params << "-genkey";
  params << "-v";
  params << "-keystore";
  params << ki.kKey;
  params << "-alias";
  params << ki.kAlias;
  params << "-sigalg";
  params << "MD5withRSA";
  params << "-keyalg";
  params << "RSA";
  params << "-keysize";
  params << "1024";
  params << "-validity";
  params << "20000";

  outputText->clear();
  tabs->setCurrentIndex(3);

  connect( p, SIGNAL(readyReadStandardOutput()), this, SLOT(readStandartOutput()) );
  p->start( PathUtils::nativePath( JDK->path->text() + "/bin/keytool" ), params );

  if (!p->waitForStarted())
    return;

  p->waitForReadyRead();

  p->write( QString( ki.kPassword + "\n" ).toLatin1().constData() );
  p->waitForBytesWritten();
  p->waitForReadyRead();

  p->write( QString( ki.kPassword + "\n" ).toLatin1().constData() );
  p->waitForBytesWritten();
  p->waitForReadyRead();

  if (!ki.kName.isEmpty())
    p->write( QString( ki.kName + "\n" ).toLatin1().constData() );
  else
    p->write("\n");

  p->waitForBytesWritten();
  p->waitForReadyRead();

  if (!ki.kUnit.isEmpty())
    p->write( QString( ki.kUnit + "\n" ).toLatin1().constData() );
  else
    p->write("\n");

  p->waitForBytesWritten();
  p->waitForReadyRead();

  if (!ki.kOrganization.isEmpty())
    p->write( QString( ki.kOrganization + "\n" ).toLatin1().constData() );
  else
    p->write("\n");

  p->waitForBytesWritten();
  p->waitForReadyRead();

  if (!ki.kCity.isEmpty())
    p->write( QString( ki.kCity + "\n" ).toLatin1().constData() );
  else
    p->write("\n");

  p->waitForBytesWritten();
  p->waitForReadyRead();

  if (!ki.kState.isEmpty())
    p->write( QString( ki.kState + "\n" ).toLatin1().constData() );
  else
    p->write("\n");

  p->waitForBytesWritten();
  p->waitForReadyRead();

  if (!ki.kCountryCode.isEmpty())
    p->write( QString( ki.kCountryCode + "\n" ).toLatin1().constData() );
  else
    p->write("\n");

  p->waitForBytesWritten();
  p->waitForReadyRead();

  p->write("yes\n");
  p->waitForBytesWritten();
  p->waitForReadyRead();

  p->write( QString( ki.kPassword + "\n" ).toLatin1().constData() );
  p->waitForBytesWritten();
  p->waitForReadyRead();

  p->write( QString( ki.kPassword + "\n" ).toLatin1().constData() );
  p->waitForBytesWritten();
  p->waitForFinished();

  writeLog("DONE");
}
//=================================================================================================
void MainWindow::selectKeyFile()
{
  QString keyFile = QFileDialog::getOpenFileName( this, QString( tr("Keystore file") ),
                                                   qApp->applicationDirPath(),
                                                   QString( tr("Keystore (*.keystore)") ) );

  if (!keyFile.isEmpty())
    kKey->setText( keyFile );
}
//=================================================================================================
void MainWindow::readStandartOutput()
{
  QProcess *p = dynamic_cast<QProcess *>( sender() );

  if (p)
    writeLog( p->readAllStandardOutput() );
}
//=================================================================================================
void MainWindow::updateExcludes( const QString &path )
{
  seExcludes->clear();

  QDir dir( PathUtils::nativePath( path ) );

  foreach (QFileInfo info, dir.entryInfoList( QDir::NoDotAndDotDot | QDir::System | QDir::Hidden  | QDir::AllDirs | QDir::Files, QDir::DirsFirst ))
  {
    if (info.isDir())
    {
      QListWidgetItem *it = new QListWidgetItem( QIcon(""), info.baseName(), seExcludes );
      it->setData( Qt::CheckStateRole, Qt::Unchecked );
      seExcludes->addItem( it );
    }
  }
}
//=================================================================================================
