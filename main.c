#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ncurses.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <locale.h>

#include "defines.h"
#include "types.h"
#include "prop.h"

/* プロトタイプ宣言 */
static void PrintHighScore( void );
static void NewScore(int score);

static int atom[ATOM_MAX]; /* 原子命題の配列 */
static char atomstr_ga[ATOM_MAX][STRLENMAX]; /* 助詞「が」を使う文字列*/
static char atomstr_ha[ATOM_MAX][STRLENMAX]; /* 助詞「は」を使う文字列*/
static char negatomstr_ga[ATOM_MAX][STRLENMAX]; /* 「が」否定版 */
static char negatomstr_ha[ATOM_MAX][STRLENMAX]; /* 「は」否定版 */

/* 原子命題文字列リスト用配列 */
typedef struct AtomString {
  char* subj; /* 主語 */
  char* pred; /* 述語 */
  char* neg_pred; /* 否定述語 */
}AtomString;

/* 原子命題文字列リスト. 
   各文字列は偶数バイト+'\0'でなければならない. 
   文字列が長すぎると選択肢表示領域が足りなくなるのでほどほどに.
*/
static AtomString as_list[] = {
  {"大里くんの顔", "赤い", "赤くない"},
  {"大里くん", "酔っている", "シラフである"},
  {"大里くん", "恋をしている", "恋をしていない"},
  {"大里くんの目", "充血している", "充血していない"},
  {"大里くん", "黙っている", "喋っている"}, //5

  {"大里くん", "小学生である", "小学生ではない"},
  {"大里くん", "笑っている", "笑っていない"},
  {"大里くん", "眠っている", "起きている"},
  {"大里くん", "目を開けている", "目を閉じている"},
  {"大里くん", "勉強している", "勉強していない"}, //10

  {"大里くん", "うろたえている", "おちついている"},
  {"俺の右手", "光っている", "光っていない"},
  {"俺の右手", "唸っている", "唸っていない"},
  {"おまえ","俺を怒らせた","俺を怒らせていない"},
  {"おまえ","負けている","勝っている"}, //15

  {"少女の顔", "赤い", "赤くない"},
  {"少女", "酔っている", "シラフである"},
  {"少女", "恋をしている", "恋をしていない"},
  {"少女の目", "充血している", "充血していない"},
  {"少女", "黙っている", "喋っている"}, //20

  {"少女", "小学生である", "小学生ではない"},
  {"少女", "笑っている", "笑っていない"},
  {"少女", "眠っている", "起きている"},
  {"少女", "目を開けている", "目を閉じている"},
  {"少女", "勉強している", "勉強していない"}, //25

  {"少女", "うろたえている", "おちついている"},
  {"俺","この赤の扉を選ぶ","この赤の扉を選ばない"},
  {"おまえ","今まで食ったパンの枚数を覚えている","今まで食ったパンの枚数を覚えていない"},
  {"おまえ","もう死んでいる","まだ死んでいない"},
  {"ヒゲ長き小学生に勝るもの","ない","ある"}, //30

  {"大里くんの髪","長い","短い"},
  {"少女の髪","長い","短い"},
  {"爆破","適切な処理である","適切な処理でない"},
  {"風","吹く","吹かない"},
  {"桶屋","儲かる","儲からない"},//35

  {"任天堂","テトリスを作った","テトリスを作ってない"},
  {"セガ","コラムスを作った","コラムスを作ってない"},
  {"京都市","自転車を撤去する","自転車を撤去しない"},
  {"ビール","うまい","まずい"},
  {"おまえ","辛党である","辛党でない"},//40

  {"僕の頭","親方のげんこつより固い","親方のげんこつよりやわらかい"},
  {"ペン","剣より強い","剣より弱い"},
  {"正義","勝つ","負ける"},
  {"大里くん","メイドである","メイドでない"},
  {"少女","メイドである","メイドでない"},//45

  {"あなたの名前","「公衆トイレ」である","「公衆トイレ」でない"},
};
#define ATOMSTRING_MAX 46 /* ↑この配列の要素数 */

Prop Condition[CONDITION_MAX]; /* 前提条件の配列 */

static char ConditionAtom[ATOM_MAX]; /* 式評価用配列 */

