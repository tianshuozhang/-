#include "formmenu.h"
#include "ui_formmenu.h"
#include <qdir.h>
#include <stdio.h>
#include <QDebug>
#include <QMessageBox>
#include <QTime>
#include <thread>
#define TM_FMT "%Y-%m-%d %H:%M:%S"
#define NETLINK_TEST 29
#define MAX_PAYLOAD 1024  /* maximum payload size*/
#include <sqlite3.h>
#include <iostream>
#include "main.cpp"
FormMenu::FormMenu(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormMenu)
{
    ui->setupUi(this);

}

FormMenu::~FormMenu()
{
    delete m;
    m = NULL;
    delete formChildAddRule;
    formChildAddRule = NULL;
    delete ui;
    m_myTimer->stop();
    thread1.detach();
}

int max(int a, int b)
{
    return a > b ? a : b;
}

void FormMenu::Init()
{
    char temp[30];
    strcpy(temp,User.toStdString().data());


    const char *plus="%%";
    sql=sqlite3_mprintf("SELECT * FROM %s WHERE USERNAME LIKE '%s%s%s' AND LOGTIME LIKE '%s%s%s' AND FILEPATH LIKE '%s%s%s' AND COMMANDNAME LIKE '%s%s%s' AND RESULT LIKE '%s%s%s';",
        User.toUtf8().data(),plus,"",plus,plus,"",plus,plus,"",plus,plus,"",plus,plus,"",plus);

    char Username[100], commandname[100],  logtime[100],file_path[100],result[10];


    formChildAddRule = NULL;
    formChildAddRule = new FormAddRule(NULL);

    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableWidget->setSortingEnabled(true);
    ui->tableWidget->sortByColumn(0, Qt::AscendingOrder);
    ui->tableWidget->horizontalHeader()->setStretchLastSection(true);
    ui->tableWidget->verticalHeader()->setDefaultSectionSize(36);
    ui->tableWidget->setIconSize(QSize(32, 32));
    QHeaderView* headerView = ui->tableWidget->verticalHeader();
    headerView->setHidden(true);

    //update();

    connect(this, SIGNAL(sendAddButtonPressed(QMap<int,task>*)),
            formChildAddRule, SLOT(RecvAddButtonPressed(QMap<int,task>*)));
    connect(formChildAddRule, SIGNAL(sendSearch(task)),
            this, SLOT(RecvSearch(task)));
    connect(m_myTimer, SIGNAL(timeout()), this, SLOT(TimerResponse()));

    m_myTimer->start(3000);
    thread1=std::thread(&solve);




}

void FormMenu::update()
{


       /* int nRow = ui->tableWidget->rowCount();


        QString wStr;

        wStr = "";

        //qDebug() << ID;

        ui->tableWidget->insertRow(nRow);
        wStr += t.Username + " ";
        QTableWidgetItem *itemUserame = new QTableWidgetItem(t.Username);
        ui->tableWidget->setItem(nRow, 0, itemUserame);


        wStr += QString(t.uid) + " ";
        QTableWidgetItem *itemUid = new QTableWidgetItem(QString(t.uid));
        ui->tableWidget->setItem(nRow, 0, itemUid);


        wStr += t.Commondname + " ";
        QTableWidgetItem *itemCommandname = new QTableWidgetItem(t.Commondname);
        ui->tableWidget->setItem(nRow, 0, itemCommandname);


        wStr += QString(t.pid) + " ";
        QTableWidgetItem *itemPid= new QTableWidgetItem(QString(t.pid));
        ui->tableWidget->setItem(nRow, 0, itemPid);



        wStr += t.LogTime + " ";
        QTableWidgetItem *itemLogtime = new QTableWidgetItem(t.LogTime);
        ui->tableWidget->setItem(nRow, 0, itemLogtime);



        wStr += t.File_path + " ";
        QTableWidgetItem *itemFilepath = new QTableWidgetItem(t.File_path);
        ui->tableWidget->setItem(nRow, 0, itemFilepath);



        wStr += t.Result + "\n";
        QTableWidgetItem *itemResult = new QTableWidgetItem(t.Result);
        ui->tableWidget->setItem(nRow, 0, itemResult);
        */




}

void FormMenu::RecvSignInButtonPressed(QString strUser)
{
    User = strUser;
    Init();
}

void FormMenu::TimerResponse()
{
    show_all_log();
}

