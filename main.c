#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ncurses.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>

#include "defines.h"
#include "types.h"
#include "prop.h"

/* �ץ�ȥ�������� */
static void PrintHighScore( void );
static void NewScore(int score);

static int atom[ATOM_MAX]; /* ����̿������� */
static char atomstr_ga[ATOM_MAX][STRLENMAX]; /* ����֤��פ�Ȥ�ʸ����*/
static char atomstr_ha[ATOM_MAX][STRLENMAX]; /* ����֤ϡפ�Ȥ�ʸ����*/
static char negatomstr_ga[ATOM_MAX][STRLENMAX]; /* �֤��������� */
static char negatomstr_ha[ATOM_MAX][STRLENMAX]; /* �֤ϡ������� */

/* ����̿��ʸ����ꥹ�������� */
typedef struct AtomString {
  char* subj; /* ��� */
  char* pred; /* �Ҹ� */
  char* neg_pred; /* ����Ҹ� */
}AtomString;

/* ����̿��ʸ����ꥹ��. 
   ��ʸ����϶����Х���+'\0'�Ǥʤ���Фʤ�ʤ�. 
   ʸ����Ĺ������������ɽ���ΰ褬­��ʤ��ʤ�ΤǤۤɤۤɤ�.
*/
static AtomString as_list[] = {
  {"��Τ����δ�", "�֤�", "�֤��ʤ�"},
  {"��Τ����", "��äƤ���", "����դǤ���"},
  {"��Τ����", "���򤷤Ƥ���", "���򤷤Ƥ��ʤ�"},
  {"��Τ�������", "���줷�Ƥ���", "���줷�Ƥ��ʤ�"},
  {"��Τ����", "�ۤäƤ���", "���äƤ���"}, //5

  {"��Τ����", "�������Ǥ���", "�������ǤϤʤ�"},
  {"��Τ����", "�ФäƤ���", "�ФäƤ��ʤ�"},
  {"��Τ����", "̲�äƤ���", "�����Ƥ���"},
  {"��Τ����", "�ܤ򳫤��Ƥ���", "�ܤ��Ĥ��Ƥ���"},
  {"��Τ����", "�ٶ����Ƥ���", "�ٶ����Ƥ��ʤ�"}, //10

  {"��Τ����", "�������Ƥ���", "�����Ĥ��Ƥ���"},
  {"���α���", "���äƤ���", "���äƤ��ʤ�"},
  {"���α���", "ӹ�äƤ���", "ӹ�äƤ��ʤ�"},
  {"���ޤ�","�����ܤ餻��","�����ܤ餻�Ƥ��ʤ�"},
  {"���ޤ�","�餱�Ƥ���","���äƤ���"}, //15

  {"�����δ�", "�֤�", "�֤��ʤ�"},
  {"����", "��äƤ���", "����դǤ���"},
  {"����", "���򤷤Ƥ���", "���򤷤Ƥ��ʤ�"},
  {"��������", "���줷�Ƥ���", "���줷�Ƥ��ʤ�"},
  {"����", "�ۤäƤ���", "���äƤ���"}, //20

  {"����", "�������Ǥ���", "�������ǤϤʤ�"},
  {"����", "�ФäƤ���", "�ФäƤ��ʤ�"},
  {"����", "̲�äƤ���", "�����Ƥ���"},
  {"����", "�ܤ򳫤��Ƥ���", "�ܤ��Ĥ��Ƥ���"},
  {"����", "�ٶ����Ƥ���", "�ٶ����Ƥ��ʤ�"}, //25

  {"����", "�������Ƥ���", "�����Ĥ��Ƥ���"},
  {"��","�����֤��������","�����֤�������Фʤ�"},
  {"���ޤ�","���ޤǿ��ä��ѥ�������Ф��Ƥ���","���ޤǿ��ä��ѥ�������Ф��Ƥ��ʤ�"},
  {"���ޤ�","�⤦���Ǥ���","�ޤ����Ǥ��ʤ�"},
  {"�ҥ�Ĺ���������˾�����","�ʤ�","����"}, //30

  {"��Τ�����ȱ","Ĺ��","û��"},
  {"������ȱ","Ĺ��","û��"},
  {"����","Ŭ�ڤʽ����Ǥ���","Ŭ�ڤʽ����Ǥʤ�"},
  {"��","�᤯","�ᤫ�ʤ�"},
  {"����","�٤���","�٤���ʤ�"},//35

  {"ǤŷƲ","�ƥȥꥹ���ä�","�ƥȥꥹ���äƤʤ�"},
  {"����","����ॹ���ä�","����ॹ���äƤʤ�"},
  {"���Ի�","��ž�֤�ű���","��ž�֤�ű��ʤ�"},
  {"�ӡ���","���ޤ�","�ޤ���"},
  {"���ޤ�","���ޤǤ���","���ޤǤʤ�"},//40

  {"�ͤ�Ƭ","�����Τ��󤳤Ĥ��Ǥ�","�����Τ��󤳤Ĥ����餫��"},
  {"�ڥ�","����궯��","�����夤"},
  {"����","����","�餱��"},
  {"��Τ����","�ᥤ�ɤǤ���","�ᥤ�ɤǤʤ�"},
  {"����","�ᥤ�ɤǤ���","�ᥤ�ɤǤʤ�"},//45

  {"���ʤ���̾��","�ָ����ȥ���פǤ���","�ָ����ȥ���פǤʤ�"},
};
#define ATOMSTRING_MAX 46 /* ��������������ǿ� */

