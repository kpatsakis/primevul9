static void pexpi(int d, int p, js_Ast *exp)
{
	int tp, paren;

	if (!exp) return;

	tp = prec(exp->type);
	paren = 0;
	if (tp < p) {
		pc('(');
		paren = 1;
	}
	p = tp;

	switch (exp->type) {
	case AST_IDENTIFIER: ps(exp->string); break;
	case EXP_IDENTIFIER: ps(exp->string); break;
	case EXP_NUMBER: printf("%.9g", exp->number); break;
	case EXP_STRING: pstr(exp->string); break;
	case EXP_REGEXP: pregexp(exp->string, exp->number); break;

	case EXP_UNDEF: break;
	case EXP_NULL: ps("null"); break;
	case EXP_TRUE: ps("true"); break;
	case EXP_FALSE: ps("false"); break;
	case EXP_THIS: ps("this"); break;

	case EXP_OBJECT: pobject(d, exp->a); break;
	case EXP_ARRAY: parray(d, exp->a); break;

	case EXP_DELETE: puna(d, p, exp, "delete ", ""); break;
	case EXP_VOID: puna(d, p, exp, "void ", ""); break;
	case EXP_TYPEOF: puna(d, p, exp, "typeof ", ""); break;
	case EXP_PREINC: puna(d, p, exp, "++", ""); break;
	case EXP_PREDEC: puna(d, p, exp, "--", ""); break;
	case EXP_POSTINC: puna(d, p, exp, "", "++"); break;
	case EXP_POSTDEC: puna(d, p, exp, "", "--"); break;
	case EXP_POS: puna(d, p, exp, "+", ""); break;
	case EXP_NEG: puna(d, p, exp, "-", ""); break;
	case EXP_BITNOT: puna(d, p, exp, "~", ""); break;
	case EXP_LOGNOT: puna(d, p, exp, "!", ""); break;

	case EXP_LOGOR: pbin(d, p, exp, "||"); break;
	case EXP_LOGAND: pbin(d, p, exp, "&&"); break;
	case EXP_BITOR: pbin(d, p, exp, "|"); break;
	case EXP_BITXOR: pbin(d, p, exp, "^"); break;
	case EXP_BITAND: pbin(d, p, exp, "&"); break;
	case EXP_EQ: pbin(d, p, exp, "=="); break;
	case EXP_NE: pbin(d, p, exp, "!="); break;
	case EXP_STRICTEQ: pbin(d, p, exp, "==="); break;
	case EXP_STRICTNE: pbin(d, p, exp, "!=="); break;
	case EXP_LT: pbin(d, p, exp, "<"); break;
	case EXP_GT: pbin(d, p, exp, ">"); break;
	case EXP_LE: pbin(d, p, exp, "<="); break;
	case EXP_GE: pbin(d, p, exp, ">="); break;
	case EXP_IN: pbin(d, p, exp, "in"); break;
	case EXP_SHL: pbin(d, p, exp, "<<"); break;
	case EXP_SHR: pbin(d, p, exp, ">>"); break;
	case EXP_USHR: pbin(d, p, exp, ">>>"); break;
	case EXP_ADD: pbin(d, p, exp, "+"); break;
	case EXP_SUB: pbin(d, p, exp, "-"); break;
	case EXP_MUL: pbin(d, p, exp, "*"); break;
	case EXP_DIV: pbin(d, p, exp, "/"); break;
	case EXP_MOD: pbin(d, p, exp, "%"); break;
	case EXP_ASS: pbin(d, p, exp, "="); break;
	case EXP_ASS_MUL: pbin(d, p, exp, "*="); break;
	case EXP_ASS_DIV: pbin(d, p, exp, "/="); break;
	case EXP_ASS_MOD: pbin(d, p, exp, "%="); break;
	case EXP_ASS_ADD: pbin(d, p, exp, "+="); break;
	case EXP_ASS_SUB: pbin(d, p, exp, "-="); break;
	case EXP_ASS_SHL: pbin(d, p, exp, "<<="); break;
	case EXP_ASS_SHR: pbin(d, p, exp, ">>="); break;
	case EXP_ASS_USHR: pbin(d, p, exp, ">>>="); break;
	case EXP_ASS_BITAND: pbin(d, p, exp, "&="); break;
	case EXP_ASS_BITXOR: pbin(d, p, exp, "^="); break;
	case EXP_ASS_BITOR: pbin(d, p, exp, "|="); break;

	case EXP_INSTANCEOF:
		pexpi(d, p, exp->a);
		ps(" instanceof ");
		pexpi(d, p, exp->b);
		break;

	case EXP_COMMA:
		pexpi(d, p, exp->a);
		pc(','); sp();
		pexpi(d, p, exp->b);
		break;

	case EXP_COND:
		pexpi(d, p, exp->a);
		sp(); pc('?'); sp();
		pexpi(d, p, exp->b);
		sp(); pc(':'); sp();
		pexpi(d, p, exp->c);
		break;

	case EXP_INDEX:
		pexpi(d, p, exp->a);
		pc('[');
		pexpi(d, 0, exp->b);
		pc(']');
		break;

	case EXP_MEMBER:
		pexpi(d, p, exp->a);
		pc('.');
		pexpi(d, 0, exp->b);
		break;

	case EXP_CALL:
		pexpi(d, p, exp->a);
		pc('(');
		pargs(d, exp->b);
		pc(')');
		break;

	case EXP_NEW:
		ps("new ");
		pexpi(d, p, exp->a);
		pc('(');
		pargs(d, exp->b);
		pc(')');
		break;

	case EXP_FUN:
		if (p == 0) pc('(');
		ps("function ");
		pexpi(d, 0, exp->a);
		pc('(');
		pargs(d, exp->b);
		pc(')'); sp(); pc('{'); nl();
		pstmlist(d, exp->c);
		in(d); pc('}');
		if (p == 0) pc(')');
		break;

	default:
		ps("<UNKNOWN>");
		break;
	}

	if (paren) pc(')');
}