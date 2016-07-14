#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <math.h>       /* floor */
#include <iostream>
#include <stdio.h>
#include <QtCore>
#include <QtWidgets>
#include <QElapsedTimer>

using namespace std;

//inicializacija spremenljivk

#define MAX_COL 325 //9*9*4 = 324
#define MAX_ROW 730 //9*9*9 = 729

#define SQ_OFFSET 0 // vsak kvadratek more imet stevilko (81 pravil)
#define RW_OFFSET 81 // vsaka vrstica more imet vse razlicne stevilke (81 pravil)
#define CL_OFFSET 162 // vsak stolpec more imet vse razlicne stevilke (81 pravil)
#define BX_OFFSET 243 // vsaka skatla more imet vse razlicne stevilke (81 pravil)

// sudoku_array [9][9];
int sudoku_array[9][9] = {
  { 9, 5, 0, 0, 2, 0, 0, 0, 0 },
  { 0, 7, 8, 5, 0, 0, 0, 0, 0 },
  { 0, 6, 0, 4, 0, 0, 0, 0, 0 },
  { 0, 9, 0, 7, 3, 0, 0, 5, 0 },
  { 8, 0, 0, 0, 0, 0, 0, 0, 2 },
  { 0, 1, 0, 0, 8, 5, 0, 6, 0 },
  { 0, 0, 0, 0, 0, 6, 0, 8, 0 },
  { 0, 0, 0, 0, 0, 8, 7, 9, 0 },
  { 0, 0, 0, 0, 7, 0, 0, 3, 1 }
};

//definiran node
struct Node {
    struct Node * Header; //kam pase
    //in sosedje
    struct Node * Left;
    struct Node * Right;
    struct Node * Up;
    struct Node * Down;

    //za lazjo debuganje - pregled
    char Name;
    int  Number;
};

int nCol;
int nRow;
struct Node Matrix[MAX_COL][MAX_ROW];
struct Node Root;
struct Node *RootNode = &Root;
struct Node *RowHeader[MAX_ROW];
int Data[MAX_COL][MAX_ROW];
int Result[MAX_ROW];
int nResult = 0;
int finish;
int added=0;
int Sudoku[9][9] = {};

// vracanje pravega indexa
int retNb(int N) { return N/81; }
int retRw(int N) { return (N/9)%9; }
int retCl(int N) { return N%9; }
int retBx(int N) { return ((retRw(N)/3)*3) + (retCl(N)/3); }
int retSq(int N) { return retRw(N)*9 + retCl(N); }
int retRn(int N) { return retNb(N)*9 + retRw(N); }
int retCn(int N) { return retNb(N)*9 + retCl(N); }
int retBn(int N) { return retNb(N)*9 + retBx(N); }
int getIn(int Nb, int Rw, int Cl) { return Nb*81 + Rw*9 + Cl; }

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    //ui->setupUi(this);
    //ustvarim Widget
    centralWidget = new QWidget(this);
    this->setCentralWidget( centralWidget );
    centralWidget->setWindowTitle("DLX");
    centralWidget->setMaximumSize(1000, 1000);
    centralWidget->setMinimumSize(10, 10);
    centralWidget->resize(400,400);
    mainLayout = new QGridLayout( centralWidget );
    mainLayout->setSpacing(0);
    for(int mr = 0; mr < 3; mr++)//ustvarim 3 vrstice
    {
        for(int mc = 0; mc < 3; mc++)//ustvarim 3 stolpce
        {
            //veliki robovi
            QFrame * widget = new QFrame;
            widget->setFrameStyle(QFrame::Plain);
            widget->setFrameShape(QFrame::Box);

            //znotraj velikih robov se en grid
            QGridLayout * gridLayout = new QGridLayout(widget);
            gridLayout->setSpacing(0);
            gridLayout->setMargin(0);

            for(int r = 0; r < 3; r++)//3 vrstice
            {
                for(int c = 0; c < 3; c++)//3 stolpci
                {
                    //ustvarim skatlo s textom
                    QTextEdit * tile = new QTextEdit("");
                    tile->setFrameStyle(QFrame::Plain);
                    tile->setFrameShape(QFrame::Box);
                    tile->setAlignment(Qt::AlignCenter);
                    tile->setTabChangesFocus(true);
                    tile->setStyleSheet("font: 24pt ;");

                    gridLayout->addWidget(tile, r, c, 1, 1, Qt::AlignCenter);
                }
            }
           mainLayout->addWidget(widget, mr, mc, 1, 1, Qt::AlignCenter);
        }
    }
    //ustvarim menu z eventom
    QMenuBar* menuBar = new QMenuBar();
    QMenu *fileMenu = new QMenu("Algoritem");
    menuBar->addMenu(fileMenu);
    QAction *helpAction = fileMenu->addAction("Find solution");
    connect(helpAction, SIGNAL(triggered()), this, SLOT(algoritem()));
    centralWidget->layout()->setMenuBar(menuBar);
    QMenu *fileMenu1 = new QMenu("Input");
    menuBar->addMenu(fileMenu1);
    QAction *Action = fileMenu1->addAction("Reset");
    connect(Action, SIGNAL(triggered()), this, SLOT(reset()));
    QAction *Action2 = fileMenu1->addAction("Qassim Hamza");
    connect(Action2, SIGNAL(triggered()), this, SLOT(hamza()));
}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::GetNumbers(){
    QList<QTextEdit*> textEdits = centralWidget->findChildren<QTextEdit *>();
    for(int i = 0; i < textEdits.count(); i++) {
        int y = (int(floor(i / 3)) % 3) + (3 * floor(i / 27));
        int x = (i % 3) + (3 * floor(i / 9)) - (9 * floor(i / 27));
        QTextEdit * tile = textEdits.at(i);
        QString s =tile->toPlainText();
        if(s==""){
            s="0";
        }
        sudoku_array[y][x] = s.toInt();
    }
    print_sudoku_array();
}

