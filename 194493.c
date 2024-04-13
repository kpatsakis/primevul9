str2sym(const char *str, void *encoding) {
    VALUE	sym;
    
#ifdef HAVE_RUBY_ENCODING_H
    if (0 != encoding) {
	VALUE	rstr = rb_str_new2(str);

	rb_enc_associate(rstr, (rb_encoding*)encoding);
	sym = rb_funcall(rstr, ox_to_sym_id, 0);
    } else {
	sym = ID2SYM(rb_intern(str));
    }
#else
    sym = ID2SYM(rb_intern(str));
#endif
    return sym;
}