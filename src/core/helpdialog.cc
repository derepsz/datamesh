#include "helpdialog.h"

#include <QFile>
#include <QFont>

HelpDialog::HelpDialog(QString labelstr, QString textstr, QSize size, QWidget *parent)
   : QDialog(parent)
   , label("")
   , textbox("")
   , layout()
{
   QFile ss("://common/main.css");
   ss.open(QFile::ReadOnly);
   setStyleSheet(ss.readAll());
   ss.close();

   QFont textfont("Consolas", 9, QFont::Monospace);
   textbox.setFont(textfont);
   QFont labelfont("Consolas", 11, QFont::Monospace);
   label.setFont(labelfont);

   label.setText(labelstr);
   textbox.setPlainText(textstr);
   textbox.setReadOnly(true);

   if(label.text() != "") {
      layout.addWidget(&label);
   }
   if (textbox.toPlainText() != "") {
      layout.addWidget(&textbox);
   }
   setLayout(&layout);
   setMinimumSize(size);
}
