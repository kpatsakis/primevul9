find_dirsep(const char *p, const char *pend, int flags, rb_encoding *enc)
{
    const int escape = !(flags & FNM_NOESCAPE);

    register char c;
    int open = 0;

    while ((c = *p++) != 0) {
	switch (c) {
	  case '[':
	    open = 1;
	    continue;
	  case ']':
	    open = 0;
	    continue;

	  case '{':
	    open = 1;
	    continue;
	  case '}':
	    open = 0;
	    continue;

	  case '/':
	    if (!open)
		return (char *)p-1;
	    continue;

	  case '\\':
	    if (escape && !(c = *p++))
		return (char *)p-1;
	    continue;
	}

	p = Next(p-1, pend, enc);
    }

    return (char *)p-1;
}