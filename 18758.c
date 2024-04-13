get_literal(int noReduceKeys)
{
    int		cc;
    int		nc;
    int		i;
    int		hex = FALSE;
    int		octal = FALSE;
    int		unicode = 0;

    if (got_int)
	return Ctrl_C;

#ifdef FEAT_GUI
    /*
     * In GUI there is no point inserting the internal code for a special key.
     * It is more useful to insert the string "<KEY>" instead.	This would
     * probably be useful in a text window too, but it would not be
     * vi-compatible (maybe there should be an option for it?) -- webb
     */
    if (gui.in_use)
	++allow_keys;
#endif
#ifdef USE_ON_FLY_SCROLL
    dont_scroll = TRUE;		// disallow scrolling here
#endif
    ++no_mapping;		// don't map the next key hits
    cc = 0;
    i = 0;
    for (;;)
    {
	nc = plain_vgetc();
	if ((nc == ESC || nc == CSI) && !noReduceKeys)
	    nc = decodeModifyOtherKeys(nc);

	if ((mod_mask & ~MOD_MASK_SHIFT) != 0)
	    // A character with non-Shift modifiers should not be a valid
	    // character for i_CTRL-V_digit.
	    break;

#ifdef FEAT_CMDL_INFO
	if (!(State & CMDLINE) && MB_BYTE2LEN_CHECK(nc) == 1)
	    add_to_showcmd(nc);
#endif
	if (nc == 'x' || nc == 'X')
	    hex = TRUE;
	else if (nc == 'o' || nc == 'O')
	    octal = TRUE;
	else if (nc == 'u' || nc == 'U')
	    unicode = nc;
	else
	{
	    if (hex || unicode != 0)
	    {
		if (!vim_isxdigit(nc))
		    break;
		cc = cc * 16 + hex2nr(nc);
	    }
	    else if (octal)
	    {
		if (nc < '0' || nc > '7')
		    break;
		cc = cc * 8 + nc - '0';
	    }
	    else
	    {
		if (!VIM_ISDIGIT(nc))
		    break;
		cc = cc * 10 + nc - '0';
	    }

	    ++i;
	}

	if (cc > 255 && unicode == 0)
	    cc = 255;		// limit range to 0-255
	nc = 0;

	if (hex)		// hex: up to two chars
	{
	    if (i >= 2)
		break;
	}
	else if (unicode)	// Unicode: up to four or eight chars
	{
	    if ((unicode == 'u' && i >= 4) || (unicode == 'U' && i >= 8))
		break;
	}
	else if (i >= 3)	// decimal or octal: up to three chars
	    break;
    }
    if (i == 0)	    // no number entered
    {
	if (nc == K_ZERO)   // NUL is stored as NL
	{
	    cc = '\n';
	    nc = 0;
	}
	else
	{
	    cc = nc;
	    nc = 0;
	}
    }

    if (cc == 0)	// NUL is stored as NL
	cc = '\n';
    if (enc_dbcs && (cc & 0xff) == 0)
	cc = '?';	// don't accept an illegal DBCS char, the NUL in the
			// second byte will cause trouble!

    --no_mapping;
#ifdef FEAT_GUI
    if (gui.in_use)
	--allow_keys;
#endif
    if (nc)
    {
	vungetc(nc);
	// A character typed with i_CTRL-V_digit cannot have modifiers.
	mod_mask = 0;
    }
    got_int = FALSE;	    // CTRL-C typed after CTRL-V is not an interrupt
    return cc;
}