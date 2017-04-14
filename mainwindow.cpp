#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_lineEditList
        << ui->le1 << ui->le2 << ui->le3 << ui->le4 << ui->le5 << ui->le6
        << ui->le7 << ui->le8 << ui->le9 << ui->le10 << ui->le11 << ui->le12;

    m_pushButtonList
        << ui->pb1 << ui->pb2 << ui->pb3 << ui->pb4 << ui->pb5 << ui->pb6
        << ui->pb7 << ui->pb8 << ui->pb9 << ui->pb10 << ui->pb11 << ui->pb12;

    QList<int> portNumList;
    foreach (QSerialPortInfo portInfo, QSerialPortInfo::availablePorts()) {
        portNumList.append(portInfo.portName().remove(0, 3).toInt());
    }
    qSort(portNumList);
    foreach (int portNum, portNumList) {
        ui->cbPort->addItem(QString("COM%1").arg(portNum));
    }

    for (int i = 0; i < m_pushButtonList.size(); ++i) {
        m_pushButtonList[i]->setShortcut(Qt::Key_F1 + i);
        connect(m_pushButtonList[i], &QPushButton::clicked, [this, i]() { m_signalMapper.map(m_pushButtonList[i]); });
        m_signalMapper.setMapping(m_pushButtonList[i], i);
    }
    connect(&m_signalMapper, static_cast<void (QSignalMapper::*)(int)>(&QSignalMapper::mapped), this, &MainWindow::SwitchSlot);

    foreach (QLineEdit* LineEdit, m_lineEditList) {
        LineEdit->installEventFilter(this);
    }

    connect(ui->pbPing, &QPushButton::clicked, this, &MainWindow::Ping);

    loadSettings();

    m_watcher.addPath("myStyle.qss");
    connect(&m_watcher, SIGNAL(fileChanged(const QString&)), this, SLOT(setAppStyleSheet()));


}

MainWindow::~MainWindow()
{
    saveSettings();
    delete ui;
}

void MainWindow::saveSettings()
{
    QSettings settings("KdsSwitcher.ini", QSettings::IniFormat);
    settings.setIniCodec("UTF-8");
    settings.beginGroup("PortSettings");
    settings.setValue("Port", ui->cbPort->currentIndex());
    settings.setValue("Baud", ui->cbBaud->currentIndex());
    settings.setValue("Address", ui->sbAddress->value());
    settings.endGroup();

    settings.beginWriteArray("PointTypes");
    for (int i = 0; i < ui->cbType->count(); ++i) {
        settings.setArrayIndex(i);
        settings.setValue("Type", ui->cbType->itemText(i));
    }
    settings.endArray();

    settings.beginGroup(QString("PointType"));
    settings.setValue("CurrentType", ui->cbType->currentIndex());
    settings.endGroup();

    settings.beginWriteArray(QString("Points%1").arg(m_lastPointType));
    for (int i = 0; i < 12; ++i) {
        settings.setArrayIndex(i);
        settings.setValue("Parcel", m_points[i].Parcel);
        settings.setValue("Description", m_points[i].Description);
    }

    m_lastPointType = ui->cbType->currentIndex() + 1;

    settings.endArray();
}

void MainWindow::loadSettings()
{
    QSettings settings("KdsSwitcher.ini", QSettings::IniFormat);
    settings.setIniCodec("UTF-8");
    settings.beginGroup("PortSettings");
    ui->cbPort->setCurrentIndex(settings.value("Port", 0).toInt());
    ui->cbBaud->setCurrentIndex(settings.value("Baud", 0).toInt());
    ui->sbAddress->setValue(settings.value("Address", 0).toInt());
    settings.endGroup();

    disconnect(ui->cbType, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &MainWindow::CbTypeCurrentIndexChanged);
    int size = settings.beginReadArray("PointTypes");
    ui->cbType->clear();
    for (int i = 0; i < size; ++i) {
        settings.setArrayIndex(i);
        ui->cbType->addItem(settings.value("Type").toString());
    }
    settings.endArray();

    settings.beginGroup(QString("PointType"));
    ui->cbType->setCurrentIndex(settings.value("CurrentType", 0).toInt());
    settings.endGroup();
    connect(ui->cbType, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &MainWindow::CbTypeCurrentIndexChanged);

    m_lastPointType = ui->cbType->currentIndex() + 1;

    size = settings.beginReadArray(QString("Points%1").arg(m_lastPointType));
    for (int i = 0; i < size; ++i) {
        settings.setArrayIndex(i);
        m_points[i].Parcel = settings.value("Parcel", "").toString();
        m_points[i].Description = settings.value("Description", "").toString();
        m_lineEditList[i]->setText(m_points[i].Description);
    }
    settings.endArray();
}