Prop Condition[CONDITION_MAX]; /* ����������� */

static char ConditionAtom[ATOM_MAX]; /* ��ɾ�������� */

static int Problem_num; /* ���߽��꤬��λ���Ƥ�������� */
static int Correct_num; /* ����������������ο� */

static int ScoreSum; /* ������ */

/* ������� */
static int Difficulty_num_list[3][10]
= {{ 2,2,2,2,2, 2,2,2,2,2 },
   { 2,2,2,3,3, 3,4,4,5,5 },
   { 3,3,3,4,4, 4,5,5,5,5 }};
static int Num_Smax_list[6] = {0,0,50000,200000,500000,600000};
static int Difficulty=0; /* ����� */

/* �ϥ��������� */
static int HighScore[10];
static char ScoreName[10][4];

static int mygetch( void )
{
  int c;
  fd_set fds_set;
  struct timeval exittime;

  /* �Ԥ����֤����� */
  exittime.tv_usec = 0;
  exittime.tv_sec = 600;
  //  exittime.tv_sec = 20;
  
  /* �ǥ�������ץ��������� */
  FD_ZERO(&fds_set);
  FD_SET(0,&fds_set);
    
  /* ���쥯�� */
  while(select(1,&fds_set,NULL,NULL,&exittime) == -1);

  /* �����ॢ���Ȥˤ�뽪λ */
  if(!(FD_ISSET(0,&fds_set))){
    if((rand()%10 ==0))PrintHighScore();
    endwin();
    exit(0);
  }

  c = getch();

  if(c == CTRL_L){
    touchwin(curscr);
    wrefresh(curscr);
    return mygetch();
  }
  if( c == CTRL_C ){
    endwin();
    exit(0); 
  }

  return c;
}


/* ������̿�����ä�, ������б����� ����֤��פ�ʸ�ؤΥݥ��󥿤��֤� */
static char* prim2str_ga(Prim arg)
{
  if(arg.sign == 1)return atomstr_ga[arg.atom];
  return negatomstr_ga[arg.atom];
}

/* ������̿�����ä�, ������б����� ����֤ϡפ�ʸ�ؤΥݥ��󥿤��֤� */
static char* prim2str_ha(Prim arg)
{
  if(arg.sign == 1)return atomstr_ha[arg.atom];
  return negatomstr_ha[arg.atom];
}

/* ����̿���win�κ�ɸy,x����ɽ������. ����, ��§������Ԥ�. 
   �� refresh�Ϥ��ʤ�. 
   �� "�֡�", "�֡�", "�֡�"�η������ꤷ�Ƥ��ʤ�. (�Ȥ��Ȼפ���) */
static void mymvwprintw(WINDOW* win, int y, int x, char* str)
{
  int len;
  char buff[STRLENMAX];
  int kinsoku = 0;

  len = strlen(str);

  if(len < 77){
    mvwprintw(win,y,x,str);
    return;
  }

  /* ��ʸ��¿���ʤ��§���� */
  if( !strncmp("��",str+77,2) || 
      !strncmp("��",str+77,2) ||
      !strncmp("��",str+77,2) )kinsoku=2;

  /* ��ʸ�������ʤ��§���� */
  if( !strncmp("��",str+75,2) )kinsoku=-2;

  strncpy(buff, str, 77+kinsoku);
  buff[77+kinsoku]='\0';
  mvwprintw(win,y,x,buff);
  mvwprintw(win,y+1,x+3,str+77+kinsoku);

  return;
}

/* ����̿�����̤˽��Ϥ���. 
   �إܤ����ԡ���§����������äƤ��ʤ��Τ�, ʸ���������. */
