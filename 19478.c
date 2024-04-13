static void puna(int d, int p, js_Ast *exp, const char *pre, const char *suf)
{
	ps(pre);
	pexpi(d, p, exp->a);
	ps(suf);
}