void MainWindow::sl()
{
    //    if (kds.CpOpen(ui->cbPort->currentIndex(), 300, EV_RXFLAG, '\r') > 0) {
    //        /*
    //        :0;40;0;21173
    //        я!0;$0;63612
    //        :0;42;20;21517
    //        я!0;00FF7F4700FF7F4700FF7F4700FF7F4700FF7F47;7751
    //        :0;42;20;21517
    //        я!0;00FF7F4700FF7F4700FF7F4700FF7F4700FF7F47;7751
    //        :0;42;20;21517
    //        я!0;4B59863C8D1721420F0BA142A3E21D4377DD9D43;19759
    //        :0;42;20;21517
    //        я!0;6C09F93A2B1821426617A142B7F21D43DEE39D43;61965
    //        :0;42;20;21517
    //        я!0;52499D3BD88121424783A14200F01D438BFC9D43;18737
    //        :0;42;20;21517
    //        я!0;E71D273CB5152142EEEBA042EAF41D4356EE9D43;6215
    //        :0;42;20;21517
    //        я!0;C520B03CA9132142E209A14217F81D4371049E43;42450
    //        :0;42;20;21517
    //        я!0;52491D3C59062142E60EA142F4CC1D43700E9E43;23683
    //        :0;42;20;21517
    //        я!0;4D158C3CC62D214222FDA04279F81D43F8EA9D43;52285
    //        :0;42;20;21517
    //        я!0;24977F3B88852142F290A14234111E43AB0E9E43;10317
    //        :0;42;20;21517
    //        я!0;2497FF3BCA832142077FA14219021E4374F49D43;23225
    //        :0;42;20;21517
    //        я!0;70CE883C910F21426C09A14291ED1D4320F99D43;24907*/

    //        int k;
    //        char ch[100];
    //        vector<char> data;
    //        //i = sprintf(ch, ":0;0;%d\r", KDS::CalcCrc(":0;0;"));

    //        k = sprintf(ch, ":0;40;0;21173\r");
    //        kds.CpWrite(ch, k);
    //        qDebug() << ch;
    //        kds.CpReadWait(data, 1000); //1 sec
    //        qDebug() << data.data();

    //        for (int i = 0; i < 12; i++) {
    //            k = sprintf(ch, ":0;42;20;21517\r");
    //            kds.CpWrite(ch, k);
    //            qDebug() << ch;
    //            kds.CpReadWait(data, 1000); //1 sec
    //            qDebug() << data.data();
    //            QByteArray ar;
    //            ar = ar.fromHex(ar.fromRawData(data.data() + 4, 40));
    //            float* f = (float*)ar.data();
    //            for (int j = 0; j < 5; j++) {
    //                qDebug() << i + 1 << j << f[j];
    //            }
    //        }
    //        kds.CpClose();
    //    }
}

void MainWindow::Ping()
{
    m_kds.setPortName(ui->cbPort->currentText());
    m_kds.setBaudRate(ui->cbBaud->currentText().toInt());
    if (m_kds.getDev(ui->sbAddress->value()) == 25) {
        ui->statusBar->showMessage("Связь с прибором КДС установлена!", 1000);
    }
    else {
        ui->statusBar->showMessage("Нет связи с прибором КДС!", 1000);
    }
}

void MainWindow::setAppStyleSheet()
{
    QFile file("myStyle.qss");
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Cannot open stylesheet file "
                 << "myStyle.qss";
        return;
    }
    else {
        qDebug() << "Open stylesheet file "
                 << "myStyle.qss";
    }
    QString stylesheet = QString::fromUtf8(file.readAll());
    qApp->setStyleSheet(stylesheet);
}

void MainWindow::SwitchSlot(int pointNum)
{
    m_numPoint = pointNum;
    m_kds.setPortName(ui->cbPort->currentText());
    m_kds.setBaudRate(ui->cbBaud->currentText().toInt());
    if (m_kds.setOut(ui->sbAddress->value(), m_points[m_numPoint].Parcel.toInt())) {
        ui->statusBar->showMessage("Прибором КДС успешно переключен!", 1000);
    }
    else {
        ui->statusBar->showMessage("Нет связи с прибором КДС!", 1000);
    }
}

#include <QKeyEvent>

bool MainWindow::eventFilter(QObject* obj, QEvent* event)
{
    if (event->type() == QEvent::MouseButtonDblClick) {
        m_numPoint = m_lineEditList.indexOf(reinterpret_cast<QLineEdit*>(obj));
        if (m_numPoint > -1) {
            PointEdit pe(&m_points[m_numPoint], m_lineEditList[m_numPoint], this);
            pe.exec();
            return true;
        }
    }
    return QMainWindow::eventFilter(obj, event);
}

void MainWindow::CbTypeCurrentIndexChanged(int index)
{
    MainWindow::saveSettings();
    MainWindow::loadSettings();
}
