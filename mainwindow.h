#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSql>
#include <QSqlTableModel>
#include <QSqlDatabase>
#include <QSqlTableModel>
#include <QMessageBox>
#include <QTableView>
#include <QFileDialog>
#include <QFile>
#include <QDate>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    QSqlDatabase database;
    QSqlTableModel *model;
    void setModel();
    void insertRec(QString tableName);
    int candidate(QString filter);
    void setDatabase();
    int autoFill(QString name, QString phone);

private slots:
    void on_pushButtonOK_clicked();

    void on_lineEditName_editingFinished();

    void on_lineEditPhone_editingFinished();

    void on_lineEditName_returnPressed();

    void on_lineEditPhone_returnPressed();

private:
    Ui::MainWindow *ui;
    int candidate_cnt;
};

#endif // MAINWINDOW_H
