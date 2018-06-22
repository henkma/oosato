#ifndef __TYPES_H__
#define __TYPES_H__

/* 原子命題, もしくはその否定 */
typedef struct Prim {
  int atom; /* 原子命題id */
  int sign; /* 符号,-1または+1 */
}Prim;

/* 命題 */
typedef struct Prop {
  int proptype; /* 命題の構成タイプ */
  Prim first;   /* 第一構成要素(A) */
  Prim second;  /* 第二構成要素(B) */
  Prim third;   /* 第三構成要素(C) これは使わないときもある */
}Prop;

#endif /* __TYPES_H__ */