static int Problem_num; /* 現在出題が終了している問題数 */
static int Correct_num; /* 現在正答した問題の数 */

static int ScoreSum; /* スコア */

/* 難易度用 */
static int Difficulty_num_list[3][10]
= {{ 2,2,2,2,2, 2,2,2,2,2 },
   { 2,2,2,3,3, 3,4,4,5,5 },
   { 3,3,3,4,4, 4,5,5,5,5 }};
static int Num_Smax_list[6] = {0,0,50000,200000,500000,600000};
static int Difficulty=0; /* 難易度 */

/* ハイスコア用 */
static int HighScore[10];
static char ScoreName[10][4];

static int mygetch( void )
{
  int c;
  fd_set fds_set;
  struct timeval exittime;

  /* 待ち時間の設定 */
  exittime.tv_usec = 0;
  exittime.tv_sec = 600;
  //  exittime.tv_sec = 20;
  
  /* ディスクリプタ集合設定 */
  FD_ZERO(&fds_set);
  FD_SET(0,&fds_set);
    
  /* セレクト */
  while(select(1,&fds_set,NULL,NULL,&exittime) == -1);

  /* タイムアウトによる終了 */
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


/* 小さい命題をもらって, それに対応する 助詞「が」の文へのポインタを返す */
static char* prim2str_ga(Prim arg)
{
  if(arg.sign == 1)return atomstr_ga[arg.atom];
  return negatomstr_ga[arg.atom];
}

/* 小さい命題をもらって, それに対応する 助詞「は」の文へのポインタを返す */
static char* prim2str_ha(Prim arg)
{
  if(arg.sign == 1)return atomstr_ha[arg.atom];
  return negatomstr_ha[arg.atom];
}

/* 前提命題をwinの座標y,xから表示する. 改行, 禁則処理を行う. 
   ※ refreshはしない. 
   ※ "「。", "「、", "「」"の形は想定していない. (使わんと思うし) */
static void mymvwprintw(WINDOW* win, int y, int x, char* str)
{
  int len;
  char buff[STRLENMAX];
  int kinsoku = 0;

  len = strlen(str);

  if(len < 115){
    mvwprintw(win,y,x,str);
    return;
  }

  /* 一文字多くなる禁則処理 */
  if( !strncmp("。",str+115,3) || 
      !strncmp("、",str+115,3) ||
      !strncmp("」",str+115,3) )kinsoku=3;

  /* 一文字少くなる禁則処理 */
  if( !strncmp("「",str+115,3) )kinsoku=-3;

  strncpy(buff, str, 115+kinsoku);
  buff[115+kinsoku]='\0';
  mvwprintw(win,y,x,buff);
  mvwprintw(win,y+1,x+3,str+115+kinsoku);

  return;
}

/* 前提命題を画面に出力する. 
   ヘボい改行・禁則処理しか作っていないので, 文字数に注意. */
static void print_condition(int arg)
{
  char buff[STRLENMAX];

  switch(Condition[arg].proptype){
  case A_IMP_B:
    sprintf(buff, 
	    "・ %sとき、%s。\n", 
	    prim2str_ga(Condition[arg].first),
	    prim2str_ha(Condition[arg].second));
    break;
  case A_OR_B:
    sprintf(buff, 
	    "・ %s、 または %s。\n", 
	    prim2str_ha(Condition[arg].first),
	    prim2str_ha(Condition[arg].second));
    break;
  case A_AND_B_IMP_C:
    sprintf(buff, 
	    "・ %sときで、しかも%sとき、%s。\n", 
	    prim2str_ga(Condition[arg].first),
	    prim2str_ga(Condition[arg].second),
	    prim2str_ha(Condition[arg].third));
    break;
  case A_OR_B_IMP_C:
    sprintf(buff, 
	    "・ %s、または%sとき、%s。\n", 
	    prim2str_ga(Condition[arg].first),
	    prim2str_ga(Condition[arg].second),
	    prim2str_ha(Condition[arg].third));
    break;
  }
  mymvwprintw(stdscr, arg*2+2, 0,buff);
  wrefresh(stdscr);
  return;
}

/* 解答のための選択肢用配列 */
static int Alternative[ATOM_MAX];
/* 解答のための選択肢用配列の表示. realvalueがtrueなら色を付ける. */
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
      mvwprintw(stdscr, locate, 2 , "[!][ ][ ] %s。", atomstr_ha[idx]);
      break;
    case ONLY_FALSE:
      mvwprintw(stdscr, locate, 2 , "[ ][!][ ] %s。", atomstr_ha[idx]);
      break;
    case ANYWAY:
      mvwprintw(stdscr, locate, 2 , "[ ][ ][!] %s。", atomstr_ha[idx]);
      break;
    }
    wattroff(stdscr, COLOR_PAIR(1));
  }
  else 
    mvwprintw(stdscr, locate, 2 , "[ ][ ][ ] %s。", atomstr_ha[idx]);
  Alternative[locate-14]=idx;

  return 1;
}

