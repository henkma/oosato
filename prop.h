#ifndef __PROP_H__

/* ����̿��ο����Ϥ���, �������, Prop*�������. */
void make_question(int, Prop*);

/* �Ƹ���̿��ο������Ϥ�, �����consistent���ɤ���Ĵ�٤�.
   6�Ĥ��int��Prop����������ǿ�. */
int testprop( int, int, int, int, int, int, Prop* );

/* ���Ƥξ���ͤ��Ƥߤ�, ������������ConditionAtom�˥��åȤ���.
   ����������̿��ο�������. �����Ƥ����Ϥ�����get_answer��Ƥ֤٤�. */
void make_answer(int, Prop*);

/* ����������������. ����Ǥʤ���Το����֤�. */
int get_answer(char*);

/* �����Ȥ����Ϥ��줿̿���,
   �����Ȥ����Ϥ��줿����̿�꤬�Ȥ��Ƥ��뤫�ɤ�����Ĵ�٤� */
int is_there(int,Prop);

#endif
