static void pvarlist(int d, js_Ast *list)
{
	while (list) {
		assert(list->type == AST_LIST);
		pvar(d, list->a);
		list = list->b;
		if (list)
			comma();
	}
}