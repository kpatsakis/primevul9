get_class_from_attrs(Attr a, PInfo pi, VALUE base_class) {
    for (; 0 != a->name; a++) {
	if ('c' == *a->name && '\0' == *(a->name + 1)) {
	    return classname2class(a->value, pi, base_class);
	}
    }
    return Qundef;
}