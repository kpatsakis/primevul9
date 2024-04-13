signcolumn_on(win_T *wp)
{
    if (*wp->w_p_scl == 'n')
	return FALSE;
    if (*wp->w_p_scl == 'y')
	return TRUE;
    return (wp->w_buffer->b_signlist != NULL
# ifdef FEAT_NETBEANS_INTG
			|| wp->w_buffer->b_has_sign_column
# endif
		    );
}