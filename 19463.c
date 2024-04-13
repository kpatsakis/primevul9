static int prec(enum js_AstType type)
{
	switch (type) {
	case AST_IDENTIFIER:
	case EXP_IDENTIFIER:
	case EXP_NUMBER:
	case EXP_STRING:
	case EXP_REGEXP:
	case EXP_UNDEF:
	case EXP_NULL:
	case EXP_TRUE:
	case EXP_FALSE:
	case EXP_THIS:
	case EXP_ARRAY:
	case EXP_OBJECT:
		return 170;

	case EXP_FUN:
	case EXP_INDEX:
	case EXP_MEMBER:
	case EXP_CALL:
	case EXP_NEW:
		return 160;

	case EXP_POSTINC:
	case EXP_POSTDEC:
		return 150;

	case EXP_DELETE:
	case EXP_VOID:
	case EXP_TYPEOF:
	case EXP_PREINC:
	case EXP_PREDEC:
	case EXP_POS:
	case EXP_NEG:
	case EXP_BITNOT:
	case EXP_LOGNOT:
		return 140;

	case EXP_MOD:
	case EXP_DIV:
	case EXP_MUL:
		return 130;

	case EXP_SUB:
	case EXP_ADD:
		return 120;

	case EXP_USHR:
	case EXP_SHR:
	case EXP_SHL:
		return 110;

	case EXP_IN:
	case EXP_INSTANCEOF:
	case EXP_GE:
	case EXP_LE:
	case EXP_GT:
	case EXP_LT:
		return 100;

	case EXP_STRICTNE:
	case EXP_STRICTEQ:
	case EXP_NE:
	case EXP_EQ:
		return 90;

	case EXP_BITAND: return 80;
	case EXP_BITXOR: return 70;
	case EXP_BITOR: return 60;
	case EXP_LOGAND: return 50;
	case EXP_LOGOR: return 40;

	case EXP_COND:
		return 30;

	case EXP_ASS:
	case EXP_ASS_MUL:
	case EXP_ASS_DIV:
	case EXP_ASS_MOD:
	case EXP_ASS_ADD:
	case EXP_ASS_SUB:
	case EXP_ASS_SHL:
	case EXP_ASS_SHR:
	case EXP_ASS_USHR:
	case EXP_ASS_BITAND:
	case EXP_ASS_BITXOR:
	case EXP_ASS_BITOR:
		return 20;

#define COMMA 15

	case EXP_COMMA:
		return 10;

	default:
		return 0;
	}
}