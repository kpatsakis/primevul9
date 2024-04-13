resolve_classname(VALUE mod, const char *class_name, Effort effort, VALUE base_class) {
    VALUE	clas;
    ID		ci = rb_intern(class_name);

    switch (effort) {
    case TolerantEffort:
	if (rb_const_defined_at(mod, ci)) {
	    clas = rb_const_get_at(mod, ci);
	} else {
	    clas = Qundef;
	}
	break;
    case AutoEffort:
	if (rb_const_defined_at(mod, ci)) {
	    clas = rb_const_get_at(mod, ci);
	} else {
	    clas = rb_define_class_under(mod, class_name, base_class);
	}
	break;
    case StrictEffort:
    default:
	/* raise an error if name is not defined */
	clas = rb_const_get_at(mod, ci);
	break;
    }
    return clas;
}