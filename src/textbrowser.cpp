#include "textbrowser.h"
#include "ui_textbrowser.h"
#include <QScrollBar>

TextBrowser::TextBrowser(const QString &title, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TextBrowser)
{
    setWindowTitle(title);
    ui->setupUi(this);
#if defined(Q_OS_WIN)
    setFixedWidth(600);
#else
    setFixedWidth(geometry().width());
#endif
    ui->textBrowser->document()->setDefaultStyleSheet("a{outline:none;text-decoration:underline;}");
}

TextBrowser::~TextBrowser()
{
    delete ui;
}

void TextBrowser::setText(const QString &text)
{
    ui->textBrowser->setPlainText(text);
    QScrollBar* verticalScrollBar = ui->textBrowser->verticalScrollBar();
    verticalScrollBar->setValue(verticalScrollBar->maximum());
}

void TextBrowser::setMarkdown(const QString &markdown)
{
    ui->textBrowser->setMarkdown(markdown);
    ui->textBrowser->setHtml(ui->textBrowser->document()->toHtml());
}
