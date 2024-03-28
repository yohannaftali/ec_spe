#include "servicedialog.h"
#include "ui_servicedialog.h"

QT_USE_NAMESPACE

ServiceDialog::ServiceDialog(QWidget *parent) : QDialog(parent),
     ui(new Ui::ServiceDialog)
{
    ui->setupUi(this);    
    ui->serverBox->setInsertPolicy(QComboBox::NoInsert);
    connect(ui->applyButton, SIGNAL(clicked()), this, SLOT(apply()));
    connect(ui->serverBox, SIGNAL(currentIndexChanged(int)), this, SLOT(checkCustomServerPolicy(int)));
    fillServerParameters();
    appData = new QSettings("WTB","wtbAppEcInd231Service");
    readAppData();
}

ServiceDialog::~ServiceDialog()
{
    delete ui;
}

ServiceDialog::Settings ServiceDialog::settings() const
{
    return currentSettings;
}

void ServiceDialog::checkCustomServerPolicy(int idx)
{
    //QRegExp rx("/^(https?:\/\/)?([\da-z\.-]+)\.([a-z\.]{2,6})([\/\w \.-]*)*\/?$/");
    //QValidator *validator = new QRegExpValidator(rx, this);
    bool isCustomServerPolicy = !ui->serverBox->itemData(idx).isValid();
    ui->serverBox->setEditable(isCustomServerPolicy);
    if (isCustomServerPolicy) {
        ui->serverBox->clearEditText();
        //QLineEdit *edit = ui->serverBox->lineEdit();
       // edit->setValidator(validator);
    }
}

void ServiceDialog::fillServerParameters()
{
    ui->serverBox->addItem(QStringLiteral("http://203.174.11.12:4431"), "http://203.174.11.12:4431");
    ui->serverBox->addItem(QStringLiteral("Custom"));
}

void ServiceDialog::readAppData()
{
    qDebug() << "read server setting";

    currentSettings.serverIndex = appData->value("server/index").toInt();
    ui->serverBox->setCurrentIndex(currentSettings.serverIndex);
    currentSettings.serverAddress = appData->value(QString("server/value")).toString();
    qDebug() << "* current server index " << currentSettings.serverIndex;
    if(currentSettings.serverIndex == 1)
    {
        ui->serverBox->setCurrentText(currentSettings.serverAddress);
    }
    qDebug() << "* current server address " << currentSettings.serverAddress;
}

void ServiceDialog::updateSettings()
{
    currentSettings.serverIndex = ui->serverBox->currentIndex();
    appData->setValue("server/index", currentSettings.serverIndex);

    if(ui->serverBox->currentIndex() == 1)
    {
        currentSettings.serverAddress = ui->serverBox->currentText();
    }
    else
    {
        currentSettings.serverAddress = ui->serverBox->itemData(ui->serverBox->currentIndex()).toString();
    }
    qDebug()<< "currentSettings.serverAddress:" <<  currentSettings.serverAddress;
    appData->setValue("server/value", currentSettings.serverAddress);
}

void ServiceDialog::apply()
{
    updateSettings();
    hide();
    applyNewConfiguration();
}
