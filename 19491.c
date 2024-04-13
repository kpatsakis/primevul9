static void pstmlist(int d, js_Ast *list)
{
	while (list) {
		assert(list->type == AST_LIST);
		pstm(d+1, list->a);
		nl();
		list = list->b;
	}
}