void MainWindow::put_results(){
    QList<QTextEdit*> textEdits = centralWidget->findChildren<QTextEdit *>();
    for(int i = 0; i < textEdits.count(); i++) {
        int y = (int(floor(i / 3)) % 3) + (3 * floor(i / 27));
        int x = (i % 3) + (3 * floor(i / 9)) - (9 * floor(i / 27));
        QTextEdit * tile = textEdits.at(i);
        QString s = QString::number(Sudoku[y][x]+1);
        tile->setText(s);
    }
}
void MainWindow::print_sudoku_array(){
    for (int i = 0; i < 9; ++i)
       {
           for (int j = 0; j < 9; ++j)
           {
               std::cout << sudoku_array[i][j] << ' ';
           }
           std::cout << std::endl;
       }
}

void MainWindow::BuildData(){
    int a,b,c;
    int index;
    //naredim tipicni exact cover

    //omejitve 9*9*4 in 9*9*9
    nCol = 324;
    nRow = 729 + 1; //header je se zraven kot vrstica
    for(a=0;a<9;a++)
    {
        for(b=0;b<9;b++)
        {
            for(c=0;c<9;c++)
            {
                index = getIn(c, a, b); //pridobim index
                //postavim 1ko na ustrezno mesto za pravilo
                Data[SQ_OFFSET + retSq(index)][index] = 1; //pravilo 1 - celica
                Data[RW_OFFSET + retRn(index)][index] = 1; //pravilo 2 - vrstica
                Data[CL_OFFSET + retCn(index)][index] = 1; //pravilo 3 - stolpec
                Data[BX_OFFSET + retBn(index)][index] = 1; //pravilo 4 - skatla
            }
        }
    }
    for(a=0;a<nCol;a++)//naredim izjemo za na konec - za nadaljno pravilno premikanje da povezem z zacetkom
    {
        Data[a][nRow-1] = 2;
    }
    /*cout<<"IZPIS DATA:"<<endl;
    for(int i=0;i<324;i++){
        for(int j=0;j<nRow;j++){
            cout<<Data[i][j]<<" ";
        }
        cout<<endl;
    }*/
    CreateMatrix(); //ustvarim linked-listed matriko

    //dodam se matriki informacije o vrsti in stevilki pravila
    for(a=0;a<RW_OFFSET;a++){
        Matrix[a][nRow-1].Name = 'S';
        Matrix[a][nRow-1].Number = a;
    }
    for(a=RW_OFFSET;a<CL_OFFSET;a++){
        Matrix[a][nRow-1].Name = 'R';
        Matrix[a][nRow-1].Number = a-RW_OFFSET;
    }
    for(a=CL_OFFSET;a<BX_OFFSET;a++){
        Matrix[a][nRow-1].Name = 'C';
        Matrix[a][nRow-1].Number = a-CL_OFFSET;
    }
    for(a=BX_OFFSET;a<nCol;a++){
        Matrix[a][nRow-1].Name = 'B';
        Matrix[a][nRow-1].Number = a-BX_OFFSET;
    }
}

