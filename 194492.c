parse_double_time(const char *text, VALUE clas) {
    long	v = 0;
    long	v2 = 0;
    const char	*dot = 0;
    char	c;
    
    for (; '.' != *text; text++) {
	c = *text;
	if (c < '0' || '9' < c) {
	    return Qnil;
	}
	v = 10 * v + (long)(c - '0');
    }
    dot = text++;
    for (; '\0' != *text && text - dot <= 6; text++) {
	c = *text;
	if (c < '0' || '9' < c) {
	    return Qnil;
	}
	v2 = 10 * v2 + (long)(c - '0');
    }
    for (; text - dot <= 9; text++) {
	v2 *= 10;
    }
#if HAS_NANO_TIME
    return rb_time_nano_new(v, v2);
#else
    return rb_time_new(v, v2 / 1000);
#endif
}