#include "readme.h"

#include <QBoxLayout>
#include <QFile>
#include <QImage>
#include <QLabel>
#include <QPixmap>
#include <QPointer>
#include <QPlainTextEdit>


ReadMe::ReadMe(QWidget *parent)
   : QDialog{parent}
{


   QFile ss("://common/main.css");
   ss.open(QFile::ReadOnly);
   setStyleSheet(ss.readAll());
   ss.close();

   QPixmap img("://readme/img/gui_nums.png");
   img = img.scaledToWidth(600, Qt::SmoothTransformation);
   QPointer<QLabel> titleP = new QLabel();
   titleP->setScaledContents(true);
   titleP->setPixmap(img);

   QFont labelfont("Consolas", 14, QFont::Monospace);
   QString legend_text {
      "\nDataMesh GUI\n\n"
      "1. Viewport\n"
      "2. Menubar / Toolbar\n"
      "3. Saved Facet Groups / Parameter Editor\n"
      "4. Shader Editors\n"
      "5. Debug Output Window"
   };

   QPointer<QLabel> titleL = new QLabel(legend_text);
   titleL->setFont(labelfont);

   QFile f("://readme/readme.txt");
   f.open(QIODevice::ReadOnly);
   QString readme_str = QTextStream(&f).readAll();
   f.close();

   QPointer<QPlainTextEdit> readme_text = new QPlainTextEdit();
   QFont readme_font("Consolas", 11, QFont::Monospace);
   readme_text->setFont(readme_font);
   readme_text->setPlainText(readme_str);
   readme_text->setReadOnly(true);

   QPointer<QHBoxLayout> legend_layout =  new QHBoxLayout();
   QPointer<QVBoxLayout> readme_layout = new QVBoxLayout();

   legend_layout->addWidget(titleP);
   legend_layout->addWidget(titleL);
   legend_layout->insertSpacing(1, 30);
   legend_layout->insertSpacing(3, 30);
   readme_layout->addLayout(legend_layout);
   readme_layout->addWidget(readme_text);
   readme_layout->insertSpacing(1, 30);
   setLayout(readme_layout);
   setWindowTitle("readme");
   setModal(false);
   setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

}
