option_expand(int opt_idx, char_u *val)
{
    /* if option doesn't need expansion nothing to do */
    if (!(options[opt_idx].flags & P_EXPAND) || options[opt_idx].var == NULL)
	return NULL;

    /* If val is longer than MAXPATHL no meaningful expansion can be done,
     * expand_env() would truncate the string. */
    if (val != NULL && STRLEN(val) > MAXPATHL)
	return NULL;

    if (val == NULL)
	val = *(char_u **)options[opt_idx].var;

    /*
     * Expanding this with NameBuff, expand_env() must not be passed IObuff.
     * Escape spaces when expanding 'tags', they are used to separate file
     * names.
     * For 'spellsuggest' expand after "file:".
     */
    expand_env_esc(val, NameBuff, MAXPATHL,
	    (char_u **)options[opt_idx].var == &p_tags, FALSE,
#ifdef FEAT_SPELL
	    (char_u **)options[opt_idx].var == &p_sps ? (char_u *)"file:" :
#endif
				  NULL);
    if (STRCMP(NameBuff, val) == 0)   /* they are the same */
	return NULL;

    return NameBuff;
}