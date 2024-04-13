typval_compare_string(
	typval_T    *tv1,
	typval_T    *tv2,
	exprtype_T  type,
	int	    ic,
	int	    *res)
{
    int		i = 0;
    int		val = FALSE;
    char_u	*s1, *s2;
    char_u	buf1[NUMBUFLEN], buf2[NUMBUFLEN];

    if (in_vim9script()
	  && ((tv1->v_type != VAR_STRING && tv1->v_type != VAR_SPECIAL)
	   || (tv2->v_type != VAR_STRING && tv2->v_type != VAR_SPECIAL)))
    {
	semsg(_(e_cannot_compare_str_with_str),
		   vartype_name(tv1->v_type), vartype_name(tv2->v_type));
	return FAIL;
    }
    s1 = tv_get_string_buf(tv1, buf1);
    s2 = tv_get_string_buf(tv2, buf2);
    if (type != EXPR_MATCH && type != EXPR_NOMATCH)
	i = ic ? MB_STRICMP(s1, s2) : STRCMP(s1, s2);
    switch (type)
    {
	case EXPR_IS:	    if (in_vim9script())
			    {
				// Really check it is the same string, not just
				// the same value.
				val = tv1->vval.v_string == tv2->vval.v_string;
				break;
			    }
			    // FALLTHROUGH
	case EXPR_EQUAL:    val = (i == 0); break;
	case EXPR_ISNOT:    if (in_vim9script())
			    {
				// Really check it is not the same string, not
				// just a different value.
				val = tv1->vval.v_string != tv2->vval.v_string;
				break;
			    }
			    // FALLTHROUGH
	case EXPR_NEQUAL:   val = (i != 0); break;
	case EXPR_GREATER:  val = (i > 0); break;
	case EXPR_GEQUAL:   val = (i >= 0); break;
	case EXPR_SMALLER:  val = (i < 0); break;
	case EXPR_SEQUAL:   val = (i <= 0); break;

	case EXPR_MATCH:
	case EXPR_NOMATCH:
		val = pattern_match(s2, s1, ic);
		if (type == EXPR_NOMATCH)
		    val = !val;
		break;

	default:  break;  // avoid gcc warning
    }
    *res = val;
    return OK;
}