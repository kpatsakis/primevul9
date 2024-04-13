classname2obj(const char *name, PInfo pi, VALUE base_class) {
    VALUE   clas = classname2class(name, pi, base_class);
    
    if (Qundef == clas) {
	return Qnil;
    } else {
	return rb_obj_alloc(clas);
    }
}