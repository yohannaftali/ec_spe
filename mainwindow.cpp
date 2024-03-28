#include "mainwindow.h"
#include "ui_mainwindow.h"

// don;t forget to add permission
// sudo usermod -a -G dialout namauser

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);    
    this->setWindowState(Qt::WindowMaximized);
    // global variable
    prevLine = "";
    currLine = "";
    showConsole = false;
    controller = "mm_packing_weight";
    // Initial new connection to server
    wtb = new ServerWtb(this);

    // form
    createUserInterface();
    settingsDialog = new SettingsDialog;
    serviceDialog = new ServiceDialog;

    connect(serviceDialog, SIGNAL(applyNewConfiguration()), this, SLOT(readServiceParameter()));
    lineEditInput->setFocus();

    QColor colorGreen(0,255,0,200);
    QColor colorWhite(255,255,255,200);
    pixmapGreen= new QPixmap(20,20);
    pixmapGreen->fill(colorGreen);
    pixmapWhite = new QPixmap (20,20);
    pixmapWhite->fill(colorWhite);
    QDir directory(":/Noto_Sans");
    for(const QFileInfo & info: directory.entryInfoList()){
        int id = QFontDatabase::addApplicationFont(info.absoluteFilePath());
        if(id != -1){
            QStringList font_families = QFontDatabase::applicationFontFamilies(id);
            if(!font_families.isEmpty())
                qDebug() << font_families.first();
        }
    }
    appFont = QFont("Noto Sans");
    this->readServiceParameter();
    callConnect();
}

MainWindow::~MainWindow()
{
    delete ui;
}

///////////////////////////////////////////////////////////////////////////////
// User Interface
///////////////////////////////////////////////////////////////////////////////
void MainWindow::createUserInterface()
{
    loginWidth = 1580;
    loginHeight = 753;
    moduleWidth = 1580;
    moduleHeight = 753;
    moduleWidthRight = 700;
    moduleHeightRight = 600;
    consoleWidth = 1000;
    consoleHeight = moduleHeight;

    layoutMain = new QVBoxLayout ;
    this->ui->centralWidget->setLayout(layoutMain);

    widgetLogin = new QWidget();
    widgetLogin->setStyleSheet("background: rgba(3, 78, 173, 1);");
    layoutMain->addWidget(widgetLogin);

    widgetModule = new QWidget();
    widgetModule->setStyleSheet("background: rgba(3, 78, 173, 1);");
    layoutMain->addWidget(widgetModule);

    createLogin();
    createModule();
    createConsoleObject();

    QScreen *screen = QGuiApplication::primaryScreen();
    QRect screenRect = screen->availableGeometry();
    int screenWidth = screenRect.width();
    int screenHeight = screenRect.height();
    int windowWidth = qMin(screenWidth, moduleWidth);
    int windowHeight = qMin(screenHeight, moduleHeight);
    this->resize(windowWidth, windowHeight);
    this->move((screenWidth - windowWidth) / 2, (screenHeight - windowHeight) / 2);

    widgetLogin->show();
    widgetModule->hide();
}

