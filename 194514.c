get_struct_from_attrs(Attr a) {
    for (; 0 != a->name; a++) {
	if ('c' == *a->name && '\0' == *(a->name + 1)) {
	    return structname2obj(a->value);
	}
    }
    return Qundef;
}