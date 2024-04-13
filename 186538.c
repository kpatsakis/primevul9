startPS(linenr_T lnum, int para, int both)
{
    char_u	*s;

    s = ml_get(lnum);
    if (*s == para || *s == '\f' || (both && *s == '}'))
	return TRUE;
    if (*s == '.' && (inmacro(p_sections, s + 1) ||
					   (!para && inmacro(p_para, s + 1))))
	return TRUE;
    return FALSE;
}