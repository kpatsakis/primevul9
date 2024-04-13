parse_xsd_time(const char *text, VALUE clas) {
    long	cargs[10];
    long	*cp = cargs;
    long	v;
    int		i;
    char	c;
    struct _Tp	tpa[10] = { { 4, '-', '-' },
			   { 2, '-', '-' },
			   { 2, 'T', 'T' },
			   { 2, ':', ':' },
			   { 2, ':', ':' },
			   { 2, '.', '.' },
			   { 9, '+', '-' },
			   { 2, ':', ':' },
			   { 2, '\0', '\0' },
			   { 0, '\0', '\0' } };
    Tp		tp = tpa;
    struct tm	tm;

    for (; 0 != tp->cnt; tp++) {
	for (i = tp->cnt, v = 0; 0 < i ; text++, i--) {
	    c = *text;
	    if (c < '0' || '9' < c) {
		if (tp->end == c || tp->alt == c) {
		    break;
		}
		return Qnil;
	    }
	    v = 10 * v + (long)(c - '0');
	}
	c = *text++;
	if (tp->end != c && tp->alt != c) {
	    return Qnil;
	}
	*cp++ = v;
    }
    tm.tm_year = (int)cargs[0] - 1900;
    tm.tm_mon = (int)cargs[1] - 1;
    tm.tm_mday = (int)cargs[2];
    tm.tm_hour = (int)cargs[3];
    tm.tm_min = (int)cargs[4];
    tm.tm_sec = (int)cargs[5];
#if HAS_NANO_TIME
    return rb_time_nano_new(mktime(&tm), cargs[6]);
#else
    return rb_time_new(mktime(&tm), cargs[6] / 1000);
#endif
}