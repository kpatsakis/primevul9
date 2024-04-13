structname2obj(const char *name) {
    VALUE	ost;
    const char	*s = name;

    for (; 1; s++) {
	if ('\0' == *s) {
	    s = name;
	    break;
	} else if (':' == *s) {
	    s += 2;
	    break;
	}
    }
    ost = rb_const_get(ox_struct_class, rb_intern(s));
    /* use encoding as the indicator for Ruby 1.8.7 or 1.9.x */
#if HAS_ENCODING_SUPPORT
    return rb_struct_alloc_noinit(ost);
#elif HAS_PRIVATE_ENCODING
    return rb_struct_alloc_noinit(ost);
#else
    return rb_struct_new(ost);
#endif
}