static void print_condition(int arg)
{
  char buff[STRLENMAX];

  switch(Condition[arg].proptype){
  case A_IMP_B:
    sprintf(buff, 
	    "�� %s�Ȥ���%s��\n", 
	    prim2str_ga(Condition[arg].first),
	    prim2str_ha(Condition[arg].second));
    break;
  case A_OR_B:
    sprintf(buff, 
	    "�� %s�� �ޤ��� %s��\n", 
	    prim2str_ha(Condition[arg].first),
	    prim2str_ha(Condition[arg].second));
    break;
  case A_AND_B_IMP_C:
    sprintf(buff, 
	    "�� %s�Ȥ��ǡ�������%s�Ȥ���%s��\n", 
	    prim2str_ga(Condition[arg].first),
	    prim2str_ga(Condition[arg].second),
	    prim2str_ha(Condition[arg].third));
    break;
  case A_OR_B_IMP_C:
    sprintf(buff, 
	    "�� %s���ޤ���%s�Ȥ���%s��\n", 
	    prim2str_ga(Condition[arg].first),
	    prim2str_ga(Condition[arg].second),
	    prim2str_ha(Condition[arg].third));
    break;
  }
  mymvwprintw(stdscr, arg*2+2, 0,buff);
  wrefresh(stdscr);
  return;
}

/* �����Τ��������������� */
static int Alternative[ATOM_MAX];
/* �����Τ����������������ɽ��. realvalue��true�ʤ鿧���դ���. */
static int printalternative( int idx, int conditionnum, int locate, int realvalue)
{
  int i;
  int flag=0;

  for( i = 0 ; i < conditionnum ; i ++ )
    if(is_there(idx, Condition[i]))flag = 1;

  if(flag == 0)return 0;

  if(realvalue){
    wattron(stdscr, COLOR_PAIR(1));
    switch(realvalue){
    case ONLY_TRUE:
      mvwprintw(stdscr, locate, 2 , "[!][ ][ ] %s��", atomstr_ha[idx]);
      break;
    case ONLY_FALSE:
      mvwprintw(stdscr, locate, 2 , "[ ][!][ ] %s��", atomstr_ha[idx]);
      break;
    case ANYWAY:
      mvwprintw(stdscr, locate, 2 , "[ ][ ][!] %s��", atomstr_ha[idx]);
      break;
    }
    wattroff(stdscr, COLOR_PAIR(1));
  }
  else 
    mvwprintw(stdscr, locate, 2 , "[ ][ ][ ] %s��", atomstr_ha[idx]);
  Alternative[locate-14]=idx;

  return 1;
}

/* ����(����)���֤β�����ɽ�� */
static void printchoice(int* choice, int x , int y , int low)
{
  int i;
  for( i=14 ; i <= low ; i++ ){
    switch(choice[i-14]){
    case UNCLEAR:
      mvwprintw(stdscr, i,2,"[ ][ ][ ]");
      break;
    case ONLY_TRUE:
      mvwprintw(stdscr, i,2,"[+][ ][ ]");
      break;
    case ONLY_FALSE:
      mvwprintw(stdscr, i,2,"[ ][+][ ]");
      break;
    case ANYWAY:
      mvwprintw(stdscr, i,2,"[ ][ ][+]");
      break;
    }
  }
  mvwprintw(stdscr, 19, 2, "[ ]����");
  mvwprintw(stdscr, y,x,"*");
  wrefresh(stdscr);
}

static int playerselect(int* choice, int x , int y , int low)
{
  int i;

  switch( mygetch() ){
  case 'h':
  case CKEY_LEFT:
    if( y == 19 )break;
    x = x == 3 ? 9 : x-3;
    break;
  case 'j':
  case CKEY_DOWN:
    if( y == low ){
      y = 19;
      x = 3;
    }
    else if( y == 19 )y = 14;
    else y++;
    break;
  case 'k':
  case CKEY_UP:
    if( y == 19 )y = low;
    else if( y == 14 ){
      y = 19;
      x = 3;
    }
    else y--;
    break;
  case 'l':
  case CKEY_RIGHT:
    if( y == 19 )break;
    x = x == 9 ? 3 : x+3;
    break;
  case ' ':
  case CTRL_M:
    if( y == 19 ){
      int flag = 0;
      for( i = 0 ; i <= low-14 ; i++ )if( choice[i] == UNCLEAR )flag = 1;
      if(flag)break;
      printchoice(choice, x,y,low);
      return 0;
    }
    else {
      switch( x ){
      case 3:
	choice[y-14]=ONLY_TRUE;
	break;
      case 6:
	choice[y-14]=ONLY_FALSE;
	break;
      case 9:
	choice[y-14]=ANYWAY;
	break;
      }
    }
  }
  printchoice(choice, x,y,low);
  return playerselect(choice,x,y,low);
}