void MainWindow::createLogin()
{
    // Brand
    QFont fontLabelBrand = QFont("Noto Sans ExtraBold");
    fontLabelBrand.setPointSize(26);
    fontLabelBrand.setBold(true);

    labelBrand = new QLabel();
    labelBrand->setStyleSheet("color:white;");
    labelBrand->setText("SenologSys SPE v24.03.28");
    labelBrand->setFont(fontLabelBrand);
    labelBrand->show();
    labelBrand->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    // Info
    QFont fontLabelInfo = QFont("Noto Sans Medium");
    fontLabelInfo.setPointSize(16);
    fontLabelInfo.setItalic(true);
    fontLabelInfo.setBold(true);

    labelInfo = new QLabel();
    labelInfo->setText("");
    labelInfo->setStyleSheet("color:white;");
    labelInfo->setFont(fontLabelInfo);
    labelInfo->show();
    labelInfo->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    // Input
    // - Label
    labelLogin = new QLabel();
    labelLogin->setStyleSheet("color:white;");
    labelLogin->setText("Username :");
    appFont.setPointSize(36);
    appFont.setBold(true);
    labelLogin->setFont(appFont);
    labelLogin->show();
    // - Line Edit
    lineEditInput = new QLineEdit();
    lineEditInput->setObjectName("lineEditInput");
    lineEditInput->setStyleSheet("QLineEdit { background: lightblue; border-width: 2px; border-radius: 6px; }"
                                   "QLineEdit:hover { background: white; border-color: #4CAF50; }");
    appFont.setPointSize(26);
    appFont.setBold(true);
    lineEditInput->setFont(appFont);
    lineEditInput->show();

    // Button
    int wButton = 300;
    int hButton = 70;
    buttonLogin = new QPushButton();
    buttonLogin->setFixedSize(wButton, hButton);
    buttonLogin->setStyleSheet("QPushButton { background-color: green; color: white; border-radius: 8px; }"
                               "QPushButton:hover { background-color: #4CAF50; border: 2px solid #4CAF50; }");
    buttonLogin->setCursor(Qt::PointingHandCursor);
    buttonLogin->setText("Continue");
    appFont.setPointSize(26);
    appFont.setBold(true);
    buttonLogin->setFont(appFont);
    buttonLogin->show();

    // Layout
    QHBoxLayout *layoutInput = new QHBoxLayout();
    layoutInput->addWidget(labelLogin);
    layoutInput->addWidget(lineEditInput);

    QHBoxLayout *layoutButton = new QHBoxLayout();
    layoutButton->addStretch();
    layoutButton->addWidget(buttonLogin);
    layoutButton->addStretch();

    // Layout Login
    layoutLogin = new QVBoxLayout;
    layoutLogin->addWidget(labelBrand);
    layoutLogin->addStretch();
    layoutLogin->addWidget(labelInfo);
    layoutLogin->addLayout(layoutInput);
    layoutLogin->addStretch();
    layoutLogin->addLayout(layoutButton);
    layoutLogin->addStretch();

    widgetLogin->setLayout(layoutLogin);

    // Listener
    connect(lineEditInput,SIGNAL(returnPressed()), this, SLOT(submitAction()));
    connect(buttonLogin, SIGNAL(clicked()), this, SLOT(submitAction()));
}

