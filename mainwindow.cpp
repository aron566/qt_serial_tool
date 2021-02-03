#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)//单冒号(:)的作用是表示后面是初始化列表，用于将参数传递给父类的构造函数
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));    //设置本地编码对象
    ui->progressBar->setValue(1);//进度条初始化
    //setWindowOpacity(0.3);//设置透明度
    serial = new QSerialPort;//串口属性设置
    Send_Timer = new QTimer(this);//创建定时器
    Send_Timer_2 = new QTimer(this);//创建定时器2
    Send_Timer_3 = new QTimer(this);//创建定时器3
    serial->setPortName(ui->serial_num->currentText());//默认当前串口号
    serial->setBaudRate(QSerialPort::Baud115200);//默认115200波特率
    serial->setDataBits(QSerialPort::Data8);//默认8位数据位
    serial->setParity(QSerialPort::NoParity);//默认无校验
    serial->setStopBits(QSerialPort::OneStop);//默认停止位设置为1
    serial->setFlowControl(QSerialPort::NoFlowControl);//设置为无流控制
    send_mode1 = manul_send_mode|str_send_type;//发送1手动str发送模式
    send_mode2 = manul_send_mode|str_send_type;//发送2手动str发送模式
    send_mode3 = manul_send_mode|str_send_type;//发送1手动str发送模式
}

MainWindow::~MainWindow()
{
    delete serial;
    delete Send_Timer;
    delete Send_Timer_2;
    delete Send_Timer_3;
    delete ui;
}

//启动连接
void MainWindow::on_start_run_clicked()
{

    if(!serial->open(QIODevice::ReadWrite))//打开串口
    {
        //QMessageBox::warning(this,"警告","串口打开失败！");
        if(serial->isOpen())
        {
            QMessageBox message(QMessageBox::Warning,"警告","<font size='18' color='red'>串口打开失败！已连接状态！</font>", QMessageBox::Yes | QMessageBox::No, nullptr);
            message.exec();//阻塞显示，新式消息提示
        }
        QMessageBox message(QMessageBox::Warning,"警告","<font size='18' color='red'>串口打开失败！请确认连接状态</font>", QMessageBox::Yes | QMessageBox::No, nullptr);
        message.exec();//阻塞显示，新式消息提示
        return ;
    }
    qDebug() << "打开串口成功！";
    ui->link_state->setText("已连接");
    QObject::connect(serial,&QSerialPort::readyRead,this,&MainWindow::ReadData);
    QObject::connect(Send_Timer  ,SIGNAL(timeout()), this, SLOT(AUTO_Send_Data_1_loop()));
    QObject::connect(Send_Timer_2,SIGNAL(timeout()), this, SLOT(AUTO_Send_Data_2_loop()));
    QObject::connect(Send_Timer_3,SIGNAL(timeout()), this, SLOT(AUTO_Send_Data_3_loop()));
}

/**
 * @brief MainWindow::on_stop_run_clicked停止连接
 */
void MainWindow::on_stop_run_clicked()
{
    serial->close();
    ui->link_state->setText("未连接");
    QObject::disconnect(serial,&QSerialPort::readyRead,this,&MainWindow::ReadData);
    QObject::disconnect(Send_Timer  ,SIGNAL(timeout()), this, SLOT(AUTO_Send_Data_1_loop()));
    QObject::disconnect(Send_Timer_2,SIGNAL(timeout()), this, SLOT(AUTO_Send_Data_2_loop()));
    QObject::disconnect(Send_Timer_3,SIGNAL(timeout()), this, SLOT(AUTO_Send_Data_3_loop()));
}

//导出文件选择
void MainWindow::on_export_choose_stateChanged(int arg1)
{
    Q_UNUSED(arg1)
}

//导出文件类型选择
void MainWindow::on_export_file_style_activated(const QString &arg1)
{
    Q_UNUSED(arg1)
}

//串口号设置
void MainWindow::on_serial_num_activated(const QString &arg1)
{
    serial->setPortName(arg1);
    qDebug() << ui->serial_num->currentText();
}
//波特率设置
void MainWindow::on_communication_baudrate_set_activated(const QString &arg1)
{
    serial->setBaudRate(arg1.toInt());

}
//数据位设置
void MainWindow::on_data_bit_set_activated(const QString &arg1)
{

    qint32 temp = arg1.toInt();
    switch(temp)
    {
        case QSerialPort::Data8:
            serial->setDataBits(QSerialPort::Data8);
        break;
        case QSerialPort::Data7:
            serial->setDataBits(QSerialPort::Data7);
        break;
        case QSerialPort::Data6:
            serial->setDataBits(QSerialPort::Data6);
        break;
        case QSerialPort::Data5:
            serial->setDataBits(QSerialPort::Data5);
        break;
    default:
            serial->setDataBits(QSerialPort::Data8);
        break;
    }
}

