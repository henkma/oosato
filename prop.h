#ifndef __PROP_H__

/* 前提命題の数を渡して, 問題を作り, Prop*に入れる. */
void make_question(int, Prop*);

/* 各原子命題の真偽を渡し, それでconsistentかどうか調べる.
   6つめのintはPropの配列の要素数. */
int testprop( int, int, int, int, int, int, Prop* );

/* 全ての場合を考えてみて, 正しい解答をConditionAtomにセットする.
   引数は前提命題の数と配列. これを呼んだ後はすぐにget_answerを呼ぶべき. */
void make_answer(int, Prop*);

/* 正しい解答を得る. 不定でないものの数を返す. */
int get_answer(char*);

/* 引数として渡された命題に,
   引数として渡された原子命題が使われているかどうかを調べる */
int is_there(int,Prop);

#endif
