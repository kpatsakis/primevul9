vim_ispathsep_nocolon(int c)
{
    return vim_ispathsep(c)
#ifdef BACKSLASH_IN_FILENAME
	&& c != ':'
#endif
	;
}