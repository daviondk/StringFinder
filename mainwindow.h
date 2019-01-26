#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <indexer.h>
#include <QDesktopWidget>
#include <QFileDialog>
#include <QDir>
#include <QFileSystemWatcher>
#include <QCommonStyle>
#include <QMessageBox>
#include <indexer.h>
#include <QtConcurrent/QtConcurrent>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_openBtn_clicked();
    void on_folderInput_textChanged(const QString &arg1);
    void on_setBtn_clicked();
    void on_searchInput_textChanged(const QString &arg1);
    void on_searchBtn_clicked();

    void if_dir_changed(const QString & dirName);
    void if_file_changed(const QString & fileName);
    void if_found(QString fileName, long long size);
    void if_input_field_status_changed();

    void get_status(QString value, QString info);
    void not_found(QString message);
    void start_search();
    void select_directory(QString dir);
    void show_about_dialog();

private:
    Ui::MainWindow *ui;
    indexer * _indexer;
};

#endif // MAINWINDOW_H
