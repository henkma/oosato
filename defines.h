#ifndef __DEFINES_H__
#define __DEFINES_H__

/* curses用define */
#define CTRL_C 3
#define CTRL_L 12
#define CTRL_M 13
#define CKEY_LEFT  260
#define CKEY_RIGHT 261
#define CKEY_UP    259
#define CKEY_DOWN  258

/* 原子命題数の最大 */
#define ATOM_MAX 5
/* 命題構成法 */
#define A_IMP_B       0 /* A⊃B    */
#define A_OR_B        1 /* A∨B    */
#define A_AND_B_IMP_C 2 /* A∧B⊃C */
#define A_OR_B_IMP_C  3 /* A∨B⊃C */
#define PROPTYPE_MAX  4 /* 構成法の数 */

#define STRLENMAX 999 /* 文字列の長さ最大(適当) */

#define CONDITION_MAX 5 /* 条件数の最大 */

/* 式評価用define */
#define UNCLEAR    0 /* 未判明 */
#define ONLY_TRUE  1 /* 真 */
#define ONLY_FALSE 2 /* 偽 */
#define ANYWAY     3 /* 不定(であることが確定) */

#endif /* __DEFINES_H__ */
