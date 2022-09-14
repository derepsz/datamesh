#ifndef HELPDIALOG_H
#define HELPDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QPlainTextEdit>
#include <QBoxLayout>

class HelpDialog : public QDialog
{
      Q_OBJECT
   public:
      HelpDialog(QString, QString, QSize size, QWidget *parent = nullptr);
      QLabel label;
      QPlainTextEdit textbox;
      QVBoxLayout layout;

   private:

};

#endif // HELPDIALOG_H
