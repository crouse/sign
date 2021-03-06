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
    ui->dateEdit->setDate(QDate::currentDate());
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

        QString notesSql = QString("Create table notes (name varchar(32), phone varchar(32), logtime datetime, note text)");
        query.exec(createTableSql);
        query.exec(indexTableSql);
        query.exec(notesSql);
        qDebug() << createTableSql << indexTableSql;
        database.commit();
        setModel("");
        QString logdate = QDateTime::currentDateTime().toString("yyyy-MM-dd");
        QString filter = QString("logdate = '%1'").arg(logdate);
        setQModel(filter);
        setListModel("");
    }
}

void MainWindow::setModel(QString filter)
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

    model->setFilter(filter);
    model->select();
    ui->tableView->reset();
}

void MainWindow::setListModel(QString filter)
{
   listModel = new QSqlTableModel(this);
   listModel->setTable("notes");
   listModel->setEditStrategy(QSqlTableModel::OnFieldChange);
   listModel->setSort(2, Qt::AscendingOrder);
   listModel->setHeaderData(0, Qt::Horizontal, "姓名");
   listModel->setHeaderData(1, Qt::Horizontal, "手机");
   listModel->setHeaderData(2, Qt::Horizontal, "记录时间");
   listModel->setHeaderData(3, Qt::Horizontal, "发言记录");

    ui->tableViewListNotes->setModel(listModel);
    ui->tableViewListNotes->alternatingRowColors();
    ui->tableViewListNotes->horizontalHeader()->setStretchLastSection(true);

    listModel->setFilter(filter);
    listModel->select();
    ui->tableViewListNotes->reset();
}


void MainWindow::setQModel(QString filter)
{
    qmodel = new QSqlTableModel(this);
    qmodel->setTable("sign");
    qmodel->setEditStrategy(QSqlTableModel::OnFieldChange);
    qmodel->setSort(0, Qt::AscendingOrder);
    qmodel->setHeaderData(0, Qt::Horizontal, "姓名");
    qmodel->setHeaderData(1, Qt::Horizontal, "性别");
    qmodel->setHeaderData(2, Qt::Horizontal, "手机");
    qmodel->setHeaderData(3, Qt::Horizontal, "生日");
    qmodel->setHeaderData(4, Qt::Horizontal, "日期");
    qmodel->setHeaderData(5, Qt::Horizontal, "记录时间");

    ui->tableViewQ->setModel(qmodel);
    ui->tableViewQ->alternatingRowColors();
    ui->tableViewQ->horizontalHeader()->setStretchLastSection(true);

    qmodel->setFilter(filter);
    qmodel->select();
    ui->tableViewQ->reset();
}


