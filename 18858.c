del_termcode_idx(int idx)
{
    int		i;

    vim_free(termcodes[idx].code);
    --tc_len;
    for (i = idx; i < tc_len; ++i)
	termcodes[i] = termcodes[i + 1];
}