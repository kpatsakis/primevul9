cindent_on(void)
{
    return (!p_paste && (curbuf->b_p_cin
# ifdef FEAT_EVAL
		    || *curbuf->b_p_inde != NUL
# endif
		    ));
}