void MainWindow::createModule()
{
    // Font
    appFont.setPointSize(26);
    appFont.setBold(true);

    // Label Callback
    labelCallback = new QLabel();
    labelCallback->setStyleSheet("color:white;");
    labelCallback->setFont(appFont);
    labelCallback->show();

    // Case ID
    QLabel *labelCaseId = new QLabel();
    labelCaseId->setText("Lot No/Case ID: ");
    labelCaseId->setStyleSheet("color:white;");
    labelCaseId->setFont(appFont);
    labelCaseId->show();

    lineEditCaseId = new QLineEdit();
    lineEditCaseId->setAlignment(Qt::AlignRight);
    lineEditCaseId->setStyleSheet("QLineEdit { background: lightblue; border-width: 2px; border-radius: 6px; }"
                                   "QLineEdit:hover { background: white; border-color: #4CAF50; }");
    lineEditCaseId->setFont(appFont);
    lineEditCaseId->show();

    // Case Type
    QLabel *labelCaseType = new QLabel();
    labelCaseType->setText("Case Type: ");
    labelCaseType->setStyleSheet("color:white;");
    labelCaseType->setFont(appFont);
    labelCaseType->show();

    textCaseType = new QLabel();
    textCaseType->setText("");
    textCaseType->setStyleSheet("color:white;");
    textCaseType->setFont(appFont);
    textCaseType->show();

    // Weight Net
    QLabel *labelWeightNet = new QLabel();
    labelWeightNet->setText("Net Weight: ");
    labelWeightNet->setStyleSheet("color:white;");
    labelWeightNet->setFont(appFont);
    labelWeightNet->show();

    textWeightNet = new QLabel();
    textWeightNet->setText("");
    textWeightNet->setStyleSheet("color:white;");
    textWeightNet->setAlignment(Qt::AlignRight);
    textWeightNet->setFont(appFont);
    textWeightNet->show();

    labelWeightNetKg = new QLabel();
    labelWeightNetKg->setText("KG");
    labelWeightNetKg->setStyleSheet("color:white;");
    labelWeightNetKg->setFont(appFont);
    labelWeightNetKg->show();

    // Weight Gross
    QLabel *labelWeightGross = new QLabel();
    labelWeightGross->setText("Gross Weight: ");
    labelWeightGross->setStyleSheet("color:white;");
    labelWeightGross->setFont(appFont);
    labelWeightGross->show();

    lineEditWeightGross = new QLineEdit();
    lineEditWeightGross->setStyleSheet("QLineEdit { background: rgba(177, 177, 177, 1); border-width: 2px; border-radius: 6px; }"
                                   "QLineEdit:hover { background: rgba(200, 200, 200, 1); border-color: #4CAF50; }");
    lineEditWeightGross->setReadOnly(true);
    lineEditWeightGross->setAlignment(Qt::AlignRight);
    lineEditWeightGross->setFont(appFont);
    lineEditWeightGross->show();

    labelWeightGrossKg = new QLabel();
    labelWeightGrossKg->setText("KG");
    labelWeightGrossKg->setStyleSheet("color:white;");
    labelWeightGrossKg->setFont(appFont);
    labelWeightGrossKg->show();

    // Table
    tableView = new QTableWidget();
    tableView->setObjectName("tableViewDetail");
    tableView->setFont(appFont);
    tableView->show();

    // Button Submit
    int wButton = 300;
    int hButton = 70;
    buttonSubmit = new QPushButton();
    buttonSubmit->setFixedSize(wButton, hButton);
    buttonSubmit->setCursor(Qt::PointingHandCursor);
    buttonSubmit->setText("Submit");
    buttonSubmit->setFont(appFont);
    buttonSubmit->show();
    submitDisable();

    // Layout
    QHBoxLayout *layoutCaseId = new QHBoxLayout();
    layoutCaseId->addWidget(labelCaseId);
    layoutCaseId->addStretch();
    layoutCaseId->addWidget(lineEditCaseId);

    QHBoxLayout *layoutCaseType = new QHBoxLayout();
    layoutCaseType->addWidget(labelCaseType);
    layoutCaseType->addStretch();
    layoutCaseType->addWidget(textCaseType);

    QVBoxLayout *layoutCase = new QVBoxLayout();
    layoutCase->addLayout(layoutCaseId);
    layoutCase->addLayout(layoutCaseType);

    QHBoxLayout *layoutWeightNet = new QHBoxLayout();
    layoutWeightNet->addWidget(labelWeightNet);
    layoutWeightNet->addStretch();
    layoutWeightNet->addWidget(textWeightNet);
    layoutWeightNet->addWidget(labelWeightNetKg);

    QHBoxLayout *layoutWeightGross = new QHBoxLayout();
    layoutWeightGross->addWidget(labelWeightGross);
    layoutWeightGross->addStretch();
    layoutWeightGross->addWidget(lineEditWeightGross);
    layoutWeightGross->addWidget(labelWeightGrossKg);

    QVBoxLayout *layoutWeight = new QVBoxLayout();
    layoutWeight->addLayout(layoutWeightNet);
    layoutWeight->addLayout(layoutWeightGross);

    QHBoxLayout *layoutHeader = new QHBoxLayout();
    layoutHeader->addLayout(layoutCase);
    layoutHeader->addStretch(1);
    layoutHeader->addLayout(layoutWeight);

    QHBoxLayout *layoutButton = new QHBoxLayout();
    layoutButton->addStretch();
    layoutButton->addWidget(buttonSubmit);

    // Layout Module
    layoutModule = new QVBoxLayout;
    layoutModule->addLayout(layoutHeader);
    layoutModule->addWidget(tableView);
    layoutModule->addWidget(labelCallback);
    layoutModule->addLayout(layoutButton);

     widgetModule->setLayout(layoutModule);

    // Create the context menu
    QMenu* contextMenu = new QMenu(tableView);
    QAction* copySelection  = new QAction("Copy Selected Row", contextMenu);
    contextMenu->addAction(copySelection );
    QAction* copyTable  = new QAction("Copy Table", contextMenu);
    contextMenu->addAction(copyTable);
    tableView->setContextMenuPolicy(Qt::CustomContextMenu);
    tableView->setSelectionBehavior(QAbstractItemView::SelectRows);

    // Listener
    connect(lineEditCaseId, SIGNAL(textEdited(QString)), this, SLOT(reqAutoCompleteCaseId(QString)));
    connect(lineEditCaseId, SIGNAL(textChanged(QString)), this, SLOT(setSiblingsCaseType(QString)));
    connect(lineEditWeightGross, SIGNAL(textEdited(QString)), this, SLOT(lineEditWeightGross_onChange(QString)));
    connect(buttonSubmit, SIGNAL(clicked()), this, SLOT(reqSubmit()));
    connect(tableView, &QWidget::customContextMenuRequested, this, [this, contextMenu](const QPoint& pos) {
        contextMenu->exec(tableView->mapToGlobal(pos));
    });
    connect(copySelection, &QAction::triggered, this, &MainWindow::copySelectionTableToClipboard);
    connect(copyTable, &QAction::triggered, this, &MainWindow::copyAllTableToClipboard);
}

