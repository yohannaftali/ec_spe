#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QScreen>
#include <QtWidgets>
#include <QMenu>
#include <QAction>
#include <QItemSelectionModel>
#include <QModelIndexList>
#include <QClipboard>
#include <QApplication>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QNetworkReply>
#include <QPlainTextEdit>
#include <QMessageBox>
#include "serverwtb.h"
#include "console.h"
#include "settingsdialog.h"
#include "servicedialog.h"
#include <QTcpServer>
#include <QSslSocket>
#include <QPrinter>
#include <QPrintDialog>
#include <QScopedPointer>
#include <QSqlDatabase>
#include <QSqlTableModel>
#include <QtSerialPort/QSerialPort>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    QWidget *widgetMain;
    QVBoxLayout  *layoutMain;

    QScrollArea *scrollAreaMain;

    QWidget *widgetLogin;
    QVBoxLayout *layoutLogin;

    QWidget *widgetModule;
    QVBoxLayout *layoutModule;

    QFont appFont;

    // Login Form
    QLabel *labelLogin;
    QLabel *labelBrand;
    QLineEdit *lineEditInput;
    QPushButton *buttonLogin;
    QPushButton *buttonSubmit;
    QLabel *labelInfo;
    QLabel *labelWeightGrossKg;
    // Module Form
    QFrame *frameModule;
    QFrame *frameModuleRight;

    // Table
    QTableWidget *tableView;
    QSqlQueryModel *model;

    // Form Parameter - Master
    QLineEdit *lineEditCaseId;
    QLabel *labelCallback;
    QLabel *textCaseType;
    QLabel *textWeightNet;
    QLabel *labelWeightNetKg;
    QLineEdit *lineEditWeightGross;
    QByteArray serialData;
    QPrinter *m_currentPrinter;
    char charSerialData;
    int intSerialData;
    QLabel *labelisi;

    // windows
    int loginWidth;
    int loginHeight;
    int moduleWidth;
    int moduleHeight;

    int moduleWidthRight;
    int moduleHeightRight;
    int consoleWidth;
    int consoleHeight;
    double netWeight;
    QPixmap *pixmapGreen;
    QPixmap *pixmapWhite;

    ServerWtb *wtb;
    QString serverUrl;
    QString statusConnection;
    QString username;
    QString password;
    QString _tok;
    QString controller;

    QSerialPort *serial;
    Console *console;

    SettingsDialog *settingsDialog;
    ServiceDialog *serviceDialog;

    QString currLine;
    QString prevLine;
    bool showConsole;
    QString stableRead;
    QString currRead;
    QString isStable;
    QString currWeight;
    QString currWeight2;

    void createUserInterface();
    void createLogin();
    void createModule();
    void resetInput();
    void resetLoginForm(bool error = false);
    void callConnect();
    void hideLoginForm();  
    void createConsoleObject();
    void initActionsConnections();
    void reqAutoComplete(QString variable, QString param = "");
    void setCurrentCompleter();
    void copySelectionTableToClipboard();
    void copyAllTableToClipboard();
    void submitEnable();
    void submitDisable();

    QCompleter *currentCompleter;
    QJsonArray currentCompleterData;
    QString getACData(QString data, QString field);

private slots:
    // UI
    void submitAction();
    void setWeight(QString w, QString u = "KG");
    void resConnect();
    void resSession();
    void resLogin();
    void resHome();
    void resModule();
    void resModuleGet();

    void readData();

    void openSerialPort();
    void closeSerialPort();
    void writeData(const QByteArray &data);
    void handleError(QSerialPort::SerialPortError error);    
    void connectionLost();
    void lineEditWeightGross_onChange(QString);
    void reqAutoCompleteCaseId(QString);
    void resAutoCompleteCaseId();
    void setSiblingsCaseType(QString);

    void reqSubmit();
    void resSubmit();
    void readServiceParameter();
    void slotHandlePagePrinted(bool result);

    void on_actionLogout_triggered();
    void on_actionUpdate_triggered();   
    void on_actionSettingServer_triggered();
    void on_actionOpen_Port_triggered();
    void on_actionConfigure_triggered();
    void on_actionShow_Hide_Terminals_toggled(bool arg1);
    void on_actionReading_triggered();
    void on_actionStatus_triggered();
    void on_actionDisconnect_Port_triggered();
    void on_actionSet_Weight_Empty_triggered();
    void on_actionSet_Weight_1_triggered();
    void on_actionSet_Weight_10_triggered();
    void on_actionSet_Weight_100_triggered();
    void on_actionSet_Weight_1000_triggered();
};



#endif // MAINWINDOW_H
