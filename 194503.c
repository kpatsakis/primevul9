name2var(const char *name, void *encoding) {
    VALUE	*slot;
    ID		var_id;

    if ('0' <= *name && *name <= '9') {
	var_id = INT2NUM(atoi(name));
    } else if (Qundef == (var_id = ox_cache_get(ox_attr_cache, name, &slot, 0))) {
#ifdef HAVE_RUBY_ENCODING_H
	if (0 != encoding) {
	    volatile VALUE	rstr = rb_str_new2(name);
	    volatile VALUE	sym;
	    
	    rb_enc_associate(rstr, (rb_encoding*)encoding);
	    sym = rb_funcall(rstr, ox_to_sym_id, 0);
	    // Needed for Ruby 2.2 to get around the GC of symbols
	    // created with to_sym which is needed for encoded symbols.
	    rb_ary_push(ox_sym_bank, sym);
	    var_id = SYM2ID(sym);
	} else {
	    var_id = rb_intern(name);
	}
#else
	var_id = rb_intern(name);
#endif
	*slot = var_id;
    }
    return var_id;
}