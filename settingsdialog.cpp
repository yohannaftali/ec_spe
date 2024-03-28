/****************************************************************************
**
** Copyright (C) 2012 Denis Shienkov <denis.shienkov@gmail.com>
** Copyright (C) 2012 Laszlo Papp <lpapp@kde.org>
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtSerialPort module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "settingsdialog.h"
#include "ui_settingsdialog.h"

#include <QtSerialPort/QSerialPortInfo>
#include <QIntValidator>
#include <QLineEdit>

QT_USE_NAMESPACE

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);

    intValidator = new QIntValidator(0, 4000000, this);

    ui->baudRateBox->setInsertPolicy(QComboBox::NoInsert);

    connect(ui->applyButton, SIGNAL(clicked()),
            this, SLOT(apply()));
    connect(ui->serialPortInfoListBox, SIGNAL(currentIndexChanged(int)),
            this, SLOT(showPortInfo(int)));
    connect(ui->baudRateBox, SIGNAL(currentIndexChanged(int)),
            this, SLOT(checkCustomBaudRatePolicy(int)));

    fillPortsParameters();
    fillPortsInfo();
    appData = new QSettings("WTB","wtbAppPackingCase");
    readAppData();
   // updateSettings();
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

SettingsDialog::Settings SettingsDialog::settings() const
{
    return currentSettings;
}

void SettingsDialog::showPortInfo(int idx)
{
    if (idx != -1) {
        QStringList list = ui->serialPortInfoListBox->itemData(idx).toStringList();
        ui->descriptionLabel->setText(tr("Description: %1").arg(list.at(1)));
        ui->manufacturerLabel->setText(tr("Manufacturer: %1").arg(list.at(2)));
        ui->serialNumberLabel->setText(tr("Serial number: %1").arg(list.at(3)));
        ui->locationLabel->setText(tr("Location: %1").arg(list.at(4)));
        ui->vidLabel->setText(tr("Vendor Identifier: %1").arg(list.at(5)));
        ui->pidLabel->setText(tr("Product Identifier: %1").arg(list.at(6)));
    }
}

void SettingsDialog::apply()
{
    updateSettings();
    hide();
    applyNewConfiguration();
}

void SettingsDialog::checkCustomBaudRatePolicy(int idx)
{
    bool isCustomBaudRate = !ui->baudRateBox->itemData(idx).isValid();
    ui->baudRateBox->setEditable(isCustomBaudRate);
    if (isCustomBaudRate) {
        ui->baudRateBox->clearEditText();
        QLineEdit *edit = ui->baudRateBox->lineEdit();
        edit->setValidator(intValidator);
    }
}

void SettingsDialog::fillPortsParameters()
{
    ui->baudRateBox->addItem(QStringLiteral("9600"), QSerialPort::Baud9600);
    ui->baudRateBox->addItem(QStringLiteral("19200"), QSerialPort::Baud19200);
    ui->baudRateBox->addItem(QStringLiteral("38400"), QSerialPort::Baud38400);
    ui->baudRateBox->addItem(QStringLiteral("115200"), QSerialPort::Baud115200);
    ui->baudRateBox->addItem(QStringLiteral("Custom"));

    ui->dataBitsBox->addItem(QStringLiteral("5"), QSerialPort::Data5);
    ui->dataBitsBox->addItem(QStringLiteral("6"), QSerialPort::Data6);
    ui->dataBitsBox->addItem(QStringLiteral("7"), QSerialPort::Data7);
    ui->dataBitsBox->addItem(QStringLiteral("8"), QSerialPort::Data8);
    ui->dataBitsBox->setCurrentIndex(3);

    ui->parityBox->addItem(QStringLiteral("None"), QSerialPort::NoParity);
    ui->parityBox->addItem(QStringLiteral("Even"), QSerialPort::EvenParity);
    ui->parityBox->addItem(QStringLiteral("Odd"), QSerialPort::OddParity);
    ui->parityBox->addItem(QStringLiteral("Mark"), QSerialPort::MarkParity);
    ui->parityBox->addItem(QStringLiteral("Space"), QSerialPort::SpaceParity);

    ui->stopBitsBox->addItem(QStringLiteral("1"), QSerialPort::OneStop);
#ifdef Q_OS_WIN
    ui->stopBitsBox->addItem(QStringLiteral("1.5"), QSerialPort::OneAndHalfStop);
#endif
    ui->stopBitsBox->addItem(QStringLiteral("2"), QSerialPort::TwoStop);

    ui->flowControlBox->addItem(QStringLiteral("None"), QSerialPort::NoFlowControl);
    ui->flowControlBox->addItem(QStringLiteral("RTS/CTS"), QSerialPort::HardwareControl);
    ui->flowControlBox->addItem(QStringLiteral("XON/XOFF"), QSerialPort::SoftwareControl);

    ui->unitcomboBox->addItem(QStringLiteral("KG"));
    ui->unitcomboBox->addItem(QStringLiteral("GR"));
}

void SettingsDialog::fillPortsInfo()
{
    ui->serialPortInfoListBox->clear();
    static const QString blankString = QObject::tr("N/A");
    QString description;
    QString manufacturer;
    QString serialNumber;
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        QStringList list;
        description = info.description();
        manufacturer = info.manufacturer();
        serialNumber = info.serialNumber();
        list << info.portName()
             << (!description.isEmpty() ? description : blankString)
             << (!manufacturer.isEmpty() ? manufacturer : blankString)
             << (!serialNumber.isEmpty() ? serialNumber : blankString)
             << info.systemLocation()
             << (info.vendorIdentifier() ? QString::number(info.vendorIdentifier(), 16) : blankString)
             << (info.productIdentifier() ? QString::number(info.productIdentifier(), 16) : blankString);

        ui->serialPortInfoListBox->addItem(list.first(), list);
    }
}

void SettingsDialog::readAppData()
{
    // name
    currentSettings.name = appData->value(QString("port/name")).toString();    
    ui->serialPortInfoListBox->setCurrentText(currentSettings.name);

    // bautRate
    currentSettings.bautRateBoxIndex = appData->value("port/baudRate/index").toInt();
    ui->baudRateBox->setCurrentIndex(currentSettings.bautRateBoxIndex);

    currentSettings.baudRate = static_cast<QSerialPort::BaudRate>(appData->value(QString("port/baudRate/value")).toInt());
    currentSettings.stringBaudRate = appData->value(QString("port/baudRate/string")).toString();
    if(currentSettings.bautRateBoxIndex == 4)
    {
        ui->baudRateBox->setCurrentText(currentSettings.stringBaudRate);
    }

    // dataBits
    currentSettings.dataBitsBoxIndex = appData->value("port/dataBits/index").toInt();
    ui->dataBitsBox->setCurrentIndex(currentSettings.dataBitsBoxIndex);

    currentSettings.dataBits = static_cast<QSerialPort::DataBits>(appData->value(QString("port/dataBits/value")).toInt());
    currentSettings.stringDataBits = appData->value(QString("port/dataBits/string")).toString();

    // parity
    currentSettings.parityBoxIndex = appData->value("port/parity/index").toInt();
    ui->parityBox->setCurrentIndex(currentSettings.parityBoxIndex);

    currentSettings.parity = static_cast<QSerialPort::Parity>(appData->value(QString("port/parity/value")).toInt());
    currentSettings.stringParity = appData->value(QString("port/parity/string")).toString();

    // stopBits
    currentSettings.stopBitsBoxIndex = appData->value("port/stopBits/index").toInt();
    ui->stopBitsBox->setCurrentIndex(currentSettings.stopBitsBoxIndex);

    currentSettings.stopBits = static_cast<QSerialPort::StopBits>(appData->value(QString("port/stopBits/value")).toInt());
    currentSettings.stringStopBits = appData->value(QString("port/stopBits/string")).toString();


    // flowControl
    currentSettings.flowControlBoxIndex = appData->value("port/flowControl/index").toInt();
    ui->flowControlBox->setCurrentIndex(currentSettings.flowControlBoxIndex);

    currentSettings.flowControl =  static_cast<QSerialPort::FlowControl>(appData->value(QString("port/flowControl/value")).toInt());
    currentSettings.stringFlowControl = appData->value(QString("port/stopBits/string")).toString();

    // locahEchoEnabled
    currentSettings.localEchoEnabled = appData->value(QString("port/localEcho/bool")).toBool();
    ui->localEchoCheckBox->setChecked(currentSettings.localEchoEnabled);

}

void SettingsDialog::updateSettings()
{
    currentSettings.name = ui->serialPortInfoListBox->currentText();
    appData->setValue("port/name", currentSettings.name);

    // bautRate
    currentSettings.bautRateBoxIndex = ui->baudRateBox->currentIndex();
    appData->setValue("port/baudRate/index", currentSettings.bautRateBoxIndex);

    if(ui->baudRateBox->currentIndex() == 4)
    {
        currentSettings.baudRate = static_cast<QSerialPort::BaudRate>(ui->baudRateBox->currentText().toInt());
    }
    else
    {
        currentSettings.baudRate = static_cast<QSerialPort::BaudRate>(ui->baudRateBox->itemData(ui->baudRateBox->currentIndex()).toInt());
    }
    appData->setValue("port/baudRate/value", currentSettings.baudRate);

    currentSettings.stringBaudRate = QString::number(currentSettings.baudRate);
    appData->setValue("port/baudRate/string", currentSettings.stringBaudRate);

    // dataBits
    currentSettings.dataBitsBoxIndex = ui->dataBitsBox->currentIndex();
    appData->setValue("port/dataBits/index", currentSettings.dataBitsBoxIndex );

    currentSettings.dataBits = static_cast<QSerialPort::DataBits>(ui->dataBitsBox->itemData(ui->dataBitsBox->currentIndex()).toInt());
    appData->setValue("port/dataBits/value", currentSettings.dataBits );

    currentSettings.stringDataBits = ui->dataBitsBox->currentText();
    appData->setValue("port/dataBits/string", currentSettings.stringDataBits );

    // parity
    currentSettings.parityBoxIndex = ui->parityBox->currentIndex();
    appData->setValue("port/parity/index", currentSettings.parityBoxIndex );

    currentSettings.parity = static_cast<QSerialPort::Parity>(ui->parityBox->itemData(ui->parityBox->currentIndex()).toInt());
    appData->setValue("port/parity/value", currentSettings.parity );

    currentSettings.stringParity = ui->parityBox->currentText();
    appData->setValue("port/parity/string", currentSettings.stringParity );

    // stopBits
    currentSettings.stopBitsBoxIndex = ui->stopBitsBox->currentIndex();
    appData->setValue("port/stopBits/index", currentSettings.stopBitsBoxIndex );

    currentSettings.stopBits = static_cast<QSerialPort::StopBits>(
                ui->stopBitsBox->itemData(ui->stopBitsBox->currentIndex()).toInt());
    appData->setValue("port/stopBits/value", currentSettings.stopBits );

    currentSettings.stringStopBits = ui->stopBitsBox->currentText();
    appData->setValue("port/stopBits/string", currentSettings.stringStopBits );

    // flow control
    currentSettings.flowControlBoxIndex = ui->flowControlBox->currentIndex();
    appData->setValue("port/flowControl/index", currentSettings.flowControlBoxIndex );

    currentSettings.flowControl = static_cast<QSerialPort::FlowControl>(
                ui->flowControlBox->itemData(ui->flowControlBox->currentIndex()).toInt());
    appData->setValue("port/flowControl/value", currentSettings.flowControl );

    currentSettings.stringFlowControl = ui->flowControlBox->currentText();
    appData->setValue("port/flowControl/string", currentSettings.stringFlowControl );

    // localEchoEnabled
    currentSettings.localEchoEnabled = ui->localEchoCheckBox->isChecked();
    appData->setValue("port/localEcho/bool", currentSettings.localEchoEnabled );
}