void MainWindow::setWeight(QString w, QString u)
{
    if(w == "") {
        QString m = u != "KG" ? u : w;
        ui->actionReading->setText(m);
        submitDisable();
    }
    else{
        ui->actionReading->setText(QString("%1 %2").arg(w, u));
        double gw = w.replace(",", "").toDouble();
        if(gw < netWeight) {
             submitDisable();
        }
        else{
            submitEnable();
        }
    }
    stableRead = w;
    lineEditWeightGross->setText(w);
}

void MainWindow::submitEnable()
{
    QString styleGreen = "QPushButton { background-color: green; color: white; border-radius: 8px; }"
                         "QPushButton:hover { background-color: #4CAF50; border: 2px solid #4CAF50; }";
    buttonSubmit->setEnabled(true);
    buttonSubmit->setStyleSheet(styleGreen);
}

void MainWindow::submitDisable()
{
    QString styleRed = "QPushButton { background-color: red; color: white; border-radius: 8px; } "
                       "QPushButton:hover { background-color: red; border: 2px solid red; }";
    buttonSubmit->setEnabled(false);
    buttonSubmit->setStyleSheet(styleRed);
}

void MainWindow::createConsoleObject()
{
    console = new Console();
    console->setEnabled(false);
    serial = new QSerialPort(this);    
    connect(serial, SIGNAL(errorOccurred(QSerialPort::SerialPortError)), this, SLOT(handleError(QSerialPort::SerialPortError)));
    connect(serial, SIGNAL(readyRead()), this, SLOT(readData()));
    connect(console, SIGNAL(getData(QByteArray)), this, SLOT(writeData(QByteArray)));
    connect(serial, SIGNAL(aboutToClose()), this, SLOT(connectionLost()));
    initActionsConnections();
    console->hide();
}

///////////////////////////////////////////////////////////////////////////////
// Serial Port
///////////////////////////////////////////////////////////////////////////////

void MainWindow::openSerialPort()
{
    SettingsDialog::Settings p = settingsDialog->settings();
    serial->setPortName(p.name);
    serial->setBaudRate(p.baudRate);
    serial->setDataBits(p.dataBits);
    serial->setParity(p.parity);
    serial->setStopBits(p.stopBits);
    serial->setFlowControl(p.flowControl);
    if (serial->open(QIODevice::ReadWrite)) {
            console->setEnabled(true);
            console->setLocalEchoEnabled(p.localEchoEnabled);
            ui->statusBar->showMessage(tr("Connected to %1 : %2, %3, %4, %5, %6")
                .arg(p.name, p.stringBaudRate, p.stringDataBits, p.stringParity, p.stringStopBits, p.stringFlowControl));
    } else {
        QMessageBox::critical(this, tr("Error"), serial->errorString());

        ui->statusBar->showMessage(tr("Open error"));
    }
}

void MainWindow::closeSerialPort()
{
    serial->clear();
    serial->close();
    setWeight("", "Please Connect Device");
    console->setEnabled(false);
    ui->statusBar->showMessage(tr("Disconnected"));
}

void MainWindow::writeData(const QByteArray &data)
{
    serial->write(data);
}

void MainWindow::connectionLost()
{
     ui->statusBar->showMessage(tr("Closing Connection"));
}

void MainWindow::readData()
{
    static QRegularExpression whitespace("\\s+");
    serialData = serial->readAll();
    serial->clear();
    if(showConsole)
    {
        console->putData(serialData);
    }
    if(!(serialData.isNull() || serialData.isEmpty())) {
        for(int i = 0; i < serialData.size(); i++) {
            charSerialData = serialData.at(i);
            intSerialData = charSerialData;
            if(intSerialData == 13) {
                ui->actionOpen_Port->setIcon(*pixmapGreen);
                if(currLine != "" && prevLine != currLine && currLine.size() == 15) {
                    prevLine = currLine;
                    currRead = currLine;
                    QStringList list = currLine.split(whitespace, Qt::SkipEmptyParts);
                    isStable = list[0];
                    currWeight = list[1];
                    currWeight2 = list[2];
                    if(isStable == ",0") {
                        labelWeightGrossKg->setText("KG");
                        double b = currWeight.split(" ")[0].toFloat() / 100;
                        QString a = QString::number(b, 'g', 4);
                        setWeight(a);
                    }
                    else if(isStable == ";0") {
                        labelWeightGrossKg->setText("KG");
                        double b = currWeight.split(" ")[0].toFloat() / 10;
                        QString a = QString::number(b, 'g', 4);
                        setWeight(a);
                    }
                    else if(isStable == "<0") {
                        labelWeightGrossKg->setText("KG");
                        double b = currWeight.split(" ")[0].toFloat() / 100;
                        QString a = QString::number(b, 'g', 4);
                        setWeight(a);
                    }
                    else if(isStable == ")") {
                        labelWeightGrossKg->setText("Gram");
                        setWeight(currWeight2, "G");
                    }
                }
            }
            else {
                if(intSerialData != 2) {
                    currLine.append(charSerialData);
                }
                else {
                    currLine = "";
                    ui->actionOpen_Port->setIcon(*pixmapWhite);
                }
            }
        }
    }
}

