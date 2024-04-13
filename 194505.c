get_id_from_attrs(PInfo pi, Attr a) {
    for (; 0 != a->name; a++) {
	if ('i' == *a->name && '\0' == *(a->name + 1)) {
	    unsigned long	id = 0;
	    const char		*text = a->value;
	    char		c;
	    
	    for (; '\0' != *text; text++) {
		c = *text;
		if ('0' <= c && c <= '9') {
		    id = id * 10 + (c - '0');
		} else {
		    set_error(&pi->err, "bad number format", pi->str, pi->s);
		    return 0;
		}
	    }
	    return id;
	}
    }
    return 0;
}