/* 選択(途中)状態の回答欄表示 */
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
  mvwprintw(stdscr, 19, 2, "[ ]決定");
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
    boolstr="真";
    negboolstr="偽";
  }
  else { /* ここでは必ず boolatom == ONLY_FALSE */
    boolstr="偽";
    negboolstr="真";
  }
  
  /* 適当に消す */
  for( i=14 ; i < 22 ; i++)
    mvwprintw(stdscr, i,0,"                                                                              ");
  wrefresh(stdscr);
  
  mvwprintw(stdscr, 15, 2,"「%s」が%sと仮定します。", 
	    atomstr_ha[atomidx], negboolstr);
  mvwprintw(stdscr, 16, 2,"すると");
  for( i = 0 ; i < ex_prop_num ; i++)
    wprintw(stdscr, "、%2d番目", extracted[i]+1);
  wprintw(stdscr, "が矛盾します。");
  mvwprintw(stdscr, 17, 2,"したがって「%s」は%sです。", 
	    atomstr_ha[atomidx], boolstr);

  wrefresh(stdscr);
  
  return;
}

/* 命題を抽出する. 目的の数抽出したら, 何か言えるか考えて, 言えたら1を返す.
   何も言えないときは0を返す. */
static int Hint_aux( int prop_max, int* extracted, 
		      int prop_min, int ex_prop_num, int depth )
{
  int i;

  if(depth == 0){
    /* ここでは予定された数の命題が抽出されている */
    char boolatom[ATOM_MAX];
    Prop ex_prop[CONDITION_MAX];

    for(i=0; i < ex_prop_num ; i++)
      ex_prop[i]=Condition[extracted[i]];

    make_answer(ex_prop_num, ex_prop);
    get_answer(boolatom);
    for(i=0; i < ATOM_MAX ; i++){
      if( boolatom[i] == ONLY_TRUE || boolatom[i] == ONLY_FALSE ){
	/* 確定事項発見！ */
	PrintHint(boolatom[i],i,extracted,ex_prop_num);
	return 1;
      }
    }
    return 0;
  }

  /* もう一つ抽出 */
  for( i = prop_min ; i < prop_max ; i++){
    extracted[ex_prop_num]=i;
    if(Hint_aux(prop_max, extracted, i+1, ex_prop_num+1, depth-1)){
      /* 何か説明できたら1を返す */
      return 1;
    }
  }

  return 0;
}
/* プレイヤが間違ったとき, 解法のヒントを出すための関数 
   numは前提命題の数. */
