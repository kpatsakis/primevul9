parse_keyword_arg_i(VALUE key, VALUE value, VALUE self)
{
    if (key == ID2SYM(rb_intern("name"))) {
	rb_iv_set(self, "@name", value);
    } else {
	rb_raise(rb_eArgError, "unknown keyword: %"PRIsVALUE,
		 RB_OBJ_STRING(key));
    }
    return ST_CONTINUE;
}