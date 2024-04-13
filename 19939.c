spell_add_word(
    char_u	*word,
    int		len,
    int		what,	    // SPELL_ADD_ values
    int		idx,	    // "zG" and "zW": zero, otherwise index in
			    // 'spellfile'
    int		undo)	    // TRUE for "zug", "zuG", "zuw" and "zuW"
{
    FILE	*fd = NULL;
    buf_T	*buf = NULL;
    int		new_spf = FALSE;
    char_u	*fname;
    char_u	*fnamebuf = NULL;
    char_u	line[MAXWLEN * 2];
    long	fpos, fpos_next = 0;
    int		i;
    char_u	*spf;

    if (!valid_spell_word(word, word + len))
    {
	emsg(_(e_illegal_character_in_word));
	return;
    }

    if (idx == 0)	    // use internal wordlist
    {
	if (int_wordlist == NULL)
	{
	    int_wordlist = vim_tempname('s', FALSE);
	    if (int_wordlist == NULL)
		return;
	}
	fname = int_wordlist;
    }
    else
    {
	// If 'spellfile' isn't set figure out a good default value.
	if (*curwin->w_s->b_p_spf == NUL)
	{
	    init_spellfile();
	    new_spf = TRUE;
	}

	if (*curwin->w_s->b_p_spf == NUL)
	{
	    semsg(_(e_option_str_is_not_set), "spellfile");
	    return;
	}
	fnamebuf = alloc(MAXPATHL);
	if (fnamebuf == NULL)
	    return;

	for (spf = curwin->w_s->b_p_spf, i = 1; *spf != NUL; ++i)
	{
	    copy_option_part(&spf, fnamebuf, MAXPATHL, ",");
	    if (i == idx)
		break;
	    if (*spf == NUL)
	    {
		semsg(_(e_spellfile_does_not_have_nr_entries), idx);
		vim_free(fnamebuf);
		return;
	    }
	}

	// Check that the user isn't editing the .add file somewhere.
	buf = buflist_findname_exp(fnamebuf);
	if (buf != NULL && buf->b_ml.ml_mfp == NULL)
	    buf = NULL;
	if (buf != NULL && bufIsChanged(buf))
	{
	    emsg(_(e_file_is_loaded_in_another_buffer));
	    vim_free(fnamebuf);
	    return;
	}

	fname = fnamebuf;
    }

    if (what == SPELL_ADD_BAD || undo)
    {
	// When the word appears as good word we need to remove that one,
	// since its flags sort before the one with WF_BANNED.
	fd = mch_fopen((char *)fname, "r");
	if (fd != NULL)
	{
	    while (!vim_fgets(line, MAXWLEN * 2, fd))
	    {
		fpos = fpos_next;
		fpos_next = ftell(fd);
		if (fpos_next < 0)
		    break;  // should never happen
		if (STRNCMP(word, line, len) == 0
			&& (line[len] == '/' || line[len] < ' '))
		{
		    // Found duplicate word.  Remove it by writing a '#' at
		    // the start of the line.  Mixing reading and writing
		    // doesn't work for all systems, close the file first.
		    fclose(fd);
		    fd = mch_fopen((char *)fname, "r+");
		    if (fd == NULL)
			break;
		    if (fseek(fd, fpos, SEEK_SET) == 0)
		    {
			fputc('#', fd);
			if (undo)
			{
			    home_replace(NULL, fname, NameBuff, MAXPATHL, TRUE);
			    smsg(_("Word '%.*s' removed from %s"),
							 len, word, NameBuff);
			}
		    }
		    if (fseek(fd, fpos_next, SEEK_SET) != 0)
		    {
			PERROR(_("Seek error in spellfile"));
			break;
		    }
		}
	    }
	    if (fd != NULL)
		fclose(fd);
	}
    }

    if (!undo)
    {
	fd = mch_fopen((char *)fname, "a");
	if (fd == NULL && new_spf)
	{
	    char_u *p;

	    // We just initialized the 'spellfile' option and can't open the
	    // file.  We may need to create the "spell" directory first.  We
	    // already checked the runtime directory is writable in
	    // init_spellfile().
	    if (!dir_of_file_exists(fname) && (p = gettail_sep(fname)) != fname)
	    {
		int c = *p;

		// The directory doesn't exist.  Try creating it and opening
		// the file again.
		*p = NUL;
		vim_mkdir(fname, 0755);
		*p = c;
		fd = mch_fopen((char *)fname, "a");
	    }
	}

	if (fd == NULL)
	    semsg(_(e_cant_open_file_str), fname);
	else
	{
	    if (what == SPELL_ADD_BAD)
		fprintf(fd, "%.*s/!\n", len, word);
	    else if (what == SPELL_ADD_RARE)
		fprintf(fd, "%.*s/?\n", len, word);
	    else
		fprintf(fd, "%.*s\n", len, word);
	    fclose(fd);

	    home_replace(NULL, fname, NameBuff, MAXPATHL, TRUE);
	    smsg(_("Word '%.*s' added to %s"), len, word, NameBuff);
	}
    }

    if (fd != NULL)
    {
	// Update the .add.spl file.
	mkspell(1, &fname, FALSE, TRUE, TRUE);

	// If the .add file is edited somewhere, reload it.
	if (buf != NULL)
	    buf_reload(buf, buf->b_orig_mode, FALSE);

	redraw_all_later(UPD_SOME_VALID);
    }
    vim_free(fnamebuf);
}