static void PrintHint(char boolatom, int atomidx, 
		     int* extracted, int ex_prop_num)
{
  int i;
  char* boolstr;
  char* negboolstr;
  
  if(boolatom == ONLY_TRUE){
    boolstr="��";
    negboolstr="��";
  }
  else { /* �����Ǥ�ɬ�� boolatom == ONLY_FALSE */
    boolstr="��";
    negboolstr="��";
  }
  
  /* Ŭ���˾ä� */
  for( i=14 ; i < 22 ; i++)
    mvwprintw(stdscr, i,0,"                                                                              ");
  wrefresh(stdscr);
  
  mvwprintw(stdscr, 15, 2,"��%s�פ�%s�Ȳ��ꤷ�ޤ���", 
	    atomstr_ha[atomidx], negboolstr);
  mvwprintw(stdscr, 16, 2,"�����");
  for( i = 0 ; i < ex_prop_num ; i++)
    wprintw(stdscr, "��%2d����", extracted[i]+1);
  wprintw(stdscr, "��̷�⤷�ޤ���");
  mvwprintw(stdscr, 17, 2,"�������äơ�%s�פ�%s�Ǥ���", 
	    atomstr_ha[atomidx], boolstr);

  wrefresh(stdscr);
  
  return;
}

/* ̿�����Ф���. ��Ū�ο���Ф�����, ���������뤫�ͤ���, ��������1���֤�.
   ��������ʤ��Ȥ���0���֤�. */
static int Hint_aux( int prop_max, int* extracted, 
		      int prop_min, int ex_prop_num, int depth )
{
  int i;

  if(depth == 0){
    /* �����Ǥ�ͽ�ꤵ�줿����̿�꤬��Ф���Ƥ��� */
    char boolatom[ATOM_MAX];
    Prop ex_prop[CONDITION_MAX];

    for(i=0; i < ex_prop_num ; i++)
      ex_prop[i]=Condition[extracted[i]];

    make_answer(ex_prop_num, ex_prop);
    get_answer(boolatom);
    for(i=0; i < ATOM_MAX ; i++){
      if( boolatom[i] == ONLY_TRUE || boolatom[i] == ONLY_FALSE ){
	/* �������ȯ���� */
	PrintHint(boolatom[i],i,extracted,ex_prop_num);
	return 1;
      }
    }
    return 0;
  }

  /* �⤦������ */
  for( i = prop_min ; i < prop_max ; i++){
    extracted[ex_prop_num]=i;
    if(Hint_aux(prop_max, extracted, i+1, ex_prop_num+1, depth-1)){
      /* ���������Ǥ�����1���֤� */
      return 1;
    }
  }

  return 0;
}
/* �ץ쥤�䤬�ְ�ä��Ȥ�, ��ˡ�Υҥ�Ȥ�Ф�����δؿ� 
   num������̿��ο�. */
static void Hint( int num )
{
  int depth;
  int extracted[CONDITION_MAX];
  
  for( depth = 2 ; depth < num+1 ; depth ++ ){
    if( Hint_aux(num,extracted,0,0,depth) )return; /* ���������Ǥ����齪λ */
  }
  return; /* cannot reach */
}

