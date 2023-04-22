#ifndef TEXTBROWSER_H
#define TEXTBROWSER_H

#include <QDialog>

namespace Ui {
class TextBrowser;
}

class TextBrowser : public QDialog
{
    Q_OBJECT

public:
    explicit TextBrowser(const QString &title,QWidget *parent = nullptr);
    ~TextBrowser();

    void setText(const QString &text);
    void setMarkdown(const QString &markdown);
private:
    Ui::TextBrowser *ui;
};

#endif // TEXTBROWSER_H
