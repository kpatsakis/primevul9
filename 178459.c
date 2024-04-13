skip_regexp_err(
    char_u	*startp,
    int		delim,
    int		magic)
{
    char_u *p = skip_regexp(startp, delim, magic);

    if (*p != delim)
    {
	semsg(_(e_missing_delimiter_after_search_pattern_str), startp);
	return NULL;
    }
    return p;
}