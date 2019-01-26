#include "mainwindow.h"
#include "ui_mainwindow.h"

#include<QFileDialog>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->searchInput, &QLineEdit::returnPressed, this, &MainWindow::start_search);
    connect(ui->searchBtn, &QAbstractButton::click, this, &MainWindow::start_search);
    //connect(ui->searchButton, &QAbstractButton::clicked, this, &MainWindow::cancel);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_openBtn_clicked()
{
    QString folder = QFileDialog::getExistingDirectory(this, "select directory", QString(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    ui->folderInput->setText(folder);
}

void MainWindow::on_folderInput_textChanged(const QString &input)
{
    ui->setBtn->setEnabled(QFileInfo(input).isDir());
}

void MainWindow::on_setBtn_clicked()
{

    select_directory(ui->folderInput->text());
}

void MainWindow::select_directory(QString dir){

    QFileSystemWatcher * fsWatcher = new QFileSystemWatcher(this);
    fsWatcher->addPath(dir);

    connect(fsWatcher, &QFileSystemWatcher::fileChanged, this, &MainWindow::if_dir_changed);
    connect(fsWatcher, &QFileSystemWatcher::directoryChanged, this, &MainWindow::if_dir_changed);

    _indexer = new indexer(dir);

    connect(_indexer, &indexer::changeInputFieldStatus, this, &MainWindow::if_input_field_status_changed);
    connect(_indexer, &indexer::sendStatus, this, &MainWindow::get_status);
    connect(_indexer, &indexer::addElementToUi, this, &MainWindow::if_found);
    connect(_indexer, &indexer::notFound, this, &MainWindow::not_found);

    ui->treeWidget->header()->setSectionResizeMode(0, QHeaderView::Stretch);
    ui->treeWidget->header()->setSectionResizeMode(1, QHeaderView::ResizeToContents);

    QFuture<void> myIndexer = QtConcurrent::run(_indexer, &indexer::encode_file);
}

void MainWindow::if_dir_changed(const QString & dirName) {
    select_directory(dirName);
}

void MainWindow::if_file_changed(const QString & fileName) {
    QFileInfo fileInfo(fileName);
    qDebug() << fileInfo.dir().path();
    select_directory(fileInfo.dir().path());
}

void MainWindow::get_status(QString value, QString info) {
    if (value.size() == 0) {
        ui->statusBar->showMessage(info);
        return;
    }

    ui->statusBar->showMessage(value + info);
}

void MainWindow::not_found(QString message) {
    auto item = new QTreeWidgetItem(ui->treeWidget);
    item->setText(0, message);
    item->setText(1, QString("=("));
    ui->treeWidget->addTopLevelItem(item);
}

void MainWindow::start_search() {
    QString input = ui->searchInput->text();
    ui->treeWidget->clear();

    if (input.length() < 3) {
        ui->statusBar->showMessage("Input must have 3 or more symbols");
        return;
    }

    QFuture<void> myIndexer = QtConcurrent::run(_indexer, &indexer::search, input);
}

QPair<long long, QString> make_size_string(long long size) {
    if (size < 1024)
        return QPair<long long, QString>(size, " B");

    if (size < 1024 * 1024)
        return QPair<long long, QString>(size / 1024, " KB");

    if (size < 1024 * 1024 * 1024)
        return QPair<long long, QString>(size / (1024 * 1024), " MB");

    return QPair<long long, QString>(size / (1024 * 1024 * 1024), " GB");
}

void MainWindow::if_found(QString fileName, long long size) {
    QPair<long long, QString> goodSize = make_size_string(size);

    auto item = new QTreeWidgetItem(ui->treeWidget);
    item->setText(0, fileName);
    item->setText(1, QString::number(goodSize.first) + QString(goodSize.second));
    ui->treeWidget->addTopLevelItem(item);
}

void MainWindow::if_input_field_status_changed() {
    ui->searchInput->setPlaceholderText("Ready to use...");
    ui->searchInput->setDisabled(false);
}

void MainWindow::show_about_dialog() {
    QMessageBox::aboutQt(this);
}

void MainWindow::on_searchInput_textChanged(const QString &input)
{
    ui->searchBtn->setEnabled(input.size() > 2);
}

void MainWindow::on_searchBtn_clicked()
{
    start_search();
}
