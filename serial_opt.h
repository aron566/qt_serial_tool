#ifndef SERIAL_OPT_H
#define SERIAL_OPT_H

#include <QObject>

class serial_opt : public QObject
{
    Q_OBJECT
public:
    explicit serial_opt(QObject *parent = nullptr);

signals:

public slots:
};

#endif // SERIAL_OPT_H
