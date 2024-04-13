rb_push_glob(VALUE str, VALUE base, int flags) /* '\0' is delimiter */
{
    VALUE ary;
    int status;

    /* can contain null bytes as separators */
    if (!RB_TYPE_P(str, T_STRING)) {
	FilePathValue(str);
    }
    else if (!rb_str_to_cstr(str)) {
        rb_raise(rb_eArgError, "nul-separated glob pattern is deprecated");
    }
    else {
	rb_check_safe_obj(str);
	rb_enc_check(str, rb_enc_from_encoding(rb_usascii_encoding()));
    }
    ary = rb_ary_new();

    status = push_glob(ary, str, base, flags);
    if (status) GLOB_JUMP_TAG(status);

    return ary;
}