int MainWindow::candidate(QString filter)
{
    candidateModel = new QSqlTableModel(this);
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
        QString name = candidateModel->record(0).value("name").toString();
        QString phone = candidateModel->record(0).value("phone").toString();
        QString gender = candidateModel->record(0).value("gender").toString();
        QString birthday = candidateModel->record(0).value("birthday").toString();
        if (ui->lineEditName->text().isEmpty()) ui->lineEditName->setText(name);
        if (ui->lineEditPhone->text().isEmpty()) ui->lineEditPhone->setText(phone);
        ui->lineEditBirth->setText(birthday);
        ui->comboBoxGender->setCurrentText(gender);
        qDebug() << name << phone << gender;
        return cnt;
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
    delete model;
    QDate logdate_date = ui->dateTimeEdit->date();
    QString date = logdate_date.toString("yyyy-MM-dd");
    QString filter = QString("logdate = '%1'").arg(date);
    setModel(filter);
    if (!ui->tableViewChoose->isHidden()) ui->tableViewChoose->hide();
    QString name = ui->lineEditName->text().trimmed();
    QString phone = ui->lineEditPhone->text().trimmed();
    QString logdate = ui->dateTimeEdit->date().toString("yyyy-MM-dd");
    if (name.isEmpty()) return;

    int stat = testIfAddedToday(name, phone, logdate);
    if (stat) {
        QMessageBox msgBox;
        msgBox.setText(QString("刚才已经录入啦，嘿嘿： %1 %2").arg(name, phone));
        msgBox.exec();
        return;
    }

    insertRec("sign");
    model->setSort(5,Qt::DescendingOrder);
    model->select();

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

    if (!name.isEmpty() and !phone.isEmpty()) {
        cnt = candidate(QString("name = '%1' and phone = '%2'").arg(name).arg(phone));
        if (cnt <= 0) return cnt;
    }

    if (!phone.isEmpty()) {
        cnt = candidate(QString("phone = '%1'").arg(phone));
        return cnt;
    }

    if (!name.isEmpty()) {
        cnt = candidate(QString("name = '%1'").arg(name));
        return cnt;
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
    autoFill(ui->lineEditName->text().trimmed(), ui->lineEditPhone->text().trimmed());
}

void MainWindow::on_lineEditName_returnPressed()
{
    autoFill(ui->lineEditName->text().trimmed(), ui->lineEditPhone->text().trimmed());
}

void MainWindow::on_lineEditPhone_editingFinished()
{
    autoFill(ui->lineEditName->text().trimmed(), ui->lineEditPhone->text().trimmed());
}

void MainWindow::on_lineEditPhone_returnPressed()
{
    autoFill(ui->lineEditName->text().trimmed(), ui->lineEditPhone->text().trimmed());
}

void MainWindow::on_tableViewChoose_doubleClicked(const QModelIndex &index)
{
    QSqlRecord record = candidateModel->record(index.row());
    QString name = record.value(0).toString();
    QString gender = record.value(1).toString();
    QString phone = record.value(2).toString();
    QString birthday = record.value(3).toString();
    QString currentDt = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");

    ui->lineEditPhone->setText(phone);
    ui->lineEditBirth->setText(birthday);
    ui->comboBoxGender->setCurrentText(gender);
    ui->dateTimeEdit->setDateTime(QDateTime::currentDateTime());

    ui->tableViewChoose->hide();
    ui->dateTimeEdit->setFocus();
    delete candidateModel;
}

void MainWindow::on_actionUpdate_triggered()
{
    QSqlQuery query;
    query.exec("delete from sign_dict");
    query.exec("insert into sign_dict (name, gender, phone, birthday, logdate, current) select name, gender, phone, birthday, logdate, current from sign group by name, phone");
    database.commit();
}

int MainWindow::testIfAddedToday(QString name, QString phone, QString logdate)
{
    int cnt = 0;
    QSqlQuery query;
    query.exec(QString("select * from sign where name = '%1' and phone = '%2' and logdate = '%3'").arg(name).arg(phone).arg(logdate));

    while(query.next()) {
        cnt++;
    }

    if (cnt) {
        return true;
    }

    return false;
}

void MainWindow::on_actionQuery_triggered()
{
    delete model;

    QDate logdate = ui->dateTimeEdit->date();
    QString date = logdate.toString("yyyy-MM-dd");
    QString filter = QString("logdate = '%1'").arg(date);

    setModel(filter);
    clearEdits();
}

void MainWindow::on_actionQueryPeople_triggered()
{
    delete model;
    QString filter = QString("name = '%1'").arg(ui->lineEditName->text().trimmed());
    setModel(filter);
    clearEdits();
}

void MainWindow::on_actionQueryPhone_triggered()
{
    delete model;
    QString filter = QString("phone = '%1'").arg(ui->lineEditPhone->text().trimmed());
    setModel(filter);
    clearEdits();
}

void MainWindow::clearEdits()
{
    ui->lineEditName->clear();
    ui->lineEditBirth->clear();
    ui->lineEditPhone->clear();
    ui->lineEditName->setFocus();
}

void MainWindow::on_actionClearEdits_triggered()
{
   clearEdits();
}

void MainWindow::on_actionQueryBirthday_triggered()
{
    delete model;
    QString filter = QString("birthday like '\%%1\%'").arg(ui->lineEditBirth->text().trimmed());
    setModel(filter);
    clearEdits();
}

void MainWindow::on_actionQueryAll_triggered()
{
    delete model;
    QString filter = QString("");
    setModel(filter);
    clearEdits();
}

void MainWindow::on_tabWidget_tabBarClicked(int index)
{
    qDebug() << "clicked" << index;
    if (index == 0) ui->mainToolBar->show();
    if (index == 2 || index == 1) {
        ui->mainToolBar->hide();
    }
}

void MainWindow::on_dateEdit_editingFinished()
{
    QString qdate = ui->dateEdit->date().toString("yyyy-MM-dd");
    QString filter = QString("logdate = '%1'").arg(qdate);
    delete qmodel;
    setQModel(filter);
}

void MainWindow::on_pushButtonQquery_clicked()
{
    QString qdate = ui->dateEdit->date().toString("yyyy-MM-dd");
    QString filter = QString("logdate = '%1'").arg(qdate);
    delete qmodel;
    setQModel(filter);
}

void MainWindow::on_tableViewQ_doubleClicked(const QModelIndex &index)
{
    ui->plainTextEdit->setFocus();
    qDebug() << "double clicked" << index.row();
    int rowNum = index.row();
    //QRecord record = qmodel->record(rowNum).value("name").toString();
    QSqlRecord record = qmodel->record(rowNum);
    QString name = record.value("name").toString();
    QString phone = record.value("phone").toString();
    qDebug() << name << phone;
    ui->lineEditQname->setText(name);
    ui->lineEditQPhone->setText(phone);
    ui->dateTimeEditQ->setDateTime(QDateTime::currentDateTime());
}

void MainWindow::on_tableViewQ_clicked(const QModelIndex &index)
{
    qDebug() << "single clicked" << index;
}

void MainWindow::on_pushButtonQSave_clicked()
{
    QString name = ui->lineEditQname->text();
    QString phone = ui->lineEditQPhone->text();
    QString text = ui->plainTextEdit->toPlainText();
    ui->dateTimeEditQ->setDateTime(QDateTime::currentDateTime());
    QString logtime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    QString logdate = ui->dateTimeEditQ->dateTime().toString("yyyy-MM-dd");

    qDebug() << "on_pushButtonQSave_clicked" << name << phone << text << logtime;
    QString sql = QString("insert into notes (name, phone, logtime, note) values ('%1', '%2', '%3', '%4')").arg(name).arg(phone).arg(logtime).arg(text);
    QSqlQuery query;
    query.exec(sql);
    database.commit();
    QString filter = QString("logtime > '%1'").arg(logdate);
    delete listModel;
    setListModel(filter);

    ui->plainTextEdit->clear();
}