// delanje linked-list-a matrike; pomozne funkcije
//premaknem v smeri do roba
int dataLeft(int i){
    if(i-1<0){
        return nCol-1;
    }
    else{
        return i-1;
    }
}
int dataRight(int i){
    return (i+1) % nCol;
}
int dataUp(int i){
    if(i-1<0){
        return nRow-1;
    }
    else{
        return i-1;
    }
}
int dataDown(int i){
    return (i+1) % nRow;
}

void MainWindow::CreateMatrix() {
    int i,j;
    //naredim linked-list matriko - povezem vozlisca itd..
    for(int a=0;a<nCol;a++) {//grem cez vse elemente v exact coverju
        for(int b=0;b<nRow;b++) {
            if(Data[a][b]!=0) { //kjer ni nic, je node
                //povezem z sosednjimi
                // Left pointer
                i = a;
                j = b;
                do {
                    i = dataLeft(i);
                } while (Data[i][j]==0);
                Matrix[a][b].Left = &Matrix[i][j];
                // Right pointer
                i = a;
                j = b;
                do {
                    i = dataRight(i);
                } while (Data[i][j]==0);
                Matrix[a][b].Right = &Matrix[i][j];
                // Up pointer
                i = a; j = b;
                do {
                    j = dataUp(j);
                } while (Data[i][j]==0);
                Matrix[a][b].Up = &Matrix[i][j];
                // Down pointer
                i = a;
                j = b;
                do {
                    j = dataDown(j);
                } while (Data[i][j]==0);
                Matrix[a][b].Down = &Matrix[i][j];
                // Header pointer
                Matrix[a][b].Header = &Matrix[a][nRow-1];
                Matrix[a][b].Number = b;
                //Row Header
                RowHeader[b] = &Matrix[a][b];
            }
        }
    }
    for(int a=0;a<nCol;a++) {//vsem stolpcem dam se ime in stevilko
        Matrix[a][nRow-1].Name = 'C';
        Matrix[a][nRow-1].Number = a;
    }
    //vstavimo se root (ekstra header) vhod na celotno strukturo ter povezemo z vsem
    Root.Name = 'R';
    Root.Left = &Matrix[nCol-1][nRow-1];
    Root.Right = &Matrix[0][nRow-1];
    Matrix[nCol-1][nRow-1].Right = &Root;
    Matrix[0][nRow-1].Left = &Root;
}

void Cover(struct Node *ColNode) {
    struct Node *RowNode, *RightNode;
    ColNode->Right->Left = ColNode->Left;
    ColNode->Left->Right = ColNode->Right;
    for(RowNode = ColNode->Down; RowNode!=ColNode; RowNode = RowNode->Down) {
        for(RightNode = RowNode->Right; RightNode!=RowNode; RightNode = RightNode->Right) {
            RightNode->Up->Down = RightNode->Down;
            RightNode->Down->Up = RightNode->Up;
        }
    }
}

void UnCover(struct Node *ColNode) {
    struct Node *RowNode, *LeftNode;
    for(RowNode = ColNode->Up; RowNode!=ColNode; RowNode = RowNode->Up) {
        for(LeftNode = RowNode->Left; LeftNode!=RowNode; LeftNode = LeftNode->Left) {
            LeftNode->Up->Down = LeftNode;
            LeftNode->Down->Up = LeftNode;
        }
    }
    ColNode->Right->Left = ColNode;
    ColNode->Left->Right = ColNode;
}

void SolutionRow(struct Node *RowNode) {
    Cover(RowNode->Header);
    struct Node *RightNode;
    for(RightNode = RowNode->Right; RightNode!=RowNode; RightNode = RightNode->Right)
    {
        Cover(RightNode->Header);
    }
}

void AddNumber(int N, int R, int C) {
    SolutionRow(RowHeader[getIn(N, R, C)]);
    added++;//zabelezim kok sem jih dodal
    Result[nResult++] = getIn(N, R, C); //dodam ze kot resitev te obstojece
}
void MainWindow::LoadPuzzle() {
    int a,b;
    int temp;
    for(a=0;a<9;a++) {
        for(b=0;b<9;b++) {
            temp=sudoku_array[a][b]; //dobim stevilko
            if(temp>=1&&temp<=9){//vse razn 0
                AddNumber(temp-1, a, b);
            }
        }
    }
}

