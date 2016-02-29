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
    QSqlTableModel *candidateModel;
    void setModel(QString filter);
    void insertRec(QString tableName);
    int candidate(QString filter);
    void setDatabase();
    int autoFill(QString name, QString phone);
    int queryCnt(QString sql);
    int testIfAddedToday(QString name, QString phone, QString logdate);
    void clearEdits();

private slots:
    void on_pushButtonOK_clicked();

    void on_lineEditName_editingFinished();

    void on_lineEditPhone_editingFinished();

    void on_lineEditName_returnPressed();

    void on_lineEditPhone_returnPressed();

    void on_tableViewChoose_doubleClicked(const QModelIndex &index);

    void on_actionUpdate_triggered();

    void on_actionQuery_triggered();

    void on_actionQueryPeople_triggered();

    void on_actionQueryPhone_triggered();

    void on_actionClearEdits_triggered();

    void on_actionQueryBirthday_triggered();

    void on_actionQueryAll_triggered();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
