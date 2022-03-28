#ifndef LOGVIEWER_H
#define LOGVIEWER_H

#include <QDialog>

namespace Ui {
class LogViewer;
}

class LogViewer : public QDialog
{
    Q_OBJECT

public:
    explicit LogViewer(QWidget *parent = nullptr);
    ~LogViewer();

    void setLog(const QString &text);
private:
    Ui::LogViewer *ui;
};

#endif // LOGVIEWER_H