//设置校验位
void MainWindow::on_serial_parity_activated(const QString &arg1)
{

    qint32 temp = arg1.toInt();
    switch(temp)
    {
        case QSerialPort::NoParity:
            serial->setParity(QSerialPort::NoParity); //校验位设置为0
        break;
        case QSerialPort::EvenParity:
            serial->setParity(QSerialPort::EvenParity);
        break;
        case QSerialPort::OddParity:
            serial->setParity(QSerialPort::OddParity);
        break;
        case QSerialPort::SpaceParity:
            serial->setParity(QSerialPort::SpaceParity);
        break;
    default:
            serial->setParity(QSerialPort::NoParity);
        break;
    }
}

//停止位设置
void MainWindow::on_stop_bit_set_activated(const QString &arg1)
{
    qint32 temp = arg1.toInt();
    switch(temp)
    {
        case QSerialPort::OneAndHalfStop:
            serial->setStopBits(QSerialPort::OneAndHalfStop);
        break;
        case QSerialPort::TwoStop:
            serial->setStopBits(QSerialPort::TwoStop);
        break;
        case QSerialPort::OneStop:
           serial->setStopBits(QSerialPort::OneStop);
        break;
    default:
            serial->setStopBits(QSerialPort::OneStop);
        break;
    }
}
//清空接收区
void MainWindow::on_receive_clear_clicked()
{
    ui->hex_rec->clear();
    ui->str_rec->clear();
    ui->receive_char_count->display("0");
    ui->receive_char_count_2->display("0");
}

/**
 * @brief MainWindow::send_data
 * @param line_edit
 */
