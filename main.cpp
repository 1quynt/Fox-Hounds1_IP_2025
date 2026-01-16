#include <graphics.h>
#include <iostream>
#include <stdio.h>
#include <math.h>
#include <time.h>

// CONSTANTE
#define SCREEN_W 800
#define SCREEN_H 600
#define CELL 60
#define BOARD_X 160 // (800 - 8*60)/2
#define BOARD_Y 80

// Culori
#define C_ALB 15
#define C_GRI 8
#define C_ROSU 4
#define C_ALBASTRU 1
#define C_GALBEN 14
#define C_PORTOCALIU 12
#define C_NEGRU 0
#define C_HIGHLIGHT 6
#define GREEN 2

// VARIABILE GLOBALE
int board[8][8];
int turn = 1;         // 1=Vulpe, 2=Caini
int stare = 0;        // 0=Meniu, 1=Mod, 2=AlegereParte, 3=Joc, 4=Final 5=Pauza
int selR = -1, selC = -1; // Piesa selectata
int winner = 0;       // Cine a castigat
int isPvP = 0;        // 0=Cu PC, 1=Cu Om
int euSuntVulpe = 1;  // Daca joc cu PC-ul

//JOC

void initJoc() {

    for(int i=0; i<8; i++)
        for(int j=0; j<8; j++)
            board[i][j] = 0;

    board[0][4] = 1;
    board[7][1] = 2;
    board[7][3] = 2;
    board[7][5] = 2;
    board[7][7] = 2;

    turn = 1;
    selR = -1;
    winner = 0;
}

int valid(int r1, int c1, int r2, int c2, int piesa) {

    if(r2 < 0 || r2 > 7 || c2 < 0 || c2 > 7) return 0;
    if(board[r2][c2] != 0) return 0;
    if(abs(r2-r1) != 1 || abs(c2-c1) != 1) return 0;

    if(piesa == 2 && r2 >= r1) return 0; // cainii doar inainte

    return 1;
}

int verificaCastigator() {

    for(int i=0; i<8; i++) {
        if(board[7][i] == 1) return 1;
    }

    // cautam coordonatele vulpii
    int vr = -1, vc = -1;
    for(int i=0; i<8; i++) {
        for(int j=0; j<8; j++) {
            if(board[i][j] == 1) {
                vr = i;
                vc = j;
                break;
            }
        }
    }

    // Pentru orice eventualitate, verificam daca vulpea nu e pe tabla
    if(vr == -1) return 2;

    // verificam scaparile pe diagonala
    int dr[] = {-1, -1, 1, 1};
    int dc[] = {-1, 1, -1, 1};

    int areMiscareValida = 0;

    for(int k=0; k<4; k++) {
        int nR = vr + dr[k]; // rand vecin
        int nC = vc + dc[k]; // coloana vecina

        // verificam daca vecinul e in interiorul tablei
        if(nR >= 0 && nR <= 7 && nC >= 0 && nC <= 7) {

            // verif daca vecinul e liber
            if(board[nR][nC] == 0) {
                areMiscareValida = 1;
                break; // s-a gasit o scapare
            }
        }
    }

    // Daca am verificat toti vecinii si n-am gasit nicio miscare valida:
    if(areMiscareValida == 0) return 2;

    return 0; // Jocul continua
}


void mutarePC(int piesaPC) {
    int startR[100], startC[100];
    int finalR[100], finalC[100];
    int k = 0; // nr mutari gasite
    for(int i = 0; i < 8; i++) {
        for(int j = 0; j < 8; j++) {

            // verificam daca am gasit o piesa care apartine PC-ului
            if(board[i][j] == piesaPC) {

                // Verif STANGA-JOS (i+1, j-1)
                if(valid(i, j, i+1, j-1, piesaPC) == 1) {
                    startR[k] = i; startC[k] = j;
                    finalR[k] = i+1; finalC[k] = j-1;
                    k++;
                }

                // Verif DREAPTA-JOS (i+1, j+1)
                if(valid(i, j, i+1, j+1, piesaPC) == 1) {
                    startR[k] = i; startC[k] = j;
                    finalR[k] = i+1; finalC[k] = j+1;
                    k++;
                }

                // Verif STANGA-SUS (i-1, j-1)
                if(valid(i, j, i-1, j-1, piesaPC) == 1) {
                    startR[k] = i; startC[k] = j;
                    finalR[k] = i-1; finalC[k] = j-1;
                    k++;
                }

                // Verif DREAPTA-SUS (i-1, j+1)
                if(valid(i, j, i-1, j+1, piesaPC) == 1) {
                    startR[k] = i; startC[k] = j;
                    finalR[k] = i-1; finalC[k] = j+1;
                    k++;
                }
            }
        }
    }
    if(k > 0) {
        int ales = rand() % k;

        // Scoatem coordonatele din vectori
        int rVechi = startR[ales];
        int cVechi = startC[ales];
        int rNou = finalR[ales];
        int cNou = finalC[ales];

        // Facem schimbul pe tabla
        board[rNou][cNou] = piesaPC; // Punem piesa in noua pozitie
        board[rVechi][cVechi] = 0;

        if(turn == 1) turn = 2;
        else turn = 1;
    }
}


