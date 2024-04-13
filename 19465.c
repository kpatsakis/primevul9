static void pcaselist(int d, js_Ast *list)
{
	while (list) {
		js_Ast *stm = list->a;
		if (stm->type == STM_CASE) {
			in(d); ps("case "); pexp(d, stm->a); pc(':'); nl();
			pstmlist(d, stm->b);
		}
		if (stm->type == STM_DEFAULT) {
			in(d); ps("default:"); nl();
			pstmlist(d, stm->a);
		}
		list = list->b;
	}
}