static int mainloop( void )
{
  int i;
  int num;
  int tmp;
  int locate;
  int scoremax;

  num = Difficulty_num_list[Difficulty][Problem_num];
  scoremax = Num_Smax_list[num];

  make_question(num,Condition); /* ������� */
  make_answer(num,Condition); /* �������� */
  get_answer(ConditionAtom);

  /* ���������å� */
  tmp = 0;
  for( i = 0 ; i < ATOM_MAX ; i++ )
    if( ConditionAtom[i] == ONLY_TRUE || ConditionAtom[i] == ONLY_FALSE )
      tmp ++;

  /* ��������������, �⤷�������꼫�Τ�̷�⤷�Ƥ���Ȥ��Ϥ��ľ��. */
  if( tmp == 0 )return mainloop();
  /* ��ĻĤ�������ΤȤ���7/10�Ǥ��ľ��. */
  if( tmp == 1 && (rand() % 10 < 7) )return mainloop();

  mvwprintw(stdscr, 0, 15, "��10��, ��%2d��           ������%07d          ����/���� : %d/%2d", Problem_num+1, ScoreSum, Correct_num, Problem_num);

  for( i = 0 ; i < num ; i++ ){
    print_condition(i);
  }

  mvwprintw(stdscr, 12, 2, "Ŭ�ڤʤ�Τ˥ޡ�������, ����򲡤��Ʋ�������");
  mvwprintw(stdscr, 13, 2, "�� �� ����");
  
  locate = 14;
  for( i = 0 ; i < ATOM_MAX ; i++ ){
    if(printalternative(i,num,locate,0))locate++;
  }

  wrefresh(stdscr);
  
  {
    int choice[ATOM_MAX];
    int ok;
    int time_begin;
    int localscore;

    time_begin = (int)time(NULL)-1;

    for( i = 0 ; i < ATOM_MAX ;  i++ )choice[i] = UNCLEAR;
    printchoice(choice, 3,14,locate-1);
    playerselect(choice, 3,14, locate-1);

    ok = 1;
    for( i = 0 ; i < locate-14 ; i ++ ){
      if(choice[i] != ConditionAtom[Alternative[i]]){
	ok=0;
	printalternative(Alternative[i],num,i+14,
			 ConditionAtom[Alternative[i]]);
      }
    }
    if(ok) {
      mvwprintw(stdscr, 19, 2, "����");
      Correct_num ++;
      localscore = scoremax/((int)time(NULL) - time_begin +10);
      localscore ++;
      mvwprintw(stdscr, 20, 2, "������%6d", localscore);
      ScoreSum += localscore;
    }
    else {
      int c;

      mvwprintw(stdscr, 19, 2, "��ǰ��");
      mvwprintw(stdscr, 21, 2, 
		"��ˡ�Υҥ�Ȥ��ɤߤޤ�����[Y/N]");
      wrefresh(stdscr);
      while(1){
	c = mygetch();
	if( c == 'N' || c == 'n' )break;
	if( c == 'Y' || c == 'y' ){
	  Hint(num);
	  break;
	}
      }
    }
    mvwprintw(stdscr, 21, 2, "Yet another OOSATO��(���������򲡤��Ʋ�������)");
    wrefresh(stdscr);

    mygetch();
  }


  for(i=0; i < 22 ; i++ ){
    mvwprintw(stdscr, i,0,"********************************************************************************");
    wrefresh(stdscr);
    usleep(5000);
  }
  for(i=0; i < 22 ; i++ ){
    mvwprintw(stdscr, i,0,"                                                                                ");
    wrefresh(stdscr);
    usleep(5000);
  }

  /*
  printf("\n̿���:%d\n",num);
  printf("  A:%s(%s)\n  B:%s(%s)\n  C:%s(%s)\n  D:%s(%s)\n  E:%s(%s)\n",
	 atomstr_ha[0], Answer[(int)(ConditionAtom[0])],
	 atomstr_ha[1], Answer[(int)(ConditionAtom[1])],
	 atomstr_ha[2], Answer[(int)(ConditionAtom[2])],
	 atomstr_ha[3], Answer[(int)(ConditionAtom[3])],
	 atomstr_ha[4], Answer[(int)(ConditionAtom[4])]);
  */
  return 1;
}

/* ����̿��ꥹ�Ȥ���Ŭ����̿�������. idx�Ϻ��ޤǤ����򤵤줿��. */
static int set_atom( int idx )
{
  int i;

  atom[idx] = rand() % ATOMSTRING_MAX;

  /* ���Ǥ�����Ǥ��������Ӥʤ����ʤ���Фʤ�ʤ�. */
  for( i = 0 ; i < idx ; i ++ )
    if( atom[i] == atom[idx] )return set_atom(idx);

  /* ����̿��˴ؤ���ʸ�������� */
  sprintf(atomstr_ga[idx], "%s��%s", 
	  as_list[atom[idx]].subj, as_list[atom[idx]].pred);
  sprintf(atomstr_ha[idx], "%s��%s", 
	  as_list[atom[idx]].subj, as_list[atom[idx]].pred);
  sprintf(negatomstr_ga[idx], "%s��%s", 
	  as_list[atom[idx]].subj, as_list[atom[idx]].neg_pred);
  sprintf(negatomstr_ha[idx], "%s��%s", 
	  as_list[atom[idx]].subj, as_list[atom[idx]].neg_pred);
  return 0;
}

