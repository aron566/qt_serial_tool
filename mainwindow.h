#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSerialPort/QSerialPort>         // 提供访问串口的功能
#include <QtSerialPort/QSerialPortInfo>     // 提供系统中存在的串口的信息
#include <QList>
#include <QDebug>
#include <QMessageBox>
#include <QByteArray>
#include <QTimer>
#include <QAction>
#include <QString>
#include <QDateTime>
#include <QTextCodec>
#include <QTextBlock>
#include <QTextDocument>
#include <QPlainTextEdit>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    QString utf8ToGb2312(char *strUtf8);
    quint16 crc_cal(quint16 data, quint16 GenPoly, quint16 crc_data)
    {
        quint16 TmpI;
        data *= 2;
        for (TmpI = 8; TmpI > 0; TmpI--) {
            data = data / 2;
            if ((data ^ crc_data) & 1)
                crc_data = (crc_data / 2) ^ GenPoly;
            else
                crc_data /= 2;
        }
        return crc_data;
    }

    quint16 crc_return(const quint8 *crc_buf, quint16 crc_len)
    {
            quint16 temp;
            quint16 ret = 0xFFFF;
            for (temp = 0; temp < crc_len; temp++)
            {
                ret = crc_cal(crc_buf[temp], 0xA001, ret);
            }
            return ret;
    }

private slots:
    void ReadData();

    void on_start_run_clicked();

    void on_stop_run_clicked();

    void on_export_choose_stateChanged(int arg1);

    void on_export_file_style_activated(const QString &arg1);

    void on_serial_num_activated(const QString &arg1);

    void on_communication_baudrate_set_activated(const QString &arg1);

    void on_data_bit_set_activated(const QString &arg1);

    void on_stop_bit_set_activated(const QString &arg1);

    void on_receive_clear_clicked();

    void on_manul_send_1_clicked();

    void on_manul_send_2_clicked();

    void on_manul_send_3_clicked();

    void on_scan_serial_num_clicked();

    void on_serial_parity_activated(const QString &arg1);

    void on_clear_char_count_clicked();

    void on_auto_send_1_delay_ms_textChanged(const QString &arg1);

    void on_auto_send_2_delay_ms_textChanged(const QString &arg1);

    void on_auto_send_3_delay_ms_textChanged(const QString &arg1);

    void menu_process();

    void AUTO_Send_Data_1_loop();//自动发送启动
    void AUTO_Send_Data_2_loop();
    void AUTO_Send_Data_3_loop();
    void on_send_1_mode_currentTextChanged(const QString &arg1);

    void on_send_2_mode_currentTextChanged(const QString &arg1);

    void on_send_3_mode_currentTextChanged(const QString &arg1);

    void on_contact_us_triggered();

    void on_CRC16_window1_clicked();

    void on_CRC16_window2_clicked();

    void on_CRC16_window3_clicked();

private:
    Ui::MainWindow *ui;
    QSerialPort *serial;//全局串口对象
    QTimer *Send_Timer;//定时器1
    QTimer *Send_Timer_2;//定时器2
    QTimer *Send_Timer_3;//定时器3
    quint8 send_mode1 = 0;
    quint8 send_mode2 = 0;
    quint8 send_mode3 = 0;
    enum send_data_type
    {
       hex_send_type = 8,
       str_send_type = 4
    };
    enum send_data_mode
    {
       auto_send_mode = 2,
       manul_send_mode = 1
    };
    QByteArray QString2Hex(QString str);
    char ConvertHexChar(char ch);
    void send_data(QPlainTextEdit *line_edit);
    void calc_crc(QPlainTextEdit *line_edit);
};


#endif // MAINWINDOW_H
