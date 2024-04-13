static void pvar(int d, js_Ast *var)
{
	assert(var->type == EXP_VAR);
	pexp(d, var->a);
	if (var->b) {
		sp(); pc('='); sp();
		pexp(d, var->b);
	}
}