// DESENARE

void buton(int x, int y, int w, char* text, int mx=-1, int my=-1) {
    int isHover = 0;
    if(mx != -1 && my != -1)
        if(mx >= x && mx <= x+w && my >= y && my <= y+50) isHover = 1;

    int culoareFundal = isHover ? C_GALBEN : C_GRI;

    setfillstyle(SOLID_FILL, culoareFundal);
    bar(x, y, x+w, y+50);

    setcolor(C_PORTOCALIU);
    rectangle(x, y, x+w, y+50);

    setbkcolor(culoareFundal);
    setcolor(isHover ? C_ROSU : C_PORTOCALIU);
    settextstyle(3, 0, 3);
    outtextxy(x + (w - textwidth(text)) / 2,
              y + (50 - textheight(text)) / 2,
              text);
}


void textCentrat(int y, char* txt, int size, int culoareFundal=C_ALB, int culoareText=C_ROSU){
    int w = textwidth(txt);
    int h = textheight(txt);

    setfillstyle(SOLID_FILL, culoareFundal);
    bar((SCREEN_W - w)/2, y, (SCREEN_W - w)/2 + w, y + h);

    setbkcolor(culoareFundal);
    setcolor(culoareText);
    settextstyle(3,0,size);
    outtextxy((SCREEN_W - w)/2, y, txt);
}



void deseneazaTabla() {

    setcolor(C_PORTOCALIU);
    textCentrat(20, turn==1?(char*)"Randul: VULPE":(char*)"Randul: CAINI",4);

    for(int i=0;i<8;i++){
        for(int j=0;j<8;j++){
            int x=BOARD_X+j*CELL;
            int y=BOARD_Y+i*CELL;

            if((i+j)%2==0) setfillstyle(SOLID_FILL,C_GRI);
            else setfillstyle(SOLID_FILL,C_ALB);

            if(i==selR && j==selC) setfillstyle(SOLID_FILL,C_HIGHLIGHT);

            bar(x,y,x+CELL,y+CELL);

            if(board[i][j]==1){ // Vulpe
                readimagefile("vulpe.bmp", x, y, x+CELL, y+CELL);
            }
            if(board[i][j]==2){ // Caine
                readimagefile("caine.bmp", x, y, x+CELL, y+CELL);
            }

        }
    }

    if(selR!=-1){
        int dr[4]={1,1,-1,-1};
        int dc[4]={-1,1,-1,1};
        for(int k=0;k<4;k++){
            int r=selR+dr[k], c=selC+dc[k];
            if(valid(selR,selC,r,c,board[selR][selC])){
                int x=BOARD_X+c*CELL+30;
                int y=BOARD_Y+r*CELL+30;
                setfillstyle(SOLID_FILL,GREEN);
                fillellipse(x,y,8,8);
            }
        }
    }
}

void butonStatic(int x, int y, int w, char* text, int h=50) {
    int culoareFundal = C_ALB;
    setfillstyle(SOLID_FILL, culoareFundal);
    bar(x, y, x + w, y + h);

    setcolor(C_PORTOCALIU);
    rectangle(x, y, x + w, y + h);

    setbkcolor(culoareFundal);
    setcolor(C_ROSU);
    settextstyle(3, 0, 3);
    outtextxy(x + (w - textwidth(text)) / 2,
              y + (h - textheight(text)) / 2,
              text);
}




