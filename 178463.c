clear_submatch_list(staticList10_T *sl)
{
    int i;

    for (i = 0; i < 10; ++i)
	vim_free(sl->sl_items[i].li_tv.vval.v_string);
}