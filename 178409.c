vim_regsub_both(
    char_u	*source,
    typval_T	*expr,
    char_u	*dest,
    int		destlen,
    int		flags)
{
    char_u	*src;
    char_u	*dst;
    char_u	*s;
    int		c;
    int		cc;
    int		no = -1;
    fptr_T	func_all = (fptr_T)NULL;
    fptr_T	func_one = (fptr_T)NULL;
    linenr_T	clnum = 0;	// init for GCC
    int		len = 0;	// init for GCC
#ifdef FEAT_EVAL
    static int  nesting = 0;
    int		nested;
#endif
    int		copy = flags & REGSUB_COPY;

    // Be paranoid...
    if ((source == NULL && expr == NULL) || dest == NULL)
    {
	emsg(_(e_null_argument));
	return 0;
    }
    if (prog_magic_wrong())
	return 0;
#ifdef FEAT_EVAL
    if (nesting == MAX_REGSUB_NESTING)
    {
	emsg(_(e_substitute_nesting_too_deep));
	return 0;
    }
    nested = nesting;
#endif
    src = source;
    dst = dest;

    /*
     * When the substitute part starts with "\=" evaluate it as an expression.
     */
    if (expr != NULL || (source[0] == '\\' && source[1] == '='))
    {
#ifdef FEAT_EVAL
	// To make sure that the length doesn't change between checking the
	// length and copying the string, and to speed up things, the
	// resulting string is saved from the call with
	// "flags & REGSUB_COPY" == 0 to the call with
	// "flags & REGSUB_COPY" != 0.
	if (copy)
	{
	    if (eval_result[nested] != NULL)
	    {
		STRCPY(dest, eval_result[nested]);
		dst += STRLEN(eval_result[nested]);
		VIM_CLEAR(eval_result[nested]);
	    }
	}
	else
	{
	    int		    prev_can_f_submatch = can_f_submatch;
	    regsubmatch_T   rsm_save;

	    VIM_CLEAR(eval_result[nested]);

	    // The expression may contain substitute(), which calls us
	    // recursively.  Make sure submatch() gets the text from the first
	    // level.
	    if (can_f_submatch)
		rsm_save = rsm;
	    can_f_submatch = TRUE;
	    rsm.sm_match = rex.reg_match;
	    rsm.sm_mmatch = rex.reg_mmatch;
	    rsm.sm_firstlnum = rex.reg_firstlnum;
	    rsm.sm_maxline = rex.reg_maxline;
	    rsm.sm_line_lbr = rex.reg_line_lbr;

	    // Although unlikely, it is possible that the expression invokes a
	    // substitute command (it might fail, but still).  Therefore keep
	    // an array of eval results.
	    ++nesting;

	    if (expr != NULL)
	    {
		typval_T	argv[2];
		char_u		buf[NUMBUFLEN];
		typval_T	rettv;
		staticList10_T	matchList;
		funcexe_T	funcexe;

		rettv.v_type = VAR_STRING;
		rettv.vval.v_string = NULL;
		argv[0].v_type = VAR_LIST;
		argv[0].vval.v_list = &matchList.sl_list;
		matchList.sl_list.lv_len = 0;
		CLEAR_FIELD(funcexe);
		funcexe.fe_argv_func = fill_submatch_list;
		funcexe.fe_evaluate = TRUE;
		if (expr->v_type == VAR_FUNC)
		{
		    s = expr->vval.v_string;
		    call_func(s, -1, &rettv, 1, argv, &funcexe);
		}
		else if (expr->v_type == VAR_PARTIAL)
		{
		    partial_T   *partial = expr->vval.v_partial;

		    s = partial_name(partial);
		    funcexe.fe_partial = partial;
		    call_func(s, -1, &rettv, 1, argv, &funcexe);
		}
		else if (expr->v_type == VAR_INSTR)
		{
		    exe_typval_instr(expr, &rettv);
		}
		if (matchList.sl_list.lv_len > 0)
		    // fill_submatch_list() was called
		    clear_submatch_list(&matchList);

		if (rettv.v_type == VAR_UNKNOWN)
		    // something failed, no need to report another error
		    eval_result[nested] = NULL;
		else
		{
		    eval_result[nested] = tv_get_string_buf_chk(&rettv, buf);
		    if (eval_result[nested] != NULL)
			eval_result[nested] = vim_strsave(eval_result[nested]);
		}
		clear_tv(&rettv);
	    }
	    else if (substitute_instr != NULL)
		// Execute instructions from ISN_SUBSTITUTE.
		eval_result[nested] = exe_substitute_instr();
	    else
		eval_result[nested] = eval_to_string(source + 2, TRUE);
	    --nesting;

	    if (eval_result[nested] != NULL)
	    {
		int had_backslash = FALSE;

		for (s = eval_result[nested]; *s != NUL; MB_PTR_ADV(s))
		{
		    // Change NL to CR, so that it becomes a line break,
		    // unless called from vim_regexec_nl().
		    // Skip over a backslashed character.
		    if (*s == NL && !rsm.sm_line_lbr)
			*s = CAR;
		    else if (*s == '\\' && s[1] != NUL)
		    {
			++s;
			/* Change NL to CR here too, so that this works:
			 * :s/abc\\\ndef/\="aaa\\\nbbb"/  on text:
			 *   abc\
			 *   def
			 * Not when called from vim_regexec_nl().
			 */
			if (*s == NL && !rsm.sm_line_lbr)
			    *s = CAR;
			had_backslash = TRUE;
		    }
		}
		if (had_backslash && (flags & REGSUB_BACKSLASH))
		{
		    // Backslashes will be consumed, need to double them.
		    s = vim_strsave_escaped(eval_result[nested], (char_u *)"\\");
		    if (s != NULL)
		    {
			vim_free(eval_result[nested]);
			eval_result[nested] = s;
		    }
		}

		dst += STRLEN(eval_result[nested]);
	    }

	    can_f_submatch = prev_can_f_submatch;
	    if (can_f_submatch)
		rsm = rsm_save;
	}
#endif
    }
    else
      while ((c = *src++) != NUL)
      {
	if (c == '&' && (flags & REGSUB_MAGIC))
	    no = 0;
	else if (c == '\\' && *src != NUL)
	{
	    if (*src == '&' && !(flags & REGSUB_MAGIC))
	    {
		++src;
		no = 0;
	    }
	    else if ('0' <= *src && *src <= '9')
	    {
		no = *src++ - '0';
	    }
	    else if (vim_strchr((char_u *)"uUlLeE", *src))
	    {
		switch (*src++)
		{
		case 'u':   func_one = (fptr_T)do_upper;
			    continue;
		case 'U':   func_all = (fptr_T)do_Upper;
			    continue;
		case 'l':   func_one = (fptr_T)do_lower;
			    continue;
		case 'L':   func_all = (fptr_T)do_Lower;
			    continue;
		case 'e':
		case 'E':   func_one = func_all = (fptr_T)NULL;
			    continue;
		}
	    }
	}
	if (no < 0)	      // Ordinary character.
	{
	    if (c == K_SPECIAL && src[0] != NUL && src[1] != NUL)
	    {
		// Copy a special key as-is.
		if (copy)
		{
		    if (dst + 3 > dest + destlen)
		    {
			iemsg("vim_regsub_both(): not enough space");
			return 0;
		    }
		    *dst++ = c;
		    *dst++ = *src++;
		    *dst++ = *src++;
		}
		else
		{
		    dst += 3;
		    src += 2;
		}
		continue;
	    }

	    if (c == '\\' && *src != NUL)
	    {
		// Check for abbreviations -- webb
		switch (*src)
		{
		    case 'r':	c = CAR;	++src;	break;
		    case 'n':	c = NL;		++src;	break;
		    case 't':	c = TAB;	++src;	break;
		 // Oh no!  \e already has meaning in subst pat :-(
		 // case 'e':   c = ESC;	++src;	break;
		    case 'b':	c = Ctrl_H;	++src;	break;

		    // If "backslash" is TRUE the backslash will be removed
		    // later.  Used to insert a literal CR.
		    default:	if (flags & REGSUB_BACKSLASH)
				{
				    if (copy)
				    {
					if (dst + 1 > dest + destlen)
					{
					    iemsg("vim_regsub_both(): not enough space");
					    return 0;
					}
					*dst = '\\';
				    }
				    ++dst;
				}
				c = *src++;
		}
	    }
	    else if (has_mbyte)
		c = mb_ptr2char(src - 1);

	    // Write to buffer, if copy is set.
	    if (func_one != (fptr_T)NULL)
		// Turbo C complains without the typecast
		func_one = (fptr_T)(func_one(&cc, c));
	    else if (func_all != (fptr_T)NULL)
		// Turbo C complains without the typecast
		func_all = (fptr_T)(func_all(&cc, c));
	    else // just copy
		cc = c;

	    if (has_mbyte)
	    {
		int totlen = mb_ptr2len(src - 1);
		int charlen = mb_char2len(cc);

		if (copy)
		{
		    if (dst + charlen > dest + destlen)
		    {
			iemsg("vim_regsub_both(): not enough space");
			return 0;
		    }
		    mb_char2bytes(cc, dst);
		}
		dst += charlen - 1;
		if (enc_utf8)
		{
		    int clen = utf_ptr2len(src - 1);

		    // If the character length is shorter than "totlen", there
		    // are composing characters; copy them as-is.
		    if (clen < totlen)
		    {
			if (copy)
			{
			    if (dst + totlen - clen > dest + destlen)
			    {
				iemsg("vim_regsub_both(): not enough space");
				return 0;
			    }
			    mch_memmove(dst + 1, src - 1 + clen,
						     (size_t)(totlen - clen));
			}
			dst += totlen - clen;
		    }
		}
		src += totlen - 1;
	    }
	    else if (copy)
	    {
		if (dst + 1 > dest + destlen)
		{
		    iemsg("vim_regsub_both(): not enough space");
		    return 0;
		}
		*dst = cc;
	    }
	    dst++;
	}
	else
	{
	    if (REG_MULTI)
	    {
		clnum = rex.reg_mmatch->startpos[no].lnum;
		if (clnum < 0 || rex.reg_mmatch->endpos[no].lnum < 0)
		    s = NULL;
		else
		{
		    s = reg_getline(clnum) + rex.reg_mmatch->startpos[no].col;
		    if (rex.reg_mmatch->endpos[no].lnum == clnum)
			len = rex.reg_mmatch->endpos[no].col
					    - rex.reg_mmatch->startpos[no].col;
		    else
			len = (int)STRLEN(s);
		}
	    }
	    else
	    {
		s = rex.reg_match->startp[no];
		if (rex.reg_match->endp[no] == NULL)
		    s = NULL;
		else
		    len = (int)(rex.reg_match->endp[no] - s);
	    }
	    if (s != NULL)
	    {
		for (;;)
		{
		    if (len == 0)
		    {
			if (REG_MULTI)
			{
			    if (rex.reg_mmatch->endpos[no].lnum == clnum)
				break;
			    if (copy)
			    {
				if (dst + 1 > dest + destlen)
				{
				    iemsg("vim_regsub_both(): not enough space");
				    return 0;
				}
				*dst = CAR;
			    }
			    ++dst;
			    s = reg_getline(++clnum);
			    if (rex.reg_mmatch->endpos[no].lnum == clnum)
				len = rex.reg_mmatch->endpos[no].col;
			    else
				len = (int)STRLEN(s);
			}
			else
			    break;
		    }
		    else if (*s == NUL) // we hit NUL.
		    {
			if (copy)
			    iemsg(_(e_damaged_match_string));
			goto exit;
		    }
		    else
		    {
			if ((flags & REGSUB_BACKSLASH)
						  && (*s == CAR || *s == '\\'))
			{
			    /*
			     * Insert a backslash in front of a CR, otherwise
			     * it will be replaced by a line break.
			     * Number of backslashes will be halved later,
			     * double them here.
			     */
			    if (copy)
			    {
				if (dst + 2 > dest + destlen)
				{
				    iemsg("vim_regsub_both(): not enough space");
				    return 0;
				}
				dst[0] = '\\';
				dst[1] = *s;
			    }
			    dst += 2;
			}
			else
			{
			    if (has_mbyte)
				c = mb_ptr2char(s);
			    else
				c = *s;

			    if (func_one != (fptr_T)NULL)
				// Turbo C complains without the typecast
				func_one = (fptr_T)(func_one(&cc, c));
			    else if (func_all != (fptr_T)NULL)
				// Turbo C complains without the typecast
				func_all = (fptr_T)(func_all(&cc, c));
			    else // just copy
				cc = c;

			    if (has_mbyte)
			    {
				int l;
				int charlen;

				// Copy composing characters separately, one
				// at a time.
				if (enc_utf8)
				    l = utf_ptr2len(s) - 1;
				else
				    l = mb_ptr2len(s) - 1;

				s += l;
				len -= l;
				charlen = mb_char2len(cc);
				if (copy)
				{
				    if (dst + charlen > dest + destlen)
				    {
					iemsg("vim_regsub_both(): not enough space");
					return 0;
				    }
				    mb_char2bytes(cc, dst);
				}
				dst += charlen - 1;
			    }
			    else if (copy)
			    {
				if (dst + 1 > dest + destlen)
				{
				    iemsg("vim_regsub_both(): not enough space");
				    return 0;
				}
				*dst = cc;
			    }
			    dst++;
			}

			++s;
			--len;
		    }
		}
	    }
	    no = -1;
	}
      }
    if (copy)
	*dst = NUL;

exit:
    return (int)((dst - dest) + 1);
}