spell_reload_one(
    char_u	*fname,
    int		added_word)	// invoked through "zg"
{
    slang_T	*slang;
    int		didit = FALSE;

    FOR_ALL_SPELL_LANGS(slang)
    {
	if (fullpathcmp(fname, slang->sl_fname, FALSE, TRUE) == FPC_SAME)
	{
	    slang_clear(slang);
	    if (spell_load_file(fname, NULL, slang, FALSE) == NULL)
		// reloading failed, clear the language
		slang_clear(slang);
	    redraw_all_later(UPD_SOME_VALID);
	    didit = TRUE;
	}
    }

    // When "zg" was used and the file wasn't loaded yet, should redo
    // 'spelllang' to load it now.
    if (added_word && !didit)
	did_set_spelllang(curwin);
}