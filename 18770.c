cterm_color2rgb(int nr, char_u *r, char_u *g, char_u *b, char_u *ansi_idx)
{
    int idx;

    if (nr < 16)
    {
#if defined(MSWIN)
	idx = cterm_ansi_idx[nr];
#else
	idx = nr;
#endif
	*r = ansi_table[idx][0];
	*g = ansi_table[idx][1];
	*b = ansi_table[idx][2];
	*ansi_idx = idx + 1;
    }
    else if (nr < 232)
    {
	// 216 color cube
	idx = nr - 16;
	*r = cube_value[idx / 36 % 6];
	*g = cube_value[idx / 6  % 6];
	*b = cube_value[idx      % 6];
	*ansi_idx = ANSI_INDEX_NONE;
    }
    else if (nr < 256)
    {
	// 24 grey scale ramp
	idx = nr - 232;
	*r = grey_ramp[idx];
	*g = grey_ramp[idx];
	*b = grey_ramp[idx];
	*ansi_idx = ANSI_INDEX_NONE;
    }
    else
    {
	*r = 0;
	*g = 0;
	*b = 0;
	*ansi_idx = ANSI_INDEX_NONE;
    }
}