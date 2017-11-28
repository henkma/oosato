#ifndef __TYPES_H__
#define __TYPES_H__

/* ����̿��, �⤷���Ϥ������� */
typedef struct Prim {
  int atom; /* ����̿��id */
  int sign; /* ���,-1�ޤ���+1 */
}Prim;

/* ̿�� */
typedef struct Prop {
  int proptype; /* ̿��ι��������� */
  Prim first;   /* ��칽������(A) */
  Prim second;  /* ����������(B) */
  Prim third;   /* �軰��������(C) ����ϻȤ�ʤ��Ȥ��⤢�� */
}Prop;

#endif /* __TYPES_H__ */