/* ����٤⤷���ϥ�����ɽ��������. */
static int SelectDifficulty( void )
{
  int dif = Difficulty;

  while(1){
    wattroff(stdscr,COLOR_PAIR(2));
    if(dif == 0){
      wattron(stdscr, COLOR_PAIR(4));
      mvwprintw(stdscr,14,36," <EASY> ");
      wattroff(stdscr, COLOR_PAIR(4));
    }
    else
      mvwprintw(stdscr,14,36,"  EASY  ");

    if(dif == 1){
      wattron(stdscr, COLOR_PAIR(4));
      mvwprintw(stdscr,16,36,"<NORMAL>");
      wattroff(stdscr, COLOR_PAIR(4));
    }
    else
      mvwprintw(stdscr,16,36," NORMAL ");

    if(dif == 2){
      wattron(stdscr, COLOR_PAIR(4));
      mvwprintw(stdscr,18,36," <HARD> ");
      wattroff(stdscr, COLOR_PAIR(4));
    }
    else
      mvwprintw(stdscr,18,36,"  HARD  ");

    if(dif == 3){
      wattron(stdscr, COLOR_PAIR(4));
      mvwprintw(stdscr,20,36,"<SCORE >");
      wattroff(stdscr, COLOR_PAIR(4));
    }
    else
      mvwprintw(stdscr,20,36," SCORE  ");

    wrefresh(stdscr);

    switch(mygetch()) {
    case 'j':
    case CKEY_DOWN:
      dif++;
      if(dif == 4)dif =0;
      break;
    case 'k':
    case CKEY_UP:
      dif--;
      if(dif == -1)dif = 3;
      break;
    case ' ':
    case CTRL_M:
      return dif;
    }
  }

  return -1; /* cannot reach */
}

/* �����ȥ�ɽ�� */
static int DrawTitle( void )
{
  int i;
  int x = 20;
  for(i=0; i < 22 ; i++ ){
    mvwprintw(stdscr, i,0,"                                                                                ");
  }
  wrefresh(stdscr);

  wattron(stdscr,COLOR_PAIR(2));
  for(i=0;;i++){
    mvwprintw(stdscr,i,x,"��");
    wrefresh(stdscr);
    usleep(2000);
    if(i == 9)break;
    mvwprintw(stdscr,i,x,"  ");
    wrefresh(stdscr);
  }
  x+=6;
  for(i=0;;i++){
    mvwprintw(stdscr,i,x,"Τ");
    wrefresh(stdscr);
    usleep(2000);
    if(i == 9)break;
    mvwprintw(stdscr,i,x,"  ");
    wrefresh(stdscr);
  }
  x+=6;
  for(i=0;;i++){
    mvwprintw(stdscr,i,x,"��");
    wrefresh(stdscr);
    usleep(2000);
    if(i == 9)break;
    mvwprintw(stdscr,i,x,"  ");
    wrefresh(stdscr);
  }
  x+=6;
  for(i=0;;i++){
    mvwprintw(stdscr,i,x,"��");
    wrefresh(stdscr);
    usleep(2000);
    if(i == 9)break;
    mvwprintw(stdscr,i,x,"  ");
    wrefresh(stdscr);
  }
  x+=6;
  for(i=0;;i++){
    mvwprintw(stdscr,i,x,"��");
    wrefresh(stdscr);
    usleep(2000);
    if(i == 9)break;
    mvwprintw(stdscr,i,x,"  ");
    wrefresh(stdscr);
  }
  x+=6;
  for(i=0;;i++){
    mvwprintw(stdscr,i,x,"ͫ");
    wrefresh(stdscr);
    usleep(2000);
    if(i == 9)break;
    mvwprintw(stdscr,i,x,"  ");
    wrefresh(stdscr);
  }
  x+=6;
  for(i=0;;i++){
    mvwprintw(stdscr,i,x,"ݵ");
    wrefresh(stdscr);
    usleep(2000);
    if(i == 9)break;
    mvwprintw(stdscr,i,x,"  ");
    wrefresh(stdscr);
  }
  Difficulty = SelectDifficulty();
  for(i=0; i < 22 ; i++ ){
    mvwprintw(stdscr, i,0,"                                                                                ");
  }
  wrefresh(stdscr);
  
  if(Difficulty==3){
    PrintHighScore();
    return DrawTitle();
  }

  return 0;
}

