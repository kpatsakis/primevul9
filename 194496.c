instruct(PInfo pi, const char *target, Attr attrs, const char *content) {
    if (0 == strcmp("xml", target)) {
#if HAS_ENCODING_SUPPORT
	for (; 0 != attrs->name; attrs++) {
	    if (0 == strcmp("encoding", attrs->name)) {
		pi->options->rb_enc = rb_enc_find(attrs->value);
	    }
	}
#elif HAS_PRIVATE_ENCODING
	for (; 0 != attrs->name; attrs++) {
	    if (0 == strcmp("encoding", attrs->name)) {
		pi->options->rb_enc = rb_str_new2(attrs->value);
	    }
	}
#endif
    }
}