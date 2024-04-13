term_replace_bs_del_keycode(char_u *ta_buf, int ta_len, int len)
{
    int		i;
    int		c;

    for (i = ta_len; i < ta_len + len; ++i)
    {
	if (ta_buf[i] == CSI && len - i > 2)
	{
	    c = TERMCAP2KEY(ta_buf[i + 1], ta_buf[i + 2]);
	    if (c == K_DEL || c == K_KDEL || c == K_BS)
	    {
		mch_memmove(ta_buf + i + 1, ta_buf + i + 3,
			(size_t)(len - i - 2));
		if (c == K_DEL || c == K_KDEL)
		    ta_buf[i] = DEL;
		else
		    ta_buf[i] = Ctrl_H;
		len -= 2;
	    }
	}
	else if (ta_buf[i] == '\r')
	    ta_buf[i] = '\n';
	if (has_mbyte)
	    i += (*mb_ptr2len_len)(ta_buf + i, ta_len + len - i) - 1;
    }
}