void MainWindow::handleError(QSerialPort::SerialPortError error)
{
    if(error == QSerialPort::ResourceError) {
        QMessageBox::critical(this, tr("Critical Error"), serial->errorString());
        closeSerialPort();
    }
}

void MainWindow::initActionsConnections()
{
    this->ui->actionDisconnect_Port->setDisabled(true);
    this->ui->actionOpen_Port->setEnabled(true);
    this->ui->actionConfigure->setEnabled(true);
}

///////////////////////////////////////////////////////////////////////////////
// Server Connection
///////////////////////////////////////////////////////////////////////////////
void MainWindow::readServiceParameter()
{
    ServiceDialog::Settings serviceParam = serviceDialog->settings();    
    serverUrl = serviceParam.serverAddress;
    qDebug() << "Server URL: " << serverUrl;
}

void MainWindow::callConnect()
{   
    QString paramUrl = serverUrl;
    qDebug() << "Start new connection to the server " << paramUrl;
    wtb->serverConnect(paramUrl);

    // Define callback function for response
    connect(wtb->currentReply, SIGNAL(finished()), this, SLOT(resConnect()));
}

void MainWindow::resConnect()
{
    qDebug() << "* response new connection";
    QVariant statusCode = wtb->currentReply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    if(!statusCode.isValid())
    {
        statusConnection = "Connection Down";
        ui->actionStatus->setText(statusConnection);
        QString reason = wtb->currentReply->attribute( QNetworkRequest::HttpReasonPhraseAttribute ).toString();
        labelInfo->setText(reason);
        qDebug() << "Connection is down";
        return;
    }
    int status = statusCode.toInt();    
    if ( status != 200 )
    {        
        statusConnection = "Connection Error";
        ui->actionStatus->setText(statusConnection);
        QString reason = wtb->currentReply->attribute( QNetworkRequest::HttpReasonPhraseAttribute ).toString();
        labelInfo->setText(reason);
        qDebug() << "Connection Error";
    }
    else
    {
        statusConnection = QString("Connected to %1").arg(serverUrl);
        ui->actionStatus->setText(statusConnection);
        labelInfo->setText("Please Enter Username");
        qDebug() << "Connection is up";
    }
}

void MainWindow::resSession()
{
    qDebug() << "* response session";
    QString res(wtb->currentReply->readAll());
    QJsonDocument resDoc = QJsonDocument::fromJson(res.toUtf8());
    bool isNull = resDoc.isNull();
    if(!isNull) {
        QJsonObject resObject = resDoc.object();
        QJsonValue resValue = resObject.value(QString("tok"));
        _tok = resValue.toString();
        if(_tok != "") {
            labelLogin->setText("Password:");
            buttonLogin->setText("Login");
            labelInfo->setText("Please Enter Password");
            lineEditInput->setEchoMode(QLineEdit::Password);
            lineEditInput->setText("");
            lineEditInput->setFocus();
        }
        else{
            labelInfo->setText("Check network or reset application");
        }
    }
}

void MainWindow::submitAction()
{    
    QString stage = buttonLogin->text();
    if(stage == "Continue") {
        username = lineEditInput->text();
        if(username != "") {
            wtb->reqSession(username);
            connect(wtb->currentReply, SIGNAL(finished()), this, SLOT(resSession()));
        }

    }
    else if(stage == "Login") {
        password = lineEditInput->text();
        wtb->reqLogin(username, password, _tok);
        connect(wtb->currentReply, SIGNAL(finished()), this, SLOT(resLogin()));
    }
}

void MainWindow::resLogin()
{
    qDebug() << "* response login";
    QVariant possibleRedirectUrl = wtb->currentReply->attribute(QNetworkRequest::RedirectionTargetAttribute);
    statusConnection = QString("%1 is logged in").arg(username);
    ui->actionStatus->setText(statusConnection);
    wtb->reqHome(possibleRedirectUrl.toUrl());
    connect(wtb->currentReply, SIGNAL(finished()), this, SLOT(resHome()));
    qDebug() << "* redirect to home...";
}

