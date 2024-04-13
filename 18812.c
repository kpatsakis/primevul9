free_cur_term()
{
# ifdef HAVE_DEL_CURTERM
    if (cur_term)
	del_curterm(cur_term);
# endif
}