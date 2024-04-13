get_var_sym_from_attrs(Attr a, void *encoding) {
    for (; 0 != a->name; a++) {
	if ('a' == *a->name && '\0' == *(a->name + 1)) {
	    return name2var(a->value, encoding);
	}
    }
    return Qundef;
}