void MainWindow::resHome()
{
    qDebug() << "* response home";
    wtb->reqModule(controller);
    connect(wtb->currentReply, SIGNAL(finished()), this, SLOT(resModule()));
    hideLoginForm();
}

void MainWindow::resetLoginForm(bool error)
{
    qDebug() << "Reset login form...";
    QString messageInfo = error ? "Password or username does not match" : "Please Enter Username";
    labelLogin->setText("Username:");
    buttonLogin->setText("Continue");
    labelInfo->setText(messageInfo);
    lineEditInput->setEchoMode(QLineEdit::Normal);
    lineEditInput->setText("");
    lineEditInput->setFocus();
    statusConnection = QString("connected to %1").arg(serverUrl);
    ui->actionStatus->setText(statusConnection);
}

void MainWindow::hideLoginForm()
{
    widgetLogin->hide();
}

void MainWindow::resModule()
{
    qDebug() << "* response module index";
    QString res(wtb->currentReply->readAll());
    QJsonDocument resDoc = QJsonDocument::fromJson(res.toUtf8());
    bool isNull = resDoc.isNull();
    if(!isNull) {
        QJsonObject resObject = resDoc.object();
        QJsonValue secureValue = resObject.value(QString("secure"));
        QJsonValue loginValue = secureValue.toObject().value(QString("login"));
        QJsonValue accessValue = secureValue.toObject().value(QString("access"));
        bool login = loginValue.toBool();
        bool access = accessValue.toBool();
        if(login && access)
        {
            widgetLogin->hide();
            widgetModule->show();
            lineEditCaseId->setFocus();
        }
    }
    else {
        qDebug() << "* redirect to login form...";
        resetLoginForm();
        widgetModule->hide();
        widgetLogin->show();
        callConnect();
    }
}


void MainWindow::resModuleGet()
{
    qDebug() << "* response get after autocomplete";

    QString res(wtb->currentReplyGet->readAll());
    QJsonDocument resDocx = QJsonDocument::fromJson(res.toUtf8());
    bool isNull = resDocx.isNull();
    if(!isNull) {
        QJsonObject object = resDocx.object();
        QJsonValue value = object.value("detail");
        QJsonValue valueMaster = object.value("master");
        QJsonObject objectMaster = valueMaster.toObject();
        QJsonValue weightNet = objectMaster.value("sum_weight_net");

        // Master
        netWeight = weightNet.toString().replace(",", "").toDouble();
        textWeightNet->setText(weightNet.toString());

        // Detail
        QJsonArray array = value.toArray();

        tableView->setRowCount(array.count());
        tableView->setColumnCount(7);

        QStringList headerLabels = {
            "Delivery No", "Delivery Item",  "Order No", "Order Item",
            "Material No", "Pack Qty", "Net (Kg)"
        };
        tableView->setHorizontalHeaderLabels(headerLabels);
        tableView->verticalHeader()->setVisible(false);

        tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

        QString headerStyle = "::section {"
                              "height: 50px;"
                              "background-color: rgb(26, 40, 68);"
                              "color: white;"
                              "color: white;"
                              "border: none;"
                              "font-family: 'Noto Sans';"
                              "font-size: 20px; }";
        tableView->horizontalHeader()->setStyleSheet(headerStyle);

        tableView->setAlternatingRowColors(true);
        tableView->setStyleSheet("QTableView::item:selected{"
                                 "background-color: #4CAF50; "
                                 "color: white; }"
                                 "QTableView::item:!selected{"
                                 "background-color: rgb(235, 235, 235);"
                                 "color: black; }"
                                 "QTableView::item:alternate { background-color: lightblue; }");
        tableView->showGrid();
        for (int i = 0; i < array.size(); i++) {
            const QJsonValue v = array[i];
            QJsonObject obj = v.toObject();

            QStringList values = {
                obj["delivery_no"].toString(),
                obj["delivery_item"].toString(),
                obj["order_no"].toString(),
                obj["order_item"].toString(),
                obj["material_no"].toString(),
                obj["pack_qty"].toString(),
                obj["weight_net"].toString()
            };

            for (int j = 0; j < values.size(); j++) {
                QTableWidgetItem *item = new QTableWidgetItem;
                item->setFont(QFont("Noto Sans", 14, QFont::Bold));
                item->setText(values[j]);
                item->setTextAlignment(Qt::AlignRight);
                tableView->setItem(i, j, item);
            }
        }
    }
}

