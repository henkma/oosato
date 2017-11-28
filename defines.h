#ifndef __DEFINES_H__
#define __DEFINES_H__

/* curses��define */
#define CTRL_C 3
#define CTRL_L 12
#define CTRL_M 13
#define CKEY_LEFT  260
#define CKEY_RIGHT 261
#define CKEY_UP    259
#define CKEY_DOWN  258

/* ����̿����κ��� */
#define ATOM_MAX 5
/* ̿�깽��ˡ */
#define A_IMP_B       0 /* A��B    */
#define A_OR_B        1 /* A��B    */
#define A_AND_B_IMP_C 2 /* A��B��C */
#define A_OR_B_IMP_C  3 /* A��B��C */
#define PROPTYPE_MAX  4 /* ����ˡ�ο� */

#define STRLENMAX 999 /* ʸ�����Ĺ������(Ŭ��) */

#define CONDITION_MAX 5 /* �����κ��� */

/* ��ɾ����define */
#define UNCLEAR    0 /* ̤Ƚ�� */
#define ONLY_TRUE  1 /* �� */
#define ONLY_FALSE 2 /* �� */
#define ANYWAY     3 /* ����(�Ǥ��뤳�Ȥ�����) */

#endif /* __DEFINES_H__ */
