#ifndef SERVICEDIALOG_H
#define SERVICEDIALOG_H

#include <QDialog>
#include <QSettings>
#include <QDebug>
#include <QLineEdit>

QT_USE_NAMESPACE

namespace Ui {
class ServiceDialog;
}

class ServiceDialog : public QDialog
{
    Q_OBJECT
public:    
    explicit ServiceDialog(QWidget *parent = 0);
    ~ServiceDialog();
    struct Settings {
        int serverIndex;
        QString serverAddress;

    };
    Settings settings() const;

signals:
     void applyNewConfiguration();

public slots:
private slots:
    void checkCustomServerPolicy(int idx);
    void apply();

private:
    Ui::ServiceDialog *ui;
    Settings currentSettings;
    QSettings *appData;
    void readAppData();
    void fillServerParameters();
    void updateSettings();
};

#endif // SERVICEDIALOG_H
