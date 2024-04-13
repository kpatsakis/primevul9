classname2class(const char *name, PInfo pi, VALUE base_class) {
    VALUE	*slot;
    VALUE	clas;
	    
    if (Qundef == (clas = ox_cache_get(ox_class_cache, name, &slot, 0))) {
	char		class_name[1024];
	char		*s;
	const char	*n = name;

	clas = rb_cObject;
	for (s = class_name; '\0' != *n; n++) {
	    if (':' == *n) {
		*s = '\0';
		n++;
		if (':' != *n) {
		    set_error(&pi->err, "Invalid classname, expected another ':'", pi->str, pi->s);
		    return Qundef;
		}
		if (Qundef == (clas = resolve_classname(clas, class_name, pi->options->effort, base_class))) {
		    return Qundef;
		}
		s = class_name;
	    } else {
		*s++ = *n;
	    }
	}
	*s = '\0';
	if (Qundef != (clas = resolve_classname(clas, class_name, pi->options->effort, base_class))) {
	    *slot = clas;
	}
    }
    return clas;
}