static void pbin(int d, int p, js_Ast *exp, const char *op)
{
	pexpi(d, p, exp->a);
	sp();
	ps(op);
	sp();
	pexpi(d, p, exp->b);
}