void MainWindow::send_data(QPlainTextEdit *line_edit)
{
    int str_cout = 0;
    QString str = line_edit->toPlainText().toLatin1();
    QStringList list = str.split(" ");
    qDebug() << "数据hex：" << list;
    QString str_hex;
    int i = 0;
    for(;i < list.size();i++)
    {
        str_hex.append(list.at(i));
    }
    QByteArray hex_data = QString2Hex(str_hex);
    quint8 send_mode = 0;
    if(line_edit == ui->send_window_1)
    {
        send_mode = send_mode1;
    }
    else if(line_edit == ui->send_window_2)
    {
        send_mode = send_mode2;
    }
    else if(line_edit == ui->send_window_3)
    {
        send_mode = send_mode3;
    }

    if(serial->isOpen())
    {
        QString display_send_data;
        if(send_mode & str_send_type)
        {
            qDebug() << "进入字符串发送模式";
            QTextDocument *document=Q_NULLPTR;
            QTextBlock textBlock;
            document=line_edit->document();
            textBlock=document->begin();
            qDebug() << textBlock.text().toStdString().data();
            qDebug() << str.toLocal8Bit();

            QTextCodec *utf8 = QTextCodec::codecForName("UTF-8");
            QTextCodec* gbk = QTextCodec::codecForName("GBK");

            QString strUnicode = gbk->toUnicode(line_edit->toPlainText().toLocal8Bit().data());
            QString str_temp;
            if(ui->set_encode->checkState())//使用GBK格式发送
            {
                strUnicode = gbk->toUnicode(line_edit->toPlainText().toLocal8Bit().data());
                QByteArray gb_bytes = gbk->fromUnicode(strUnicode);
                str_cout = QString::fromLocal8Bit(gb_bytes).toLocal8Bit().count();
                serial->write(QString::fromLocal8Bit(gb_bytes).toLocal8Bit());
                QDataStream out(&gb_bytes,QIODevice::ReadWrite);//将字节数组读入16进制用
                while(!out.atEnd())
                {
                   qint8 outChar = 0;
                   out>>outChar;   //每字节填充一次，直到结束
                   //十六进制的转换
                   QString str = QString("%1").arg(outChar&0xFF,2,16,QLatin1Char('0'));
                   str_temp +=str.toUpper()+" ";//大写,每发送两个字符后添加一个空格
                }
                str = gb_bytes.data();
            }
            else
            {

                QByteArray utf8_bytes = utf8->fromUnicode(strUnicode);
                str_cout = textBlock.text().toUtf8().size();
                serial->write(textBlock.text().toStdString().data());
                QDataStream out(&utf8_bytes,QIODevice::ReadWrite);    //将字节数组读入16进制用
                while(!out.atEnd())
                {
                   qint8 outChar = 0;
                   out>>outChar;   //每字节填充一次，直到结束
                   //十六进制的转换
                   QString str = QString("%1").arg(outChar&0xFF,2,16,QLatin1Char('0'));
                   str_temp +=str.toUpper()+" ";//大写,每发送两个字符后添加一个空格
                }
                str = utf8_bytes.data();
            }
            if(ui->display_send_rec->checkState())
            {
                if(ui->display_stamp->checkState())
                {
                    ui->hex_rec->append("<font size='3' color='red'>【发送】【"+QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz")+"】 "+str_temp+"</font>");
                    ui->str_rec->append("<font size='3' color='red'>【发送】【"+QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz")+"】 "+ui->send_window_1->toPlainText()+"</font>");
                }
                else
                {
                    ui->hex_rec->append("<font size='3' color='red'>【发送】"+str_temp+"</font>");
                    ui->str_rec->append("<font size='3' color='red'>【发送】"+str+"</font>");
                }
            }
            ui->send_char_count->display(ui->send_char_count->value()+str_cout);//字符串发送计算发送长度
        }
        if(send_mode & hex_send_type)
        {
            qDebug() << "进入HEX1发送模式";
            serial->write(hex_data);
            if(ui->display_send_rec->checkState())
            {
                if(ui->display_stamp->checkState())
                {
                    ui->hex_rec->append("<font size='3' color='red'>【发送】【"+QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz")+"】 "+ui->send_window_1->toPlainText()+"</font>");
                }
                else
                {
                    ui->hex_rec->append("<font size='3' color='red'>【发送】"+str+"</font>");
                }
            }
            ui->send_char_count->display(ui->send_char_count->value()+i);//hex发送计算发送长度,字节数
        }
        ui->send_frame_count->display(ui->send_frame_count->value()+1);//发送帧数更新
        if(ui->display_send_rec->checkState())
        {
            ui->hex_rec->moveCursor(QTextCursor::End);//将接收文本框的滚动条滑到最下面
            ui->str_rec->moveCursor(QTextCursor::End);
        }
    }
}

/**
 * @brief MainWindow::calc_crc
 * @param line_edit
 */
void MainWindow::calc_crc(QPlainTextEdit *line_edit)
{
    quint16 crc_ret = 0;
    QByteArray buff;
    buff.resize(2);
    QString str = line_edit->toPlainText().toLatin1();
    QStringList list = str.split(" ");
    qDebug() << "数据hex：" << list;
    QString str_hex;
    int i = 0;
    for(;i < list.size();i++)
    {
        str_hex.append(list.at(i));
    }
    QByteArray hex_data = QString2Hex(str_hex);
    crc_ret = crc_return(reinterpret_cast<const quint8 *>(hex_data.constData()), static_cast<quint16>(i));
    buff[0] = static_cast<qint8>((crc_ret&0x00FF));
    buff[1] =static_cast<qint8>(((crc_ret>>8)&0x00FF));
    line_edit->moveCursor(QTextCursor::End);
    QString strHex;
    str.clear();
    str.sprintf("%02X",buff.at(0)&0xFF);
    line_edit->insertPlainText(" "+str);

    strHex.clear();
    strHex.sprintf("%02X",buff.at(1)&0xFF);
    line_edit->insertPlainText(" "+strHex);
}

//手动发送1
void MainWindow::on_manul_send_1_clicked()
{
    send_data(ui->send_window_1);
}
//手动发送2
void MainWindow::on_manul_send_2_clicked()
{
    send_data(ui->send_window_2);
}
//手动发送3
void MainWindow::on_manul_send_3_clicked()
{
    send_data(ui->send_window_3);
}


//扫描串口号
void MainWindow::on_scan_serial_num_clicked()
{
    //查找可用的串口
    ui->serial_num->setEnabled(false);//变灰
    qApp->processEvents();
    ui->serial_num->clear();
    foreach(const QSerialPortInfo &info,QSerialPortInfo::availablePorts())
    {
        serial->setPort(info);

            ui->serial_num->addItem(serial->portName());
            qApp->processEvents();//立即处理
            qDebug() << serial->portName();//调试
    }
    ui->serial_num->setEnabled(true);
    qApp->processEvents();
    serial->setPortName(ui->serial_num->currentText());//默认当前串口号
    qDebug() << "设置串口号为：" << ui->serial_num->currentText();//调试
}


void MainWindow::ReadData()
{
    QByteArray data,str_data;
    qint64 byteLen = serial->bytesAvailable();
    while(byteLen)
    {
        str_data = data = serial->readAll();
        if(byteLen == 2 &&  data.data()[0] == 0x0D &&  data.data()[1] == 0x0A)
        {
            return;
        }
        break;
    }

    if(!data.isEmpty())//数据不为空
    {
        qDebug() << "已接收数据：" << data;
        QString Rev_str_data_to_Window,Rev_hex_data_to_Window;
        QDataStream out(&data,QIODevice::ReadWrite);    //将字节数组读入16进制用
        QString GBK_str = QString::fromLocal8Bit( str_data );//GBK字符串
        ui->receive_char_count->display(ui->receive_char_count->value()+data.size());//接收字节计数
        ui->receive_char_count_2->display(ui->receive_char_count_2->value()+1);//接收帧计数
        if(!(int(ui->receive_char_count_2->value()) % 24) )
        {
            ui->str_rec->clear();
            ui->hex_rec->clear();
        }
        if(ui->set_encode->checkState())
        {
            str_data = GBK_str.toUtf8().data();//将接收的GBK编码转为屏幕显示所需utf8
        }

        if(ui->display_send_rec->checkState())//显示收发
        {
            Rev_str_data_to_Window += "【接收】";
            Rev_hex_data_to_Window += "【接收】";
        }
        if(ui->auto_cr->checkState())//换行
        {
            if(ui->display_stamp->checkState())//显示时间戳
            {
                Rev_str_data_to_Window += "【"+QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz")+"】 "+str_data;
                Rev_hex_data_to_Window += "【"+QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz")+"】 ";
                while(!out.atEnd())
                {
                   qint8 outChar = 0;
                   out>>outChar;   //每字节填充一次，直到结束
                  //十六进制的转换
                   QString str = QString("%1").arg(outChar&0xFF,2,16,QLatin1Char('0'));
                   Rev_hex_data_to_Window +=str.toUpper()+" ";
                   str.clear();
                }

            }
            else //不显示时间戳
            {
                Rev_str_data_to_Window +=str_data;
                while(!out.atEnd())
                {
                   qint8 outChar = 0;
                   out>>outChar;//每字节填充一次，直到结束
                  //十六进制的转换
                   QString str = QString("%1").arg(outChar&0xFF,2,16,QLatin1Char('0'));
                   Rev_hex_data_to_Window +=str.toUpper()+" ";
                   str.clear();
                }

            }
            ui->hex_rec->append("<font size='3' color='blue'>"+Rev_hex_data_to_Window+"</font>");//换行
            ui->str_rec->append("<font size='3' color='blue'>"+Rev_str_data_to_Window+"</font>");//显示接收数据
        }
        else //不换行接收数据
        {
            ui->hex_rec->setStyleSheet("QWidget{background-color: rgb(255,255,255);color: rgb(255, 0, 255);}");
            if(ui->display_stamp->checkState())//显示时间戳
            {

                QString time = QDateTime::currentDateTime().toString(Qt::ISODate);
                QDateTime dateTime = QDateTime::fromString(time, "yyyy-MM-dd hh:mm:ss.zzz");//字符串转时间
                Rev_str_data_to_Window +="【"+QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz")+"】 "+str_data;
                Rev_hex_data_to_Window +="【"+QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz")+"】 ";
                while(!out.atEnd())
                {
                   qint8 outChar = 0;
                   out>>outChar;//每字节填充一次，直到结束
                  //十六进制的转换
                   QString str = QString("%1").arg(outChar&0xFF,2,16,QLatin1Char('0'));
                   Rev_hex_data_to_Window +=str.toUpper()+" ";
                   str.clear();
                   time.clear();
                }

            }
            else //不显示时间戳
            {
                Rev_str_data_to_Window +=str_data;
                while(!out.atEnd())
                {
                   qint8 outChar = 0;
                   out>>outChar;   //每字节填充一次，直到结束
                  //十六进制的转换
                   QString str = QString("%1").arg(outChar&0xFF,2,16,QLatin1Char('0'));
                   Rev_hex_data_to_Window +=str.toUpper()+" ";
                   str.clear();
                }
            }
            ui->hex_rec->insertPlainText(Rev_hex_data_to_Window);
            ui->str_rec->insertPlainText(Rev_str_data_to_Window);
        }
        ui->hex_rec->moveCursor(QTextCursor::End);//将接收文本框的滚动条滑到最下面
        ui->str_rec->moveCursor(QTextCursor::End);//将接收文本框的滚动条滑到最下面
    }
}

void MainWindow::on_clear_char_count_clicked()
{
    ui->send_char_count->display("0");
    ui->send_frame_count->display("0");
}

//发送1自动发送延时设置
void MainWindow::on_auto_send_1_delay_ms_textChanged(const QString &arg1)
{
    int delay_time = arg1.toInt();
    if(arg1.toInt() < 5)
    {
        delay_time = 5;
        ui->auto_send_1_delay_ms->setText("5");
        qDebug() << "当前1延时参数：" << arg1 << "ms";
        return ;
    }
    if(Send_Timer->isActive())
    {
        Send_Timer->start(delay_time);
    }
    qDebug() << "当前1延时参数：" << arg1 << "ms";

}
//发送2自动发送延时设置
void MainWindow::on_auto_send_2_delay_ms_textChanged(const QString &arg1)
{
    int delay_time = arg1.toInt();
    if(arg1.toInt() < 5)
    {
        delay_time = 5;
        ui->auto_send_2_delay_ms->setText("5");
        qDebug() << "当前2延时参数：" << arg1 << "ms";
        return ;
    }
    if(Send_Timer_2->isActive())
    {
        Send_Timer_2->start(delay_time);
    }
    qDebug() << "当前2延时参数：" << arg1 << "ms";
}
//发送3自动发送延时设置
void MainWindow::on_auto_send_3_delay_ms_textChanged(const QString &arg1)
{
    int delay_time = arg1.toInt();
    if(arg1.toInt() < 5)
    {
        delay_time = 5;
        ui->auto_send_3_delay_ms->setText("5");
        qDebug() << "当前3延时参数：" << arg1 << "ms";
        return ;
    }
    if(Send_Timer_3->isActive())
    {
        Send_Timer_3->start(delay_time);
    }
    qDebug() << "当前3延时参数：" << arg1 << "ms";
}
void MainWindow::menu_process()
{
    QMessageBox msgBox;
    msgBox.setWindowTitle("Tips");
    msgBox.setText("By:Aron566.");
    msgBox.setStyleSheet("QPushButton { background-color: rgb(85, 85, 85); color: rgb(0, 0, 0); }");
    msgBox.exec();
}
char MainWindow::ConvertHexChar(char ch)
{
    if((ch >= '0') && (ch <= '9'))
        return ch-0x30;
    else if((ch >= 'A') && (ch <= 'F'))
        return ch-'A'+10;
    else if((ch >= 'a') && (ch <= 'f'))
        return ch-'a'+10;
    else return (-1);
}

QByteArray MainWindow::QString2Hex(QString str)
{
    QByteArray senddata;
    int hexdata,lowhexdata;
    int hexdatalen = 0;
    int len = str.length();
    senddata.resize(len/2);
    char lstr,hstr;
    for(int i=0; i<len; )
    {
        hstr=str[i].toLatin1();
        if(hstr == ' ')
        {
            i++;
            continue;
        }
        i++;
        if(i >= len)
            break;
        lstr = str[i].toLatin1();
        hexdata = ConvertHexChar(hstr);
        lowhexdata = ConvertHexChar(lstr);
        if((hexdata == 16) || (lowhexdata == 16))
            break;
        else
            hexdata = hexdata*16+lowhexdata;
        i++;
        senddata[hexdatalen] = static_cast<char>(hexdata);
        hexdatalen++;
    }
    senddata.resize(hexdatalen);
    return senddata;
}

void MainWindow::AUTO_Send_Data_1_loop()
{
    on_manul_send_1_clicked();
}
void MainWindow::AUTO_Send_Data_2_loop()
{
    on_manul_send_2_clicked();
}
void MainWindow::AUTO_Send_Data_3_loop()
{
    on_manul_send_3_clicked();
}

void MainWindow::on_send_1_mode_currentTextChanged(const QString &arg1)
{
    if(arg1 == "手动Str发送")
    {
        send_mode1 = manul_send_mode|str_send_type;
    }
    if(arg1 == "手动Hex发送")
    {
        send_mode1 = manul_send_mode|hex_send_type;
    }
    if(arg1 == "定时Hex发送")
    {
        send_mode1 = auto_send_mode|hex_send_type;
    }
    if(arg1 == "定时Str发送")
    {
        send_mode1 = auto_send_mode|str_send_type;
    }
    bool isActive = Send_Timer->isActive();
    if (true == isActive)
    {
        if(send_mode1 & manul_send_mode)
        {
            Send_Timer->stop();
        }
    }
    else
    {
        if(send_mode1 & auto_send_mode)
        {
            Send_Timer->start(ui->auto_send_1_delay_ms->displayText().toInt());
        }
    }
}

void MainWindow::on_send_2_mode_currentTextChanged(const QString &arg1)
{
    if(arg1 == "手动Str发送")
    {
        send_mode2 = manul_send_mode|str_send_type;
    }
    if(arg1 == "手动Hex发送")
    {
        send_mode2 = manul_send_mode|hex_send_type;
    }
    if(arg1 == "定时Hex发送")
    {
        send_mode2 = auto_send_mode|hex_send_type;
    }
    if(arg1 == "定时Str发送")
    {
        send_mode2 = auto_send_mode|str_send_type;
    }
    bool isActive = Send_Timer_2->isActive();
    if (true == isActive)
    {
        if(send_mode2 & manul_send_mode)
        {
            Send_Timer_2->stop();
        }
    }
    else
    {
        if(send_mode2 & auto_send_mode)
        {
            Send_Timer_2->start(ui->auto_send_2_delay_ms->displayText().toInt());
        }
    }
}

void MainWindow::on_send_3_mode_currentTextChanged(const QString &arg1)
{
    if(arg1 == "手动Str发送")
    {
        send_mode3 = manul_send_mode|str_send_type;
    }
    if(arg1 == "手动Hex发送")
    {
        send_mode3 = manul_send_mode|hex_send_type;
    }
    if(arg1 == "定时Hex发送")
    {
        send_mode3 = auto_send_mode|hex_send_type;
    }
    if(arg1 == "定时Str发送")
    {
        send_mode3 = auto_send_mode|str_send_type;
    }
    bool isActive = Send_Timer_3->isActive();
    if (true == isActive)
    {
        if(send_mode3 & manul_send_mode)
        {
            Send_Timer_3->stop();
        }
    }
    else
    {
        if(send_mode3 & auto_send_mode)
        {
            Send_Timer_3->start(ui->auto_send_3_delay_ms->displayText().toInt());
        }
    }
}

QString utf8ToGb2312(char *strUtf8)
{
    printf("~~~~utf8  strUtf8:%s\n", strUtf8);
    QTextCodec* utf8Codec= QTextCodec::codecForName("utf-8");
    QTextCodec* gb2312Codec = QTextCodec::codecForName("gb2312");

    QString strUnicode= utf8Codec ->toUnicode(strUtf8);
    QByteArray ByteGb2312= gb2312Codec ->fromUnicode(strUnicode);

    strUtf8= ByteGb2312.data();
    printf("~~~~~gb2312	strUtf8toGb2312:%s\n", strUtf8);
    return QString::fromLocal8Bit(strUtf8);//注意这里要fromLocal8Bit()
}

void gb2312ToUtf8(QString strGb2312)
{
    printf("@@@ gb2312 str is:%s\n",strGb2312.toLocal8Bit().data());
    QTextCodec* utf8Codec= QTextCodec::codecForName("utf-8");
    QTextCodec* gb2312Codec = QTextCodec::codecForName("gb2312");

    QString strUnicode= gb2312Codec->toUnicode(strGb2312.toLocal8Bit().data());
    QByteArray ByteUtf8= utf8Codec->fromUnicode(strUnicode);

    char *utf8code = ByteUtf8.data();
    printf("@@@ Utf8  strGb2312toUtf8:%s\n",utf8code);
}

void MainWindow::on_contact_us_triggered()
{
    menu_process();
}

void MainWindow::on_CRC16_window1_clicked()
{
    calc_crc(ui->send_window_1);
}

void MainWindow::on_CRC16_window2_clicked()
{
    calc_crc(ui->send_window_2);
}

void MainWindow::on_CRC16_window3_clicked()
{
    calc_crc(ui->send_window_3);
}
