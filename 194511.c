parse_time(const char *text, VALUE clas) {
    VALUE	t;

    if (Qnil == (t = parse_double_time(text, clas)) &&
	Qnil == (t = parse_xsd_time(text, clas))) {
	VALUE	    args[1];

	/*printf("**** time parse\n"); */
	*args = rb_str_new2(text);
	t = rb_funcall2(ox_time_class, ox_parse_id, 1, args);
    }
    return t;
}