static void Hint( int num )
{
  int depth;
  int extracted[CONDITION_MAX];
  
  for( depth = 2 ; depth < num+1 ; depth ++ ){
    if( Hint_aux(num,extracted,0,0,depth) )return; /* 何か説明できたら終了 */
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

  make_question(num,Condition); /* 問題作成 */
  make_answer(num,Condition); /* 解答作成 */
  get_answer(ConditionAtom);

  /* 解答チェック */
  tmp = 0;
  for( i = 0 ; i < ATOM_MAX ; i++ )
    if( ConditionAtom[i] == ONLY_TRUE || ConditionAtom[i] == ONLY_FALSE )
      tmp ++;

  /* 解答が全部不定, もしくは問題自体が矛盾しているときはやり直し. */
  if( tmp == 0 )return mainloop();
  /* 一個残して不定のときは7/10でやり直し. */
  if( tmp == 1 && (rand() % 10 < 7) )return mainloop();

  mvwprintw(stdscr, 0, 15, "全10題, 第%2d題           点数：%07d          正答/出題 : %d/%2d", Problem_num+1, ScoreSum, Correct_num, Problem_num);

  for( i = 0 ; i < num ; i++ ){
    print_condition(i);
  }

  mvwprintw(stdscr, 12, 2, "適切なものにマークして, 決定を押して下さい。");
  mvwprintw(stdscr, 13, 2, "真 偽 不定");
  
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
      mvwprintw(stdscr, 19, 2, "正解！");
      Correct_num ++;
      localscore = scoremax/((int)time(NULL) - time_begin +10);
      localscore ++;
      mvwprintw(stdscr, 20, 2, "得点：%6d", localscore);
      ScoreSum += localscore;
    }
    else {
      int c;

      mvwprintw(stdscr, 19, 2, "残念！");
      mvwprintw(stdscr, 21, 2, 
		"解法のヒントを読みますか？[Y/N]");
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
    mvwprintw(stdscr, 21, 2, "Yet another OOSATO！(何かキーを押して下さい。)");
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
  printf("\n命題数:%d\n",num);
  printf("  A:%s(%s)\n  B:%s(%s)\n  C:%s(%s)\n  D:%s(%s)\n  E:%s(%s)\n",
	 atomstr_ha[0], Answer[(int)(ConditionAtom[0])],
	 atomstr_ha[1], Answer[(int)(ConditionAtom[1])],
	 atomstr_ha[2], Answer[(int)(ConditionAtom[2])],
	 atomstr_ha[3], Answer[(int)(ConditionAtom[3])],
	 atomstr_ha[4], Answer[(int)(ConditionAtom[4])]);
  */
  return 1;
}

/* 原子命題リストから適当に命題を選択. idxは今までに選択された数. */
static int set_atom( int idx )
{
  int i;

  atom[idx] = rand() % ATOMSTRING_MAX;

  /* すでに選んでた場合は選びなおさなければならない. */
  for( i = 0 ; i < idx ; i ++ )
    if( atom[i] == atom[idx] )return set_atom(idx);

  /* 原子命題に関する文字列を作成 */
  sprintf(atomstr_ga[idx], "%sが%s", 
	  as_list[atom[idx]].subj, as_list[atom[idx]].pred);
  sprintf(atomstr_ha[idx], "%sは%s", 
	  as_list[atom[idx]].subj, as_list[atom[idx]].pred);
  sprintf(negatomstr_ga[idx], "%sが%s", 
	  as_list[atom[idx]].subj, as_list[atom[idx]].neg_pred);
  sprintf(negatomstr_ha[idx], "%sは%s", 
	  as_list[atom[idx]].subj, as_list[atom[idx]].neg_pred);
  return 0;
}

/* 難易度もしくはスコア表示を選択. */
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

/* タイトル表示 */
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
    mvwprintw(stdscr,i,x,"大");
    wrefresh(stdscr);
    usleep(2000);
    if(i == 9)break;
    mvwprintw(stdscr,i,x,"  ");
    wrefresh(stdscr);
  }
  x+=6;
  for(i=0;;i++){
    mvwprintw(stdscr,i,x,"里");
    wrefresh(stdscr);
    usleep(2000);
    if(i == 9)break;
    mvwprintw(stdscr,i,x,"  ");
    wrefresh(stdscr);
  }
  x+=6;
  for(i=0;;i++){
    mvwprintw(stdscr,i,x,"く");
    wrefresh(stdscr);
    usleep(2000);
    if(i == 9)break;
    mvwprintw(stdscr,i,x,"  ");
    wrefresh(stdscr);
  }
  x+=6;
  for(i=0;;i++){
    mvwprintw(stdscr,i,x,"ん");
    wrefresh(stdscr);
    usleep(2000);
    if(i == 9)break;
    mvwprintw(stdscr,i,x,"  ");
    wrefresh(stdscr);
  }
  x+=6;
  for(i=0;;i++){
    mvwprintw(stdscr,i,x,"の");
    wrefresh(stdscr);
    usleep(2000);
    if(i == 9)break;
    mvwprintw(stdscr,i,x,"  ");
    wrefresh(stdscr);
  }
  x+=6;
  for(i=0;;i++){
    mvwprintw(stdscr,i,x,"憂");
    wrefresh(stdscr);
    usleep(2000);
    if(i == 9)break;
    mvwprintw(stdscr,i,x,"  ");
    wrefresh(stdscr);
  }
  x+=6;
  for(i=0;;i++){
    mvwprintw(stdscr,i,x,"鬱");
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

/* エンディング */
static void PrintStaffs( void )
{
  int i;
  for(i=0; i < 22 ; i++ ){
    mvwprintw(stdscr, i,0,"                                                                                ");
  }
  wrefresh(stdscr);

  wattron(stdscr,COLOR_PAIR(2));
  for(i=22;;i--){
    mvwprintw(stdscr,i,20,"大    里    く    ん    の    憂    鬱");
    wrefresh(stdscr);
    usleep(2222);
    if(i == 3)break;
    mvwprintw(stdscr,i,20,"                                      ");
    wrefresh(stdscr);
  }
  wattroff(stdscr,COLOR_PAIR(2));
  for(i=22;;i--){
    mvwprintw(stdscr,i,20,"    企画・製作・プログラム：henkma");
    wrefresh(stdscr);
    usleep(2800);
    if(i == 8)break;
    mvwprintw(stdscr,i,20,"                                  ");
    wrefresh(stdscr);
  }
  for(i=22;;i--){
    mvwprintw(stdscr,i,20,"   スーパーバイザー：大里くん,manduki");
    wrefresh(stdscr);
    usleep(3000);
    if(i == 9)break;
    mvwprintw(stdscr,i,20,"                                     ");
    wrefresh(stdscr);
  }
  for(i=22;;i--){
    mvwprintw(stdscr,i,20,"         いじられた人：大里くん");
    wrefresh(stdscr);
    usleep(3300);
    if(i == 10)break;
    mvwprintw(stdscr,i,20,"                               ");
    wrefresh(stdscr);
  }
  for(i=22;;i--){
    mvwprintw(stdscr,i,20,"       スペシャルサンクス：ＫＭＣ");
    wrefresh(stdscr);
    usleep(5700);
    if(i == 13)break;
    mvwprintw(stdscr,i,20,"                                 ");
    wrefresh(stdscr);
  }
  for(i=22;;i--){
    mvwprintw(stdscr,i,13,"最後までプレイして下さってどうもありがとうございます！");
    wrefresh(stdscr);
    usleep(8000);
    if(i == 15)break;
    mvwprintw(stdscr,i,13,"                                                      ");
    wrefresh(stdscr);
  }
  for(i=22;;i--){
    wattron(stdscr,COLOR_PAIR(3));
    mvwprintw(stdscr,i,21,"    あなたの正解率は %2d0%%でした。    ", Correct_num);
    wattroff(stdscr,COLOR_PAIR(3));
    wrefresh(stdscr);
    usleep(8000);
    if(i == 17)break;
    mvwprintw(stdscr,i,21,"                                        ");
    wrefresh(stdscr);
  }
  for(i=22;;i--){
    wattron(stdscr,COLOR_PAIR(3));
    mvwprintw(stdscr,i,21," あなたの合計得点は %07d点でした。", ScoreSum);
    wattroff(stdscr,COLOR_PAIR(3));
    wrefresh(stdscr);
    usleep(9000);
    if(i == 18)break;
    mvwprintw(stdscr,i,21,"                                      ");
    wrefresh(stdscr);
  }
  wrefresh(stdscr);


  if(ScoreSum <= HighScore[9]){
    mvwprintw(stdscr,20,21,"'Y'を押して下さい。タイトルに戻ります。");
    wrefresh(stdscr);

    while(1){
      i = mygetch();
      if(i=='y'||i=='Y')break;
    }
  }
  else {
    NewScore(ScoreSum); /* ランクインしたら名前入力 */
  }
  for(i=0; i < 22 ; i++ ){
    mvwprintw(stdscr, i,0,"                                                                                ");
  }
  wrefresh(stdscr);
  
}

/* えーかげんなハイスコア表示,  ま, いっか */
static void PrintHighScore( void )
{
  int i;
  for(i=0; i < 22 ; i++ ){
    mvwprintw(stdscr, i,0,"                                                                                ");
  }
  wrefresh(stdscr);

  mvwprintw(stdscr, 0,23,"壱番：%c%c%c%c %07d点"
	    ,ScoreName[0][0]
	    ,ScoreName[0][1]
	    ,ScoreName[0][2]
	    ,ScoreName[0][3]
	    ,HighScore[0]);
  mvwprintw(stdscr, 2,23,"弐番：%c%c%c%c %07d点"
	    ,ScoreName[1][0]
	    ,ScoreName[1][1]
	    ,ScoreName[1][2]
	    ,ScoreName[1][3]
	    ,HighScore[1]);
  mvwprintw(stdscr, 4,23,"参番：%c%c%c%c %07d点"
	    ,ScoreName[2][0]
	    ,ScoreName[2][1]
	    ,ScoreName[2][2]
	    ,ScoreName[2][3]
	    ,HighScore[2]);
  mvwprintw(stdscr, 6,23,"四番：%c%c%c%c %07d点"
	    ,ScoreName[3][0]
	    ,ScoreName[3][1]
	    ,ScoreName[3][2]
	    ,ScoreName[3][3]
	    ,HighScore[3]);
  mvwprintw(stdscr, 8,23,"五番：%c%c%c%c %07d点"
	    ,ScoreName[4][0]
	    ,ScoreName[4][1]
	    ,ScoreName[4][2]
	    ,ScoreName[4][3]
	    ,HighScore[4]);
  mvwprintw(stdscr,10,23,"六番：%c%c%c%c %07d点"
	    ,ScoreName[5][0]
	    ,ScoreName[5][1]
	    ,ScoreName[5][2]
	    ,ScoreName[5][3]
	    ,HighScore[5]);
  mvwprintw(stdscr,12,23,"七番：%c%c%c%c %07d点"
	    ,ScoreName[6][0]
	    ,ScoreName[6][1]
	    ,ScoreName[6][2]
	    ,ScoreName[6][3]
	    ,HighScore[6]);
  mvwprintw(stdscr,14,23,"八番：%c%c%c%c %07d点"
	    ,ScoreName[7][0]
	    ,ScoreName[7][1]
	    ,ScoreName[7][2]
	    ,ScoreName[7][3]
	    ,HighScore[7]);
  mvwprintw(stdscr,16,23,"九番：%c%c%c%c %07d点"
	    ,ScoreName[8][0]
	    ,ScoreName[8][1]
	    ,ScoreName[8][2]
	    ,ScoreName[8][3]
	    ,HighScore[8]);
  mvwprintw(stdscr,18,23,"捨番：%c%c%c%c %07d点"
	    ,ScoreName[9][0]
	    ,ScoreName[9][1]
	    ,ScoreName[9][2]
	    ,ScoreName[9][3]
	    ,HighScore[9]);

  mvwprintw(stdscr,21,40,"何かキーを押して下さい。");
  wrefresh(stdscr);

  mygetch();

  for(i=0; i < 22 ; i++ ){
    mvwprintw(stdscr, i,0,"                                                                                ");
  }
  wrefresh(stdscr);
  return;
}

/* ランキングエントリのスワップ */
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

/* "score.txt"に, 現在のランキング情報を書く */
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

/* スコアネーム入力 */
static void NewScore(int score)
{
  char name[4]="....";
  int i;
  int c;

  while(1){
    mvwprintw(stdscr,20,21,"名前入力：%c%c%c%c"
	      ,name[0],name[1],name[2],name[3]);
    wrefresh(stdscr);
    c=mygetch();
    if( c == CTRL_M )break;
    if( c < 32 || c > 126 )continue; /* まともでない文字は却下 */
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

/* "score.txt"から, 現在のランキング情報を読む. */
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

  /* curses初期化 */
  setlocale(LC_ALL, "");
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
    Correct_num = 0; /* 正答数初期化 */
    ScoreSum = 0; /* 得点の合計 */
    for(Problem_num = 0 ; Problem_num < 10 ; Problem_num++){
      for( i = 0 ; i < ATOM_MAX ; i++ )(void)set_atom(i);
      (void)mainloop();
    }
    PrintStaffs();
  }
  return 0;
}
