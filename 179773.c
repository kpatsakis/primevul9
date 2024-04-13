do_c_expr_indent(void)
{
# ifdef FEAT_EVAL
    if (*curbuf->b_p_inde != NUL)
	fixthisline(get_expr_indent);
    else
# endif
	fixthisline(get_c_indent);
}