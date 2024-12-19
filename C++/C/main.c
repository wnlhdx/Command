#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

int main()
{
//  twoone();
//  twotwo();
//  twothree();
//  twofour();
//  twofive();
//  twosix();
//  twoseven();
twoeight();
    return 0;
}

void twoone()
{
    printf("Li Hao\n");
    printf("Li\nHao\n");
    printf("Li ");
    printf("Hao");
}

void twotwo()
{
    printf("Li Hao\n");
    printf("ShangHai\n");
}


void twothree()
{
  // 2019-12-25 15:05:03整数表示是1577257503
  struct tm sttm;
  memset(&sttm,0,sizeof(sttm));

  sttm.tm_year=1995-1900; // 注意，要减1900
  sttm.tm_mon=6-1;        // 注意，要减1
  sttm.tm_mday=7;
  sttm.tm_hour=0;
  sttm.tm_min=0;
  sttm.tm_sec=0;
  sttm.tm_isdst = 0;
  time_t birthday=mktime(&sttm);
  double diftime=difftime(time(0),birthday);
  int days=(int)(diftime/(3600*24));
  printf("days %d,years %d",days,days/365);
}

void twofour(){
    for(int i=0;i<=2;i++)
           jolly();
    deny();
}


void jolly(){
    printf(" For he's a jolly good fellow!\n");
}
void deny(){
    printf(" Which nobody can deny!\n");
}

void twofive(){
    br();
    printf(",");
    ic();
    ic();
    br();
}

void br(){
    printf(" Brazil, Russia");
}
void ic(){
    printf(" India, China\n");
}

void twosix(){
    int toes=10;
    int d=toes*toes;
    int t=d*toes;
    printf("%d,%d,%d",toes,d,t);
}
void twoseven(){
    for(int i=3;i>0;i--){
        for(int j=i;j>0;j--){
            s();
        }
        printf("\n");
    }
}
void s(){
    printf("Smile!");
}

void twoeight(){
    printf("starting now\n");
    one_three();
    printf("done!");
}

void one_three(){
    printf("one\n");
    two();
    printf("three\n");
}
void two(){
    printf("two");
}
