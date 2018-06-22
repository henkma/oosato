#include "defines.h"
#include "types.h"
#include "prop.h"

static char ConditionAtom[ATOM_MAX]; /* 式評価用配列 */
static Prop Condition[CONDITION_MAX]; /* 前提条件用配列 */

/* つまらない前提命題であるかどうか. 
   つまらないとは, 矛盾しているもの, 恒真なもの, 冗長なもののいずれか */
static int trivial(Prop arg)
{
  if( arg.first.atom == arg.second.atom )return 1;
  if( (arg.proptype == A_AND_B_IMP_C || arg.proptype == A_OR_B_IMP_C)
      && ( arg.first.atom == arg.third.atom 
	   || arg.second.atom == arg.third.atom ) )return 1;
  return 0;
}

/* 現在選択されている原子命題から, 原子命題もしくはその否定を作る */
static Prim make_prim()
{
  Prim retval;
  retval.atom = rand() % ATOM_MAX;
  if((rand() & (128+64)))retval.sign = 1; /* 75%は肯定 */
  else retval.sign = -1;                  /* 25%は否定 */
  return retval;
}

/* 小さな命題2つが等しいかどうか */
static int primeq(Prim p1, Prim p2)
{
  return ((p1.atom == p2.atom) && (p1.sign == p2.sign));
}

/* 命題が等しいかどうか */
static int propeq(Prop p1, Prop p2)
{
  if( p1.proptype != p2.proptype )return 0;
  switch( p1.proptype ){
  case A_IMP_B:
  case A_OR_B:
    return (primeq(p1.first, p2.first) && primeq(p1.second, p2.second));
  case A_AND_B_IMP_C:
  case A_OR_B_IMP_C:
    return (primeq(p1.first, p2.first) && primeq(p1.second, p2.second)
	    && primeq(p1.third, p2.third));
  }

  return 0; /* cannot reach */
}

/* 前提条件となる命題を作る. 引数idxは, 現在すでに作っている数 */
static Prop make_prop(int idx)
{
  int i;
  Prop retval;

  retval.first = make_prim();
  retval.second = make_prim();
  retval.third = make_prim();
  retval.proptype = rand() % PROPTYPE_MAX;

  /* つまんない命題は作り直し */
  if(trivial(retval))return make_prop(idx);

  /* 前と同じ命題は作り直し */
  for(i = 0 ; i < idx ; i++)
    if(propeq(Condition[i],retval))return make_prop(idx);

  return retval;
}

/* 指定された数だけ前提命題を構成する */
void make_question( int num, Prop* conditionlist )
{
  int i;

  for( i=0 ; i < num ; i ++ ){
    Condition[i] = make_prop(i);
    conditionlist[i] = Condition[i];
  }
  return;
}

static int testprop_aux(Prim prm, int a, int b, int c, int d, int e)
{
  switch(prm.atom){
  case 0:
    if(a)return prm.sign == 1;
    else return prm.sign == -1;
  case 1:
    if(b)return prm.sign == 1;
    else return prm.sign == -1;
  case 2:
    if(c)return prm.sign == 1;
    else return prm.sign == -1;
  case 3:
    if(d)return prm.sign == 1;
    else return prm.sign == -1;
  case 4:
    if(e)return prm.sign == 1;
    else return prm.sign == -1;
  }
  return 0; /* cannot reach */
}

/* 各原子命題の真偽を渡し, それでconsistentかどうか調べる.
   numは前提命題の数. */
int testprop( int a, int b, int c, int d, int e, int num, Prop* proplist )
{
  int i;
  for( i = 0 ; i < num ; i++){
    switch(proplist[i].proptype){
    case A_IMP_B:
      if( testprop_aux(proplist[i].first, a,b,c,d,e)
	  && !testprop_aux(proplist[i].second, a,b,c,d,e) )return 0;
      break;
    case A_OR_B:
      if( !testprop_aux(proplist[i].first, a,b,c,d,e)
	  && !testprop_aux(proplist[i].second, a,b,c,d,e) )return 0;
      break;
    case A_AND_B_IMP_C:
      if( testprop_aux(proplist[i].first, a,b,c,d,e)
	  && testprop_aux(proplist[i].second, a,b,c,d,e)
	  && !testprop_aux(proplist[i].third, a,b,c,d,e) )return 0;
      break;
    case A_OR_B_IMP_C:
      if( ( testprop_aux(proplist[i].first, a,b,c,d,e)
	    || testprop_aux(proplist[i].second, a,b,c,d,e) )
	  && !testprop_aux(proplist[i].third, a,b,c,d,e) )return 0;
      break;
    }
  }
  return 1;
}

/* 式の評価により, 情報を詳しくしていく関数 
   bigは, atmの真偽を表すビットとして見る. */
static void set_conditionatom(int big, int atm)
{
  if( ConditionAtom[atm] == ANYWAY )return;
  if( ConditionAtom[atm] == UNCLEAR ){
    if( big & (1 << atm) )ConditionAtom[atm] = ONLY_TRUE;
    else ConditionAtom[atm] = ONLY_FALSE;
    return;
  }
  switch(ConditionAtom[atm]){
  case ONLY_TRUE:
    if( big & (1<<atm) )return;
    break;
  case ONLY_FALSE:
    if( !(big & (1<<atm)) )return;
    break;
  }
  ConditionAtom[atm] = ANYWAY;
  return;
}

/* 全ての場合を考えてみて, 正しい解答をConditionAtomにセットする.
   これを呼んだ後はすぐにget_answerを呼ぶべき. */
void make_answer( int num , Prop* proplist )
{
  int i;

  for(i=0; i < ATOM_MAX ; i++ )ConditionAtom[i]=UNCLEAR; /* リセット */

  for(i=0; i < 1 << ATOM_MAX ; i++ ){
    if( testprop( i & 1 , i & 2 , i & 4 , i & 8 , i & 16, num, proplist) ){
      set_conditionatom(i & 1, 0);
      set_conditionatom(i & 2, 1);
      set_conditionatom(i & 4, 2);
      set_conditionatom(i & 8, 3);
      set_conditionatom(i & 16, 4);
    }
  }
  return;
}

/* 命題prpに, 原子命題idxが使われているかどうかを調べる */
int is_there(int idx, Prop prp)
{
  if(prp.first.atom == idx || prp.second.atom == idx)return 1;
  if( (prp.proptype == A_AND_B_IMP_C || prp.proptype == A_OR_B_IMP_C )
      && prp.third.atom == idx )return 1;
  return 0;
}

/* static char ConditionAtom[] の内容をargの中身に書く.
   不定でないものの数を返す. */
int get_answer(char* arg)
{
  int i;
  int retval=0;

  for(i=0;i<ATOM_MAX;i++){
    arg[i]=ConditionAtom[i];
    if(arg[i] == ONLY_TRUE || arg[i] == ONLY_FALSE)retval++;
  }
  return retval;
}
