#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QCoreApplication>
#include <QDesktopServices>
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
    QString dbpath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
    database.setDatabaseName(QString("%1/sign.db").arg(dbpath));
    if (!database.open()) {
        qDebug() << "Can not open sign.db";
        QMessageBox::information(this, "", "无法连接数据库");
    } else {
        QSqlQuery query;
        QString createTableSql = QString("Create table sign ("
                                         " name varchar(32), "
                                         " gender varchar(4), "
                                         " phone varchar(32), "
                                         " birthday datetime, "
                                         " logdate datetime,"
                                         " current datetime"
                                         ");");
        QString indexTableSql = QString("Create table sign_dict ( "
                                        " name varchar(32), "
                                        " gender varchar(32),"
                                        " phone varchar(32),"
                                        " birthday datetime,"
                                        " logdate datetime,"
                                        " current datetime"
                                        " );");
        query.exec(createTableSql);
        query.exec(indexTableSql);
        qDebug() << createTableSql << indexTableSql;
        database.commit();
        setModel();
    }
}

void MainWindow::setModel()
{
    model = new QSqlTableModel(this);
    model->setTable("sign");
    model->setEditStrategy(QSqlTableModel::OnFieldChange);
    model->setSort(0, Qt::AscendingOrder);
    model->setHeaderData(0, Qt::Horizontal, "姓名");
    model->setHeaderData(1, Qt::Horizontal, "性别");
    model->setHeaderData(2, Qt::Horizontal, "手机");
    model->setHeaderData(3, Qt::Horizontal, "生日");
    model->setHeaderData(4, Qt::Horizontal, "日期");
    model->setHeaderData(5, Qt::Horizontal, "记录时间");

    ui->tableView->setModel(model);
    ui->tableView->alternatingRowColors();
    ui->tableView->horizontalHeader()->setStretchLastSection(true);
    model->select();
    ui->tableView->reset();
}

int MainWindow::candidate(QString filter)
{
    QSqlTableModel *candidateModel = new QSqlTableModel(this);
    candidateModel->setTable("sign_dict");
    candidateModel->setHeaderData(0, Qt::Horizontal, "姓名");
    candidateModel->setHeaderData(1, Qt::Horizontal, "性别");
    candidateModel->setHeaderData(2, Qt::Horizontal, "手机");
    candidateModel->setHeaderData(3, Qt::Horizontal, "生日");
    candidateModel->setHeaderData(4, Qt::Horizontal, "日期");
    candidateModel->setHeaderData(5, Qt::Horizontal, "记录时间");
    ui->tableViewChoose->setModel(candidateModel);
    ui->tableViewChoose->alternatingRowColors();
    ui->tableViewChoose->horizontalHeader()->setStretchLastSection(true);
    qDebug() << filter;
    candidateModel->setFilter(filter);
    candidateModel->select();
    int cnt = candidateModel->rowCount();


    qDebug() << "cnt = " << cnt;
    if (cnt <= 0 ) return cnt;
    if (cnt == 1) {
        qDebug() << candidateModel->record(0).value("phone");
        qDebug() << candidateModel->record(0).value("gender");
    }

    if (cnt >1) ui->tableViewChoose->show();

    return cnt;
}

void MainWindow::insertRec(QString tableName)
{
    QString name = ui->lineEditName->text().trimmed();
    QString gender = ui->comboBoxGender->currentText();
    QString phone = ui->lineEditPhone->text().trimmed();
    QString birthday = ui->lineEditBirth->text().trimmed();
    QDate logdate = ui->dateTimeEdit->date();
    QString date = logdate.toString("yyyy-MM-dd");
    QString currentDt = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");

    QSqlQuery query;
    QString insert = QString("insert into %1 (name, gender, phone, birthday, logdate, current) "
                             " values ('%2', '%3', '%4', '%5', '%6', '%7');"
                             ).arg(tableName).arg(name).arg(gender).arg(phone).arg(birthday).arg(date).arg(currentDt);
    query.exec(insert);
    database.commit();

}

void MainWindow::on_pushButtonOK_clicked()
{
    insertRec("sign");
    model->setSort(5,Qt::DescendingOrder);
    model->select();
    QString name = ui->lineEditName->text().trimmed();
    QString phone = ui->lineEditPhone->text().trimmed();

    int cnt = queryCnt(QString("select * from sign_dict where name = '%1' and phone = '%2'").arg(name).arg(phone));
    if (cnt == 0) {
        insertRec("sign_dict");
    }

    ui->tableView->reset();
    ui->lineEditName->clear();
    ui->lineEditPhone->clear();
    ui->lineEditBirth->clear();
    ui->lineEditName->setFocus();
}

int MainWindow::autoFill(QString name, QString phone)
{
    int cnt = 0;

    if (name.isEmpty() && phone.isEmpty()) return 0;

    if (!name.isEmpty()) {
        cnt = candidate(QString("name = '%1'").arg(name));
        if (cnt <= 0) return 0;
    }

    if (!phone.isEmpty()) {
        cnt = candidate(QString("phone = '%1'").arg(phone));
        if (cnt <= 0) return 0;
    }
    return cnt;
}

int MainWindow::queryCnt(QString sql)
{
    QSqlQuery query;
    query.exec(sql);
    int cnt = 0;
    while(query.next())
        cnt += 1;
    qDebug() << sql << cnt;
    return cnt;
}

void MainWindow::on_lineEditName_editingFinished()
{
    qDebug() << "name finish";
    autoFill(ui->lineEditName->text().trimmed(), "");
}

void MainWindow::on_lineEditName_returnPressed()
{
    qDebug() << "name return";
    autoFill(ui->lineEditName->text().trimmed(), "");
}

void MainWindow::on_lineEditPhone_editingFinished()
{
    autoFill("", ui->lineEditPhone->text().trimmed());
}

void MainWindow::on_lineEditPhone_returnPressed()
{
    autoFill("", ui->lineEditPhone->text().trimmed());
}
