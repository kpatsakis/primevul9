text_to_screenline(win_T *wp, char_u *text, int col)
{
    int		off = (int)(current_ScreenLine - ScreenLines);

    if (has_mbyte)
    {
	int	cells;
	int	u8c, u8cc[MAX_MCO];
	int	i;
	int	idx;
	int	c_len;
	char_u	*p;
# ifdef FEAT_ARABIC
	int	prev_c = 0;		// previous Arabic character
	int	prev_c1 = 0;		// first composing char for prev_c
# endif

# ifdef FEAT_RIGHTLEFT
	if (wp->w_p_rl)
	    idx = off;
	else
# endif
	    idx = off + col;

	// Store multibyte characters in ScreenLines[] et al. correctly.
	for (p = text; *p != NUL; )
	{
	    cells = (*mb_ptr2cells)(p);
	    c_len = (*mb_ptr2len)(p);
	    if (col + cells > wp->w_width
# ifdef FEAT_RIGHTLEFT
		    - (wp->w_p_rl ? col : 0)
# endif
		    )
		break;
	    ScreenLines[idx] = *p;
	    if (enc_utf8)
	    {
		u8c = utfc_ptr2char(p, u8cc);
		if (*p < 0x80 && u8cc[0] == 0)
		{
		    ScreenLinesUC[idx] = 0;
#ifdef FEAT_ARABIC
		    prev_c = u8c;
#endif
		}
		else
		{
#ifdef FEAT_ARABIC
		    if (p_arshape && !p_tbidi && ARABIC_CHAR(u8c))
		    {
			// Do Arabic shaping.
			int	pc, pc1, nc;
			int	pcc[MAX_MCO];
			int	firstbyte = *p;

			// The idea of what is the previous and next
			// character depends on 'rightleft'.
			if (wp->w_p_rl)
			{
			    pc = prev_c;
			    pc1 = prev_c1;
			    nc = utf_ptr2char(p + c_len);
			    prev_c1 = u8cc[0];
			}
			else
			{
			    pc = utfc_ptr2char(p + c_len, pcc);
			    nc = prev_c;
			    pc1 = pcc[0];
			}
			prev_c = u8c;

			u8c = arabic_shape(u8c, &firstbyte, &u8cc[0],
								 pc, pc1, nc);
			ScreenLines[idx] = firstbyte;
		    }
		    else
			prev_c = u8c;
#endif
		    // Non-BMP character: display as ? or fullwidth ?.
		    ScreenLinesUC[idx] = u8c;
		    for (i = 0; i < Screen_mco; ++i)
		    {
			ScreenLinesC[i][idx] = u8cc[i];
			if (u8cc[i] == 0)
			    break;
		    }
		}
		if (cells > 1)
		    ScreenLines[idx + 1] = 0;
	    }
	    else if (enc_dbcs == DBCS_JPNU && *p == 0x8e)
		// double-byte single width character
		ScreenLines2[idx] = p[1];
	    else if (cells > 1)
		// double-width character
		ScreenLines[idx + 1] = p[1];
	    col += cells;
	    idx += cells;
	    p += c_len;
	}
    }
    else
    {
	int len = (int)STRLEN(text);

	if (len > wp->w_width - col)
	    len = wp->w_width - col;
	if (len > 0)
	{
#ifdef FEAT_RIGHTLEFT
	    if (wp->w_p_rl)
		mch_memmove(current_ScreenLine, text, len);
	    else
#endif
		mch_memmove(current_ScreenLine + col, text, len);
	    col += len;
	}
    }
    return col;
}