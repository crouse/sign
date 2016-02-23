#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->tableViewChoose->hide();
    ui->dateTimeEdit->setDateTime(QDateTime::currentDateTime());
    setDatabase();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setDatabase()
{
    database = QSqlDatabase::addDatabase("QSQLITE");
    QString dbpath;
#ifdef Q_OS_MAC
QFileInfo macPath("/Volumes/sign/database/");
if (macPath.isDir()) {
    dbpath = QString("%1/sign.db").arg(macPath.path());
} else {
    dbpath = "/tmp/sign.db";
}
#else
dbpath = "sign.db";
#endif

    database.setDatabaseName(dbpath);
    if (!database.open()) {
        qDebug() << "Can not open sign.db";
        QMessageBox::information(this, "", "无法连接数据库");
    } else {
        QSqlQuery query;
        QString createTableSql = QString("Create table sign ("
                                         " name varchar(32), "
                                         " gender varchar(4), "
                                         " phone varchar(32), "
                                         " birthday datetime(4), "
                                         " logdate datetime"
                                         ");");
        query.exec(createTableSql);
        qDebug() << createTableSql;
        database.commit();
        setModel();
    }
}

int MainWindow::setModel()
{
    model = new QSqlTableModel(this);
    model->setTable("sign");
    model->setEditStrategy(QSqlTableModel::OnFieldChange);
    model->setSort(0, Qt::AscendingOrder);
    model->setHeaderData(0, Qt::Horizontal, "姓名");
    model->setHeaderData(1, Qt::Horizontal, "性别");
    model->setHeaderData(2, Qt::Horizontal, "手机");
    model->setHeaderData(3, Qt::Horizontal, "生日");
    model->setHeaderData(4, Qt::Horizontal, "录入时间");

    ui->tableView->setModel(model);
    ui->tableView->alternatingRowColors();
    ui->tableView->horizontalHeader()->setStretchLastSection(true);
    model->select();
    ui->tableView->reset();
    return true;
}

void MainWindow::on_pushButtonOK_clicked()
{
    QString name = ui->lineEditName->text().trimmed();
    QString gender = ui->comboBoxGender->currentText();
    QString phone = ui->lineEditPhone->text().trimmed();
    QString birthday = ui->lineEditBirth->text().trimmed();
    QDate logdate = ui->dateTimeEdit->date();
    QString date = logdate.toString("yyyy-MM-dd");

    QSqlQuery query;
    QString insert = QString("insert into sign (name, gender, phone, birthday, logdate) "
                             " values ('%1', '%2', '%3', '%4', '%5');"
                             ).arg(name).arg(gender).arg(phone).arg(birthday).arg(date);
    query.exec(insert);
    qDebug() << insert;
    qDebug() << query.lastError().text();
    database.commit();

    model->select();
    ui->tableView->reset();
}
