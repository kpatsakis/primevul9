has_magic(const char *p, const char *pend, int flags, rb_encoding *enc)
{
    const int escape = !(flags & FNM_NOESCAPE);
    int hasalpha = 0;
    int hasmagical = 0;

    register char c;

    while (p < pend && (c = *p++) != 0) {
	switch (c) {
	  case '{':
	    return BRACE;

	  case '*':
	  case '?':
	  case '[':
	    hasmagical = 1;
	    break;

	  case '\\':
	    if (escape && p++ >= pend)
		continue;
	    break;

#ifdef _WIN32
	  case '.':
	    break;

	  case '~':
	    hasalpha = 1;
	    break;
#endif
	  default:
	    if (IS_WIN32 || ISALPHA(c)) {
		hasalpha = 1;
	    }
	    break;
	}

	p = Next(p-1, pend, enc);
    }

    return hasmagical ? MAGICAL : hasalpha ? ALPHA : PLAIN;
}