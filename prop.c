#include "defines.h"
#include "types.h"
#include "prop.h"

static char ConditionAtom[ATOM_MAX]; /* ��ɾ�������� */
static Prop Condition[CONDITION_MAX]; /* ������������ */

/* �Ĥޤ�ʤ�����̿��Ǥ��뤫�ɤ���. 
   �Ĥޤ�ʤ��Ȥ�, ̷�⤷�Ƥ�����, �����ʤ��, ��Ĺ�ʤ�ΤΤ����줫 */
static int trivial(Prop arg)
{
  if( arg.first.atom == arg.second.atom )return 1;
  if( (arg.proptype == A_AND_B_IMP_C || arg.proptype == A_OR_B_IMP_C)
      && ( arg.first.atom == arg.third.atom 
	   || arg.second.atom == arg.third.atom ) )return 1;
  return 0;
}

/* �������򤵤�Ƥ��븶��̿�꤫��, ����̿��⤷���Ϥ���������� */
static Prim make_prim()
{
  Prim retval;
  retval.atom = rand() % ATOM_MAX;
  if((rand() & (128+64)))retval.sign = 1; /* 75%�Ϲ��� */
  else retval.sign = -1;                  /* 25%������ */
  return retval;
}

/* ������̿��2�Ĥ����������ɤ��� */
static int primeq(Prim p1, Prim p2)
{
  return ((p1.atom == p2.atom) && (p1.sign == p2.sign));
}

/* ̿�꤬���������ɤ��� */
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

/* ������Ȥʤ�̿�����. ����idx��, ���ߤ��Ǥ˺�äƤ���� */
static Prop make_prop(int idx)
{
  int i;
  Prop retval;

  retval.first = make_prim();
  retval.second = make_prim();
  retval.third = make_prim();
  retval.proptype = rand() % PROPTYPE_MAX;

  /* �Ĥޤ�ʤ�̿��Ϻ��ľ�� */
  if(trivial(retval))return make_prop(idx);

  /* ����Ʊ��̿��Ϻ��ľ�� */
  for(i = 0 ; i < idx ; i++)
    if(propeq(Condition[i],retval))return make_prop(idx);

  return retval;
}

/* ���ꤵ�줿����������̿��������� */
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

/* �Ƹ���̿��ο������Ϥ�, �����consistent���ɤ���Ĵ�٤�.
   num������̿��ο�. */
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

/* ����ɾ���ˤ��, �����ܤ������Ƥ����ؿ� 
   big��, atm�ο�����ɽ���ӥåȤȤ��Ƹ���. */
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

/* ���Ƥξ���ͤ��Ƥߤ�, ������������ConditionAtom�˥��åȤ���.
   �����Ƥ����Ϥ�����get_answer��Ƥ֤٤�. */
void make_answer( int num , Prop* proplist )
{
  int i;

  for(i=0; i < ATOM_MAX ; i++ )ConditionAtom[i]=UNCLEAR; /* �ꥻ�å� */

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

/* ̿��prp��, ����̿��idx���Ȥ��Ƥ��뤫�ɤ�����Ĵ�٤� */
int is_there(int idx, Prop prp)
{
  if(prp.first.atom == idx || prp.second.atom == idx)return 1;
  if( (prp.proptype == A_AND_B_IMP_C || prp.proptype == A_OR_B_IMP_C )
      && prp.third.atom == idx )return 1;
  return 0;
}

/* static char ConditionAtom[] �����Ƥ�arg����Ȥ˽�.
   ����Ǥʤ���Το����֤�. */
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