// MAIN
int main() {

    initwindow(SCREEN_W, SCREEN_H, "Fox and Hounds");
    srand(time(0));

    int pagina = 0;
    clock_t timerPC = 0;

    while(1) {

        if (kbhit()) {
            char c = getch();
            if (c == 27) { // ESC
                if(stare == 3) stare = 5; // pauza
                else if(stare == 5) stare = 3; // resume cu ESC
    }
}


        setactivepage(pagina);
        setvisualpage(1 - pagina);

        readimagefile("background.bmp", 0, 0, SCREEN_W, SCREEN_H);

        int mx = mousex();
        int my = mousey();

        if(stare == 0){
            butonStatic(250, 120, 300, (char*)"FOX & HOUNDS", 60);
            butonStatic(250, 500, 300, (char*)"By: M. A. Racovita & C. Caraman");
            buton(250,300,300,(char*)"START",mx,my); // buton START
            if(ismouseclick(WM_LBUTTONDOWN)){
                getmouseclick(WM_LBUTTONDOWN, mx, my);
                if(mx > 250 && mx < 550 && my > 300 && my < 350){
                    stare = 1;
                }
            }

        }

        else if(stare == 1){
            butonStatic(250, 120, 300, (char*)"Alege Modul:", 60);
            buton(250,250,300,(char*)"Player vs PC",mx,my);
            buton(250,350,300,(char*)"Player vs Player",mx,my);
            if(ismouseclick(WM_LBUTTONDOWN)){
                getmouseclick(WM_LBUTTONDOWN,mx,my);
                if(my>250&&my<300){isPvP=0;stare=2;}
                if(my>350&&my<400){isPvP=1;stare=2;}
            }
        }

        else if(stare == 2){
            butonStatic(250, 120, 300, (char*)"Alege Caracter:", 60);
            buton(250,250,300,(char*)"VULPE",mx,my);
            buton(250,350,300,(char*)"CAINI",mx,my);
            if(ismouseclick(WM_LBUTTONDOWN)){
                getmouseclick(WM_LBUTTONDOWN,mx,my);
                if(my>250&&my<300){euSuntVulpe=1;initJoc();stare=3;}
                if(my>350&&my<400){euSuntVulpe=0;initJoc();stare=3;}
            }
        }

        else if(stare == 3){
            deseneazaTabla();

            winner = verificaCastigator();
            if(winner) stare = 4;

            int eRandulMeu = isPvP || (euSuntVulpe && turn==1) || (!euSuntVulpe && turn==2);

            if(eRandulMeu){
                if(ismouseclick(WM_LBUTTONDOWN)){
                    getmouseclick(WM_LBUTTONDOWN,mx,my);
                    int c=(mx-BOARD_X)/CELL;
                    int r=(my-BOARD_Y)/CELL;
                    if(r>=0&&r<8&&c>=0&&c<8){
                        if(board[r][c]==turn){
                            selR=r; selC=c;
                        }
                        else if(selR!=-1 && valid(selR,selC,r,c,turn)){
                            board[r][c]=turn;
                            board[selR][selC]=0;
                            selR=-1;
                            turn=3-turn;
                            timerPC=clock();
                        }
                    }
                }
            }
            else if(clock()-timerPC>400){
                mutarePC(turn);
                timerPC=clock();
            }
        }

        else if(stare == 4){
            if (winner == 1)
                butonStatic(250, 150, 300, (char*)"VULPEA A CASTIGAT!", 60);
            else
                butonStatic(250, 150, 300, (char*)"CAINII AU CASTIGAT!", 60);
            buton(250,300,300,(char*)"Joaca Iar",mx,my);
            buton(250,380,300,(char*)"Iesire",mx,my);

            if(ismouseclick(WM_LBUTTONDOWN)) {
                getmouseclick(WM_LBUTTONDOWN,mx,my);
                if(mx>250 && mx<550 && my>300 && my<350) { stare = 1; } // Joaca iar
                if(mx>250 && mx<550 && my>380 && my<430) {               // Iesire
                    closegraph();
                    return 0;
                }
            }
        }

        else if(stare == 5) { // PAUZA

    setcolor(C_PORTOCALIU);
    butonStatic(250, 120, 300, (char*)"PAUZA", 60);

    buton(250, 250, 300, (char*)"Continua", mx, my);
    buton(250, 330, 300, (char*)"Meniu", mx, my);
    buton(250, 410, 300, (char*)"Iesire", mx, my);

    if(ismouseclick(WM_LBUTTONDOWN)) {
        getmouseclick(WM_LBUTTONDOWN, mx, my);

        if(mx > 250 && mx < 550 && my > 250 && my < 300) {
            stare = 3; //resume
        }

        if(mx > 250 && mx < 550 && my > 330 && my < 380) {
            stare = 0;
            selR = selC = -1;
            winner = 0; //return to game menu
        }

        if(mx > 250 && mx < 550 && my > 410 && my < 460) {
            closegraph(); //exit game
            return 0;
        }
    }
}


        pagina = 1 - pagina;
        delay(16);
    }
}
