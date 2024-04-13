static void parray(int d, js_Ast *list)
{
	pc('[');
	while (list) {
		assert(list->type == AST_LIST);
		pexpi(d, COMMA, list->a);
		list = list->b;
		if (list)
			comma();
	}
	pc(']');
}