void MainWindow::setCurrentCompleter()
{
    QString res(wtb->currentReplyAC->readAll());
    QStringList acList;
    QJsonValue datax;
    QJsonDocument resDoc = QJsonDocument::fromJson(res.toUtf8());
    bool isNull = resDoc.isNull();
    if(!isNull) {
       QJsonArray resArray =resDoc.array();
       for(int i = 0; i< resArray.size(); i++) {
           QJsonObject resObj = resArray.at(i).toObject();
           datax = resObj.value(QString("data"));
           acList.append(datax.toString());
       }
       currentCompleterData = resArray;

    }
    currentCompleter = new QCompleter(acList, this);
    currentCompleter->setCaseSensitivity(Qt::CaseInsensitive);

    QListView* completerListView = qobject_cast<QListView*>(currentCompleter->popup());
    if (completerListView)
    {
        completerListView->setStyleSheet("background-color: #F0F0F0; color: black;");
        completerListView->setAlternatingRowColors(true);
        completerListView->setSelectionBehavior(QAbstractItemView::SelectRows);
        appFont.setPointSize(20);
        completerListView->setFont(appFont);
    }
}

QString MainWindow::getACData(QString data, QString field)
{
    for(int i = 0; i< currentCompleterData.size(); i++) {
        QJsonObject resObj = currentCompleterData.at(i).toObject();
        QJsonValue currentData = resObj.value(QString("data"));
        if(currentData.toString() == data) {
            QJsonValue currentFieldData = resObj.value(field);
            return currentFieldData.toString();
        }
    }
    return "";
}

void MainWindow::on_actionLogout_triggered()
{
    qDebug() << "* action logout";
    resetLoginForm();
    widgetModule->hide();
    widgetLogin->show();
    callConnect();
}

void MainWindow::on_actionOpen_Port_triggered()
{
    this->openSerialPort();
    this->ui->actionOpen_Port->setDisabled(true);
    this->ui->actionDisconnect_Port->setEnabled(true);
    this->ui->actionConfigure->setDisabled(true);
}

void MainWindow::on_actionConfigure_triggered()
{
    settingsDialog->show();
}

void MainWindow::on_actionDisconnect_Port_triggered()
{
    this->closeSerialPort();
    this->ui->actionDisconnect_Port->setDisabled(true);
    this->ui->actionOpen_Port->setEnabled(true);
    this->ui->actionConfigure->setEnabled(true);
    ui->actionOpen_Port->setIcon(QIcon());

}

void MainWindow::on_actionShow_Hide_Terminals_toggled(bool arg1)
{
    if(arg1) {
        console->show();
        showConsole = true;
    }
    else {
        console->hide();
        showConsole = false;
    }
}

void MainWindow::on_actionReading_triggered()
{
    setWeight("");
}

void MainWindow::on_actionStatus_triggered()
{
    QMessageBox::information (this, "Status Information", QString("%1").arg(statusConnection));
}


void MainWindow::lineEditWeightGross_onChange(QString param)
{
    qDebug() << "* change text to " << param;
    labelCallback->setText("");
}

// AutoComplete
void MainWindow::reqAutoCompleteCaseId(QString param)
{
    labelCallback->setText("");
    wtb->reqAutoComplete(controller, "case_id", param);
    connect(wtb->currentReplyAC, SIGNAL(finished()), this, SLOT(resAutoCompleteCaseId()));
}

void MainWindow::resAutoCompleteCaseId()
{
     setCurrentCompleter();
     lineEditCaseId->setCompleter(currentCompleter);
     lineEditCaseId->completer()->complete();
}

void MainWindow::reqSubmit()
{
    QString caseId = lineEditCaseId->text();
    QString Weight = lineEditWeightGross->text();
    QUrlQuery params;
    params.addQueryItem("case_id", caseId);
    params.addQueryItem("weight", Weight);
    wtb->reqSubmit(controller, params);
    connect(wtb->currentReply, SIGNAL(finished()), this, SLOT(resSubmit()));
}

