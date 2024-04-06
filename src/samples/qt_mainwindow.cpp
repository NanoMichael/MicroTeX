#if (defined(BUILD_QT) || defined(BUILD_SKIA)) && !defined(MEM_CHECK)

#include <string>

#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSplitter>
#include <QString>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QSettings>

#include "qt_mainwindow.h"
#include "moc_qt_mainwindow.cpp"

using namespace std;

MainWindow::MainWindow(QWidget* parent)
  : QWidget(parent)
{
  int text_size = 20;

  // main layout for window
  QHBoxLayout* layout = new QHBoxLayout;

  // splitter separates left from right
  QSplitter* splitter = new QSplitter;
  layout->addWidget(splitter);

  // the TeX widget is on the left
  _texwidget = new TeXWidget(nullptr, text_size);
  _texwidget->setMinimumWidth(400);

  // these are the widgets on the right side
  // want an editor at the top and controls at bottom
  QWidget* right = new QWidget;
  QVBoxLayout* rlayout = new QVBoxLayout;
  right->setLayout(rlayout);
  _textedit = new QTextEdit;
  rlayout->addWidget(_textedit);

  // these are the controls which go at the bottom right
  QWidget* controls = new QWidget;
  QHBoxLayout* clayout = new QHBoxLayout;
  controls->setLayout(clayout);

  QLabel* label1 = new QLabel("Change text size:");
  _sizespin = new QSpinBox;
  _sizespin->setValue(text_size);
  _sizespin->setMinimum(1);
  QPushButton* next = new QPushButton("Next example");
  QPushButton* render = new QPushButton("Rendering");
  QPushButton* save = new QPushButton("Save as PDF");
  clayout->addWidget(label1);
  clayout->addWidget(_sizespin);
  clayout->addWidget(next);
  clayout->addWidget(render);
  clayout->addWidget(save);

  rlayout->addWidget(controls);

  splitter->addWidget(_texwidget);
  splitter->addWidget(right);

  setLayout(layout);

  QObject::connect(next, &QPushButton::clicked,
                   this, &MainWindow::nextClicked);
  QObject::connect(render, &QPushButton::clicked,
                   this, &MainWindow::renderClicked);
  QObject::connect(save, &QPushButton::clicked,
                   this, &MainWindow::saveClicked);
  QObject::connect(_sizespin, SIGNAL(valueChanged(int)),
                    this, SLOT(fontSizeChanged(int)));
}

void MainWindow::fontSizeChanged(int size)
{
  _texwidget->setTextSize(size);
}

void MainWindow::nextClicked()
{
  auto sample = _samples.next();
  _textedit->setText(QString::fromStdWString(sample));
  _texwidget->setLaTeX(sample);
}

void MainWindow::renderClicked()
{
  QString text = _textedit->toPlainText();
  _texwidget->setLaTeX(text.toStdWString());
}

QString MainWindow::saveAsDialog()
{
    const QString anyFile = "Any File (*)";
    const QString pdfFiles = "PDF files (*.pdf)";
    //const QString svgFiles = "Vector Graphics (*.svg)";
    QStringList filters = { anyFile, pdfFiles };

    QSettings settings;
    QFileDialog fileDialog(this,"Save As...");
    // fileDialog.setOption(QFileDialog::DontUseNativeDialog,true);
    fileDialog.restoreState(settings.value("filedialog.saveas").toByteArray());
    fileDialog.setDirectory(settings.value("filedialog.dir").toString());
    fileDialog.setAcceptMode(QFileDialog::AcceptSave);
    fileDialog.setFileMode(QFileDialog::AnyFile);
    fileDialog.setDefaultSuffix("pdf");
    fileDialog.setNameFilters(filters);
    fileDialog.selectNameFilter(pdfFiles);

    if (fileDialog.exec() != QDialog::Accepted)
        return {}; //  user cancelled

    QStringList selected = fileDialog.selectedFiles();
    if (selected.isEmpty())
        return {};

    QString fileName = selected.first();
    if (fileName.isEmpty())
        return {};

    QString suffix = QFileInfo(fileName).suffix().toLower();
    QString selectedFilter = fileDialog.selectedNameFilter();

    if (suffix.isEmpty()) {
        //  append default suffix
        suffix="pdf";
        fileName += ("."+suffix);
    }

    settings.setValue("filedialog.saveas",fileDialog.saveState());
    settings.setValue("filedialog.dir",fileDialog.directory().absolutePath());
    return fileName;
}

void MainWindow::saveClicked()
{
#ifdef BUILD_SKIA
  _texwidget->saveSVG("out.svg");
#else
    QString fileName = saveAsDialog();
    if (!fileName.isEmpty())
        _texwidget->savePDF(fileName);
#endif
}

#endif
