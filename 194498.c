parse_ulong(const char *s, PInfo pi) {
    unsigned long	n = 0;

    for (; '\0' != *s; s++) {
	if ('0' <= *s && *s <= '9') {
	    n = n * 10 + (*s - '0');
	} else {
	    set_error(&pi->err, "Invalid number for a julian day", pi->str, pi->s);
	    return Qundef;
	}
    }
    return ULONG2NUM(n);
}