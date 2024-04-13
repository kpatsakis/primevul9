static void pstm(int d, js_Ast *stm)
{
	if (stm->type == STM_BLOCK) {
		pblock(d, stm);
		return;
	}

	in(d);

	switch (stm->type) {
	case AST_FUNDEC:
		ps("function ");
		pexp(d, stm->a);
		pc('(');
		pargs(d, stm->b);
		pc(')'); sp(); pc('{'); nl();
		pstmlist(d, stm->c);
		in(d); pc('}');
		break;

	case STM_EMPTY:
		pc(';');
		break;

	case STM_VAR:
		ps("var ");
		pvarlist(d, stm->a);
		pc(';');
		break;

	case STM_IF:
		ps("if"); sp(); pc('('); pexp(d, stm->a); pc(')');
		pstmh(d, stm->b);
		if (stm->c) {
			nl(); in(d); ps("else");
			pstmh(d, stm->c);
		}
		break;

	case STM_DO:
		ps("do");
		pstmh(d, stm->a);
		nl();
		in(d); ps("while"); sp(); pc('('); pexp(d, stm->b); pc(')'); pc(';');
		break;

	case STM_WHILE:
		ps("while"); sp(); pc('('); pexp(d, stm->a); pc(')');
		pstmh(d, stm->b);
		break;

	case STM_FOR:
		ps("for"); sp(); pc('(');
		pexp(d, stm->a); pc(';'); sp();
		pexp(d, stm->b); pc(';'); sp();
		pexp(d, stm->c); pc(')');
		pstmh(d, stm->d);
		break;
	case STM_FOR_VAR:
		ps("for"); sp(); ps("(var ");
		pvarlist(d, stm->a); pc(';'); sp();
		pexp(d, stm->b); pc(';'); sp();
		pexp(d, stm->c); pc(')');
		pstmh(d, stm->d);
		break;
	case STM_FOR_IN:
		ps("for"); sp(); pc('(');
		pexp(d, stm->a); ps(" in ");
		pexp(d, stm->b); pc(')');
		pstmh(d, stm->c);
		break;
	case STM_FOR_IN_VAR:
		ps("for"); sp(); ps("(var ");
		pvarlist(d, stm->a); ps(" in ");
		pexp(d, stm->b); pc(')');
		pstmh(d, stm->c);
		break;

	case STM_CONTINUE:
		ps("continue");
		if (stm->a) {
			pc(' '); pexp(d, stm->a);
		}
		pc(';');
		break;

	case STM_BREAK:
		ps("break");
		if (stm->a) {
			pc(' '); pexp(d, stm->a);
		}
		pc(';');
		break;

	case STM_RETURN:
		ps("return");
		if (stm->a) {
			pc(' '); pexp(d, stm->a);
		}
		pc(';');
		break;

	case STM_WITH:
		ps("with"); sp(); pc('('); pexp(d, stm->a); pc(')');
		pstmh(d, stm->b);
		break;

	case STM_SWITCH:
		ps("switch"); sp(); pc('(');
		pexp(d, stm->a);
		pc(')'); sp(); pc('{'); nl();
		pcaselist(d, stm->b);
		in(d); pc('}');
		break;

	case STM_THROW:
		ps("throw "); pexp(d, stm->a); pc(';');
		break;

	case STM_TRY:
		ps("try");
		if (minify && stm->a->type != STM_BLOCK)
			pc(' ');
		pstmh(d, stm->a);
		if (stm->b && stm->c) {
			nl(); in(d); ps("catch"); sp(); pc('('); pexp(d, stm->b); pc(')');
			pstmh(d, stm->c);
		}
		if (stm->d) {
			nl(); in(d); ps("finally");
			pstmh(d, stm->d);
		}
		break;

	case STM_LABEL:
		pexp(d, stm->a); pc(':'); sp(); pstm(d, stm->b);
		break;

	case STM_DEBUGGER:
		ps("debugger");
		pc(';');
		break;

	default:
		pexp(d, stm);
		pc(';');
	}
}