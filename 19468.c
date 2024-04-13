static void pobject(int d, js_Ast *list)
{
	pc('{');
	if (list) {
		nl();
		in(d+1);
	}
	while (list) {
		js_Ast *kv = list->a;
		assert(list->type == AST_LIST);
		switch (kv->type) {
		default: break;
		case EXP_PROP_VAL:
			pexpi(d+1, COMMA, kv->a);
			pc(':'); sp();
			pexpi(d+1, COMMA, kv->b);
			break;
		case EXP_PROP_GET:
			ps("get ");
			pexpi(d+1, COMMA, kv->a);
			ps("()"); sp(); pc('{'); nl();
			pstmlist(d+1, kv->c);
			in(d+1); pc('}');
			break;
		case EXP_PROP_SET:
			ps("set ");
			pexpi(d+1, COMMA, kv->a);
			pc('(');
			pargs(d+1, kv->b);
			pc(')'); sp(); pc('{'); nl();
			pstmlist(d+1, kv->c);
			in(d+1); pc('}');
			break;
		}
		list = list->b;
		if (list) {
			pc(',');
			nl();
			in(d+1);
		} else {
			nl();
			in(d);
		}
	}
	pc('}');
}