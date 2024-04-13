static void pstmh(int d, js_Ast *stm)
{
	if (stm->type == STM_BLOCK) {
		sp();
		pblock(d, stm);
	} else {
		nl();
		pstm(d+1, stm);
	}
}