void FormMenu::RecvSearch(task t)
{
    // 将 t.Username 赋值给 Username
    QString username = t.Username;
    char* Username = new char[username.length() + 1];
    strcpy(Username, username.toUtf8().data());
    if (strcmp(Username, "All") == 0) {
        Username[0] = '\0';  // 置为空字符串
    }
    // 使用 username_ptr 进行操作，操作结束后需要释放内存


    // 将 t.LogTime 赋值给 Logtime
    QString logtime = t.LogTime;
    char* Logtime = new char[logtime.length() + 1];
    strcpy(Logtime, logtime.toUtf8().data());
    if (strcmp(Logtime, "All") == 0) {
        Logtime[0] = '\0';  // 将 logtime_ptr 置为空字符串
    }


    // 将 t.Commondname 赋值给 Commandname
    QString commandname = t.Commondname;
    char* Commandname = new char[commandname.length() + 1];
    strcpy(Commandname, commandname.toUtf8().data());
    if (strcmp(Commandname, "All") == 0) {
        Commandname[0] = '\0';  // 将 commandname_ptr 置为空字符串
    }
    // 使用 commandname_ptr 进行操作，操作结束后需要释放内存


    // 将 t.Result 赋值给 Result
    QString result = t.Result;
    char* Result = new char[result.length() + 1];
    strcpy(Result, result.toUtf8().data());
    if (strcmp(Result, "All") == 0) {
        Result[0] = '\0';  // 将 result_ptr 置为空字符串
    }
    // 使用 result_ptr 进行操作，操作结束后需要释放内存

    QString filepath = t.File_path;
    char* File_path = new char[filepath.length() + 1];
    strcpy(File_path, filepath.toUtf8().data());
    std::cout<<File_path<<std::endl;
    if(strcmp(File_path,"All")==0) File_path[0]='\0';

    char *plus="%%";
    printf("%s,%s,%s,%s,%s\n",Username,Logtime,File_path,Commandname,Result);
    sql=sqlite3_mprintf("SELECT * FROM %s WHERE USERNAME LIKE '%s%s%s' AND LOGTIME LIKE '%s%s%s' AND FILEPATH LIKE '%s%s%s' AND COMMANDNAME LIKE '%s%s%s' AND RESULT LIKE '%s%s%s';",
    User.toUtf8().data(),plus,Username,plus,plus,Logtime,plus,plus,File_path,plus,plus,Commandname,plus,plus,Result,plus);
    std::cout<<File_path<<std::endl;

    printf("%s",sql);
    delete[] Username;
    delete[] File_path;
    delete[] Logtime;
    delete[] Result;
    delete[] Commandname;
    formChildAddRule->close();
}

void FormMenu::on_pushButtonAdd_clicked()
{
    emit sendAddButtonPressed(m);

    formChildAddRule->show();
    if(formChildAddRule->isMinimized())
    {
        formChildAddRule->showNormal();
    }
    formChildAddRule->raise();
}



void FormMenu::on_pushButtonDelete_clicked()
{

    QTableWidgetItem *curItem = ui->tableWidget->currentItem();

    if(curItem != NULL)
    {
        if(curItem->isSelected())
        {

            QTableWidgetItem* item = ui->tableWidget->item(curItem->row(), 3);
            if (item != nullptr) {
                // 如果行和列都有效，并且 QTableWidgetItem 对象不为 nullptr，则获取单元格的文本内容
                int pid_ = item->text().toInt();
                delete_log(User.toUtf8().data(),pid_);
            }
            int id = ui->tableWidget->item(curItem->row(), 0)->text().toInt();
            ui->tableWidget->removeRow(curItem->row());


        }
    }



}

void FormMenu::show_all_log()
{

    char*zErrMsg;
    int found = 0;
    int rc;
    update();
    mu.lock();
    ui->tableWidget->setSortingEnabled(false);
    while(ui->tableWidget->rowCount() > 0)
       {
           int nRow = ui->tableWidget->rowCount();
           ui->tableWidget->removeRow(nRow - 1);
       }
     sqlite3_open("database.db", &db);
    rc=sqlite3_exec(db,sql,back,this,&zErrMsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n",  sqlite3_errmsg(db));
    }
    else
    {
        fprintf(stdout, "Records searched successfully\n");
        printf("Found %d rows.\n", found);
    }
    sqlite3_close(db);
    ui->tableWidget->setSortingEnabled(true);
    ui->tableWidget->sortByColumn(0, Qt::AscendingOrder);
    mu.unlock();
}
void copy(char **a,FormMenu *b){

            int nRow = b->ui->tableWidget->rowCount();

             QString wStr;

             wStr = "";
             //qDebug() << ID;

             b->ui->tableWidget->insertRow(nRow);
             wStr += QString::fromUtf8(a[3]); + " ";
             QTableWidgetItem *itemUserame = new QTableWidgetItem(QString::fromUtf8(a[3]));
             b->ui->tableWidget->setItem(nRow, 0, itemUserame);
             wStr += QString::fromUtf8(a[2]); + " ";
             QTableWidgetItem *itemUid = new QTableWidgetItem(QString::fromUtf8(a[2]));
             b->ui->tableWidget->setItem(nRow, 1, itemUid);
             wStr += QString::fromUtf8(a[5]); + " ";
             QTableWidgetItem *itemCommandname = new QTableWidgetItem(QString::fromUtf8(a[5]));
             b->ui->tableWidget->setItem(nRow, 2, itemCommandname);
             wStr += QString::fromUtf8(a[0]); + " ";
             QTableWidgetItem *itemPid = new QTableWidgetItem(QString::fromUtf8(a[0]));
             b->ui->tableWidget->setItem(nRow, 3, itemPid);
             wStr += QString::fromUtf8(a[4]); + " ";
             QTableWidgetItem *itemFilepath = new QTableWidgetItem(QString::fromUtf8(a[4]));
             b->ui->tableWidget->setItem(nRow, 4, itemFilepath);
             wStr += QString::fromUtf8(a[1]); + " ";
             QTableWidgetItem *itemLogtime = new QTableWidgetItem(QString::fromUtf8(a[1]));
             b->ui->tableWidget->setItem(nRow, 5, itemLogtime);
             wStr += QString::fromUtf8(a[3]); + " ";
             QTableWidgetItem *itemResult = new QTableWidgetItem(QString::fromUtf8(a[6]));
             b->ui->tableWidget->setItem(nRow, 6, itemResult);
}
static int back(void *data, int argc, char **argv, char **azColName) {
   int i;
    FormMenu *w=static_cast<FormMenu*>(data);
   for(i = 0; i<argc; i++) {
       printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
   }
   printf("\n");
   copy(argv,w);
   return 0;

}