void MainWindow::resSubmit()
{
    qDebug() << "* response module submit";
    QString res(wtb->currentReply->readAll());
    QJsonDocument jsonResponse = QJsonDocument::fromJson(res.toUtf8());
    bool isNull = jsonResponse.isNull();
    if(!isNull) {
        QJsonArray jsonObject = jsonResponse.array();
        qDebug() << jsonObject;
        for(int i=0; i< jsonObject.size(); i++) {
            const QJsonValue value = jsonObject[i];
            QString resString = value.toString();            
            qDebug() << resString;
            QStringList resList = resString.split(":");
            if(resList.at(0) == "success") {
                QMessageBox::information (this, "Success", QString("%1").arg(resString));
                qDebug() << resList.at(0);
                QString caseId = lineEditCaseId->text();
                QTextDocument doc;
                resetInput();
                QString param = "param=" + caseId;
                wtb->reqPdfController(controller,"get_pdf",param);           
            }
            else if(resList.at(0) == "error") {
                int errorCode = resList.at(1).toInt();
                switch ( errorCode )
                {
                    case 1:
                        break;
                    case 2:
                        lineEditCaseId->setFocus();
                        labelCallback->setText("Case Type is undefined");
                        break;
                    case 3:
                        lineEditWeightGross->setFocus();
                        labelCallback->setText("Weight Error");
                        break;
                    default:
                        break;
                }
            }
        }
    }
}
void MainWindow::slotHandlePagePrinted(bool result)
{
    Q_UNUSED(result);

    delete m_currentPrinter;
    m_currentPrinter = nullptr;
}

void MainWindow::resetInput()
{
    lineEditCaseId->clear();
    textCaseType->setText("");
    textWeightNet->setText("");
    lineEditWeightGross->clear();
    buttonSubmit->setEnabled(false);
    buttonSubmit->setStyleSheet("QPushButton { background-color: red; color: white; border-radius: 8px; }"
                                "QPushButton:hover { background-color: red; border: 2px solid red; }");
}

void MainWindow::setSiblingsCaseType(QString param)
{
    qDebug() << "Param: " << param;
    QString caseType = getACData(param, "case_type");

    qDebug() << "Case Type: " << caseType;
    textCaseType->setText(caseType);
    wtb->reqModuleGet(controller,"case_id="+param);
    connect(wtb->currentReplyGet, SIGNAL(finished()), this, SLOT(resModuleGet()));
}


void MainWindow::on_actionUpdate_triggered()
{
    reqSubmit();
}

void MainWindow::on_actionSettingServer_triggered()
{
    serviceDialog->show();
}

void MainWindow::copySelectionTableToClipboard()
{
    QList<QTableWidgetSelectionRange> selectedRanges = tableView->selectedRanges();

    if (!selectedRanges.isEmpty()) {
        QTableWidgetSelectionRange range = selectedRanges.first();

        int topRow = range.topRow();
        int bottomRow = range.bottomRow();
        int leftColumn = range.leftColumn();
        int rightColumn = range.rightColumn();

        QString text;
        for (int row = topRow; row <= bottomRow; ++row) {
            QStringList rowData;
            for (int col = leftColumn; col <= rightColumn; ++col) {
                QTableWidgetItem* item = tableView->item(row, col);
                if (item) {
                    rowData.append(item->text());
                } else {
                    rowData.append("");
                }
            }
            text += rowData.join('\t') + '\n';
        }

        QClipboard* clipboard = QGuiApplication::clipboard();
        clipboard->setText(text);
    }
}

void MainWindow::copyAllTableToClipboard()
{
    QString text;
    QStringList headerLabels;
    for (int column = 0; column < tableView->horizontalHeader()->count(); ++column) {
        QString headerLabel = tableView->model()->headerData(column, Qt::Horizontal).toString();
        headerLabels.append(headerLabel);
    }
    text += headerLabels.join('\t') + '\n';

    for (int row = 0; row < tableView->rowCount(); ++row) {
        QStringList rowData;
        for (int col = 0; col <= tableView->columnCount(); ++col) {
            QTableWidgetItem* item = tableView->item(row, col);
            if (item) {
                rowData.append(item->text());
            } else {
                rowData.append("");
            }
        }
        text += rowData.join('\t') + '\n';
    }

    QClipboard* clipboard = QGuiApplication::clipboard();
    clipboard->setText(text);
}

void MainWindow::on_actionSet_Weight_Empty_triggered()
{
    setWeight("");
}


void MainWindow::on_actionSet_Weight_1_triggered()
{
    setWeight("1");
}


void MainWindow::on_actionSet_Weight_10_triggered()
{
    setWeight("10");
}


void MainWindow::on_actionSet_Weight_100_triggered()
{
    setWeight("100");
}


void MainWindow::on_actionSet_Weight_1000_triggered()
{
    setWeight("1000");
}