void PrintSolution(void) {
    int a,b,c;

    for(a=0;a<9;a++)//inicializacija
    {
        for(b=0;b<9;b++)
        {
            Sudoku[a][b] = -1;
        }
    }
    //cout<<"stevilo resenih:"<<nResult<<endl;
    for(a=0;a<nResult;a++) {
        Sudoku[retRw(Result[a])][retCl(Result[a])] = retNb(Result[a]);//na pravo pozicijo damo pravo vrednost
    }
    for(a=0;a<9;a++) {
        for(b=0;b<9;b++) {
            if(a>0&&a%3==0&&b==0)//horizontal lines
            {
                 for(c=0;c<9;c++)
                 {
                     cout<<"--";
                 }
                 cout<<"\n";
            }
            if(Sudoku[a][b]>=0){
                cout<<Sudoku[a][b]+1;
                if(b%3==2){
                    cout<<"|";
                }
                else{
                    cout<<" ";
                }
            }
            else cout<<(". ");//ce ni resitve naslo za tist prostor
        }
        cout<<("\n");
    }
}
struct Node *ChooseColumn(void) {
    return RootNode->Right;
}

void MainWindow::Search(int k) {
    //pogoji kdaj sem nasel, ali sem vseh 81 polj nasel ali pa je ostal en node
    if((RootNode->Left == RootNode && RootNode->Right==RootNode) || k == (81-added)){
        cout<<("Resitev");
        PrintSolution();
        finish = 1;
        return;
    }
    struct Node *Column = ChooseColumn(); //zberem stolpec - kar desni
    Cover(Column); //ga coveram

    struct Node *RowNode;
    struct Node *RightNode;
    //rekurzijo delam tako dolgo:
    for(RowNode = Column->Down; RowNode!=Column && !finish; RowNode = RowNode->Down) {
        Result[nResult++] = RowNode->Number;//vstavim stevilko notri
        //koveram vse potrebne
        for(RightNode = RowNode->Right; RightNode!=RowNode; RightNode = RightNode->Right) {
            Cover(RightNode->Header);
        }
        Search(k+1);
        //ta node ni delal, po rekurziji in ga odstranim, prav tako vse povrnem v prejsne stanje - uncoveram
        for(RightNode = RowNode->Right; RightNode!=RowNode; RightNode = RightNode->Right) {
            UnCover(RightNode->Header);
        }
        Result[--nResult] = 0;//odstranim stevilko
    }
    //nazaj uncoveram stolpec
    UnCover(Column);
}
void MainWindow::algoritem()
{
    GetNumbers();
    finish = 0;
    added=0;
    BuildData();
    LoadPuzzle();
    cout<<endl;
    QTime time;
    time.start();
    Search(0);
    cout<<"CAS: "<<time.elapsed()<<"ms"<<endl;
    put_results();
}

void MainWindow::reset(){
    QList<QTextEdit*> textEdits = centralWidget->findChildren<QTextEdit *>();
    for(int i = 0; i < textEdits.count(); i++) {
        QTextEdit * tile = textEdits.at(i);
        QString s ="";
        tile->setText(s);
    }
}

void MainWindow::hamza(){
    int hamza[9][9] = {
      { 0, 0, 0, 7, 0, 0, 8, 0, 0 },
      { 0, 0, 0, 0, 4, 0, 0, 3, 0 },
      { 0, 0, 0, 0, 0, 9, 0, 0, 1 },
      { 6, 0, 0, 5, 0, 0, 0, 0, 0 },
      { 0, 1, 0, 0, 3, 0, 0, 4, 0 },
      { 0, 0, 5, 0, 0, 1, 0, 0, 7 },
      { 5, 0, 0, 2, 0, 0, 6, 0, 0 },
      { 0, 3, 0, 0, 8, 0, 0, 9, 0 },
      { 0, 0, 7, 0, 0, 0, 0, 0, 2 }
    };
    QList<QTextEdit*> textEdits = centralWidget->findChildren<QTextEdit *>();
    for(int i = 0; i < textEdits.count(); i++) {
        int y = (int(floor(i / 3)) % 3) + (3 * floor(i / 27));
        int x = (i % 3) + (3 * floor(i / 9)) - (9 * floor(i / 27));
        QTextEdit * tile = textEdits.at(i);
        QString s = QString::number(hamza[y][x]);
        tile->setText(s);
    }
}