/* ����ǥ��� */
static void PrintStaffs( void )
{
  int i;
  for(i=0; i < 22 ; i++ ){
    mvwprintw(stdscr, i,0,"                                                                                ");
  }
  wrefresh(stdscr);

  wattron(stdscr,COLOR_PAIR(2));
  for(i=22;;i--){
    mvwprintw(stdscr,i,20,"��    Τ    ��    ��    ��    ͫ    ݵ");
    wrefresh(stdscr);
    usleep(2222);
    if(i == 3)break;
    mvwprintw(stdscr,i,20,"                                      ");
    wrefresh(stdscr);
  }
  wattroff(stdscr,COLOR_PAIR(2));
  for(i=22;;i--){
    mvwprintw(stdscr,i,20,"    ��衦����ץ���ࡧhenkma");
    wrefresh(stdscr);
    usleep(2800);
    if(i == 8)break;
    mvwprintw(stdscr,i,20,"                                  ");
    wrefresh(stdscr);
  }
  for(i=22;;i--){
    mvwprintw(stdscr,i,20,"   �����ѡ��Х���������Τ����,manduki");
    wrefresh(stdscr);
    usleep(3000);
    if(i == 9)break;
    mvwprintw(stdscr,i,20,"                                     ");
    wrefresh(stdscr);
  }
  for(i=22;;i--){
    mvwprintw(stdscr,i,20,"         ������줿�͡���Τ����");
    wrefresh(stdscr);
    usleep(3300);
    if(i == 10)break;
    mvwprintw(stdscr,i,20,"                               ");
    wrefresh(stdscr);
  }
  for(i=22;;i--){
    mvwprintw(stdscr,i,20,"       ���ڥ���륵�󥯥����ˣͣ�");
    wrefresh(stdscr);
    usleep(5700);
    if(i == 13)break;
    mvwprintw(stdscr,i,20,"                                 ");
    wrefresh(stdscr);
  }
  for(i=22;;i--){
    mvwprintw(stdscr,i,13,"�Ǹ�ޤǥץ쥤���Ʋ����äƤɤ��⤢�꤬�Ȥ��������ޤ���");
    wrefresh(stdscr);
    usleep(8000);
    if(i == 15)break;
    mvwprintw(stdscr,i,13,"                                                      ");
    wrefresh(stdscr);
  }
  for(i=22;;i--){
    wattron(stdscr,COLOR_PAIR(3));
    mvwprintw(stdscr,i,21,"    ���ʤ�������Ψ�� %2d0%%�Ǥ�����    ", Correct_num);
    wattroff(stdscr,COLOR_PAIR(3));
    wrefresh(stdscr);
    usleep(8000);
    if(i == 17)break;
    mvwprintw(stdscr,i,21,"                                        ");
    wrefresh(stdscr);
  }
  for(i=22;;i--){
    wattron(stdscr,COLOR_PAIR(3));
    mvwprintw(stdscr,i,21," ���ʤ��ι�������� %07d���Ǥ�����", ScoreSum);
    wattroff(stdscr,COLOR_PAIR(3));
    wrefresh(stdscr);
    usleep(9000);
    if(i == 18)break;
    mvwprintw(stdscr,i,21,"                                      ");
    wrefresh(stdscr);
  }
  wrefresh(stdscr);


  if(ScoreSum <= HighScore[9]){
    mvwprintw(stdscr,20,21,"'Y'�򲡤��Ʋ������������ȥ�����ޤ���");
    wrefresh(stdscr);

    while(1){
      i = mygetch();
      if(i=='y'||i=='Y')break;
    }
  }
  else {
    NewScore(ScoreSum); /* ��󥯥��󤷤���̾������ */
  }
  for(i=0; i < 22 ; i++ ){
    mvwprintw(stdscr, i,0,"                                                                                ");
  }
  wrefresh(stdscr);
  
}

/* ����������ʥϥ�������ɽ��,  ��, ���ä� */
static void PrintHighScore( void )
{
  int i;
  for(i=0; i < 22 ; i++ ){
    mvwprintw(stdscr, i,0,"                                                                                ");
  }
  wrefresh(stdscr);

  mvwprintw(stdscr, 0,23,"���֡�%c%c%c%c %07d��"
	    ,ScoreName[0][0]
	    ,ScoreName[0][1]
	    ,ScoreName[0][2]
	    ,ScoreName[0][3]
	    ,HighScore[0]);
  mvwprintw(stdscr, 2,23,"���֡�%c%c%c%c %07d��"
	    ,ScoreName[1][0]
	    ,ScoreName[1][1]
	    ,ScoreName[1][2]
	    ,ScoreName[1][3]
	    ,HighScore[1]);
  mvwprintw(stdscr, 4,23,"���֡�%c%c%c%c %07d��"
	    ,ScoreName[2][0]
	    ,ScoreName[2][1]
	    ,ScoreName[2][2]
	    ,ScoreName[2][3]
	    ,HighScore[2]);
  mvwprintw(stdscr, 6,23,"���֡�%c%c%c%c %07d��"
	    ,ScoreName[3][0]
	    ,ScoreName[3][1]
	    ,ScoreName[3][2]
	    ,ScoreName[3][3]
	    ,HighScore[3]);
  mvwprintw(stdscr, 8,23,"���֡�%c%c%c%c %07d��"
	    ,ScoreName[4][0]
	    ,ScoreName[4][1]
	    ,ScoreName[4][2]
	    ,ScoreName[4][3]
	    ,HighScore[4]);
  mvwprintw(stdscr,10,23,"ϻ�֡�%c%c%c%c %07d��"
	    ,ScoreName[5][0]
	    ,ScoreName[5][1]
	    ,ScoreName[5][2]
	    ,ScoreName[5][3]
	    ,HighScore[5]);
  mvwprintw(stdscr,12,23,"���֡�%c%c%c%c %07d��"
	    ,ScoreName[6][0]
	    ,ScoreName[6][1]
	    ,ScoreName[6][2]
	    ,ScoreName[6][3]
	    ,HighScore[6]);
  mvwprintw(stdscr,14,23,"Ȭ�֡�%c%c%c%c %07d��"
	    ,ScoreName[7][0]
	    ,ScoreName[7][1]
	    ,ScoreName[7][2]
	    ,ScoreName[7][3]
	    ,HighScore[7]);
  mvwprintw(stdscr,16,23,"���֡�%c%c%c%c %07d��"
	    ,ScoreName[8][0]
	    ,ScoreName[8][1]
	    ,ScoreName[8][2]
	    ,ScoreName[8][3]
	    ,HighScore[8]);
  mvwprintw(stdscr,18,23,"���֡�%c%c%c%c %07d��"
	    ,ScoreName[9][0]
	    ,ScoreName[9][1]
	    ,ScoreName[9][2]
	    ,ScoreName[9][3]
	    ,HighScore[9]);

  mvwprintw(stdscr,21,40,"���������򲡤��Ʋ�������");
  wrefresh(stdscr);

  mygetch();

  for(i=0; i < 22 ; i++ ){
    mvwprintw(stdscr, i,0,"                                                                                ");
  }
  wrefresh(stdscr);
  return;
}

/* ��󥭥󥰥���ȥ�Υ���å� */
static void SwapEntry(int a, int b)
{
  int scoretmp;
  char nametmp[4];

  memcpy(nametmp,ScoreName[a],4);
  memcpy(ScoreName[a],ScoreName[b],4);
  memcpy(ScoreName[b],nametmp,4);

  scoretmp = HighScore[a];
  HighScore[a] = HighScore[b];
  HighScore[b] = scoretmp;

  return;
}

/* "score.txt"��, ���ߤΥ�󥭥󥰾����� */
static void SetHighScore( void )
{
  FILE* fp;
  int i;

  fp=fopen("./score.txt","w");
  for(i=0;i<10;i++){
    fprintf(fp,"%c%c%c%c:%d\n"
	    ,ScoreName[i][0],ScoreName[i][1],ScoreName[i][2],ScoreName[i][3],
	    HighScore[i]);
  }
  fclose(fp);
  return;
}

/* �������͡������� */
static void NewScore(int score)
{
  char name[4]="....";
  int i;
  int c;

  while(1){
    mvwprintw(stdscr,20,21,"̾�����ϡ�%c%c%c%c"
	      ,name[0],name[1],name[2],name[3]);
    wrefresh(stdscr);
    c=mygetch();
    if( c == CTRL_M )break;
    if( c < 32 || c > 126 )continue; /* �ޤȤ�Ǥʤ�ʸ���ϵѲ� */
    name[0]=name[1];
    name[1]=name[2];
    name[2]=name[3];
    name[3]=(char)c;
  }
  memcpy(ScoreName[9],name,4);
  HighScore[9]=score;

  for( i = 9 ; i > 0 ; i -- ){
    if( HighScore[i] >= HighScore[i-1] ){
      SwapEntry(i,i-1);
    }
  }
  SetHighScore();
  PrintHighScore();
  
  return;
}

/* "score.txt"����, ���ߤΥ�󥭥󥰾�����ɤ�. */
static void GetHighScore( void )
{
  char buff[99];
  FILE* fp;
  int i;
  int j;

  fp=fopen("./score.txt","r");
  for(i=0;i<10;i++){
    fgets(buff,80,fp);
    for(j=0;j<4;j++)ScoreName[i][j]=buff[j];
    HighScore[i]=atoi(buff+5);
  }
  
  fclose(fp);
  return;
}

int main( void )
{
  int i;
  int tmp;

  /* curses����� */
  initscr();
  crmode();
  nonl();
  noecho();
  raw();
  keypad(stdscr, TRUE);
  start_color();
  init_pair(1,COLOR_BLACK, COLOR_RED);
  init_pair(2,COLOR_CYAN, COLOR_BLACK);
  init_pair(3,COLOR_WHITE, COLOR_BLUE);
  init_pair(4,COLOR_BLACK, COLOR_WHITE);

  tmp = (int)time(NULL);
  printf("seed : %d\n",tmp);
  srand(tmp);

  GetHighScore();
  while(1){
    DrawTitle();
    Correct_num = 0; /* ����������� */
    ScoreSum = 0; /* �����ι�� */
    for(Problem_num = 0 ; Problem_num < 10 ; Problem_num++){
      for( i = 0 ; i < ATOM_MAX ; i++ )(void)set_atom(i);
      (void)mainloop();
    }
    PrintStaffs();
  }
  return 0;
}
