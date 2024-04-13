add_text(PInfo pi, char *text, int closed) {
    Helper	h = helper_stack_peek(&pi->helpers);

    if (!closed) {
	set_error(&pi->err, "Text not closed", pi->str, pi->s);
	return;
    }
    if (0 == h) {
	set_error(&pi->err, "Unexpected text", pi->str, pi->s);
	return;
    }
    if (DEBUG <= pi->options->trace) {
	char	indent[128];

	fill_indent(pi, indent, sizeof(indent));
	printf("%s '%s' to type %c\n", indent, text, h->type);
    }
    switch (h->type) {
    case NoCode:
    case StringCode:
	h->obj = rb_str_new2(text);
#if HAS_ENCODING_SUPPORT
	if (0 != pi->options->rb_enc) {
	    rb_enc_associate(h->obj, pi->options->rb_enc);
	}
#elif HAS_PRIVATE_ENCODING
	if (Qnil != pi->options->rb_enc) {
	    rb_funcall(h->obj, ox_force_encoding_id, 1, pi->options->rb_enc);
	}
#endif
	if (0 != pi->circ_array) {
	    circ_array_set(pi->circ_array, h->obj, (unsigned long)pi->id);
	}
	break;
    case FixnumCode:
    {
	long	n = 0;
	char	c;
	int	neg = 0;

	if ('-' == *text) {
	    neg = 1;
	    text++;
	}
	for (; '\0' != *text; text++) {
	    c = *text;
	    if ('0' <= c && c <= '9') {
		n = n * 10 + (c - '0');
	    } else {
		set_error(&pi->err, "bad number format", pi->str, pi->s);
		return;
	    }
	}
	if (neg) {
	    n = -n;
	}
	h->obj = LONG2NUM(n);
	break;
    }
    case FloatCode:
	h->obj = rb_float_new(strtod(text, 0));
	break;
    case SymbolCode:
    {
	VALUE	sym;
	VALUE	*slot;

	if (Qundef == (sym = ox_cache_get(ox_symbol_cache, text, &slot, 0))) {
	    sym = str2sym(text, (void*)pi->options->rb_enc);
	    // Needed for Ruby 2.2 to get around the GC of symbols created with
	    // to_sym which is needed for encoded symbols.
	    rb_ary_push(ox_sym_bank, sym);
	    *slot = sym;
	}
	h->obj = sym;
	break;
    }
    case DateCode:
    {
	VALUE	args[1];

	if (Qundef == (*args = parse_ulong(text, pi))) {
	    return;
	}
	h->obj = rb_funcall2(ox_date_class, ox_jd_id, 1, args);
	break;
    }
    case TimeCode:
	h->obj = parse_time(text, ox_time_class);
	break;
    case String64Code:
    {
	unsigned long	str_size = b64_orig_size(text);
	VALUE		v;
	char		*str = ALLOCA_N(char, str_size + 1);
	
	from_base64(text, (uchar*)str);
	v = rb_str_new(str, str_size);
#if HAS_ENCODING_SUPPORT
	if (0 != pi->options->rb_enc) {
	    rb_enc_associate(v, pi->options->rb_enc);
	}
#elif HAS_PRIVATE_ENCODING
	if (0 != pi->options->rb_enc) {
	    rb_funcall(v, ox_force_encoding_id, 1, pi->options->rb_enc);
	}
#endif
	if (0 != pi->circ_array) {
	    circ_array_set(pi->circ_array, v, (unsigned long)h->obj);
	}
	h->obj = v;
	break;
    }
    case Symbol64Code:
    {
	VALUE		sym;
	VALUE		*slot;
	unsigned long	str_size = b64_orig_size(text);
	char		*str = ALLOCA_N(char, str_size + 1);
	
	from_base64(text, (uchar*)str);
	if (Qundef == (sym = ox_cache_get(ox_symbol_cache, str, &slot, 0))) {
	    sym = str2sym(str, (void*)pi->options->rb_enc);
	    // Needed for Ruby 2.2 to get around the GC of symbols created with
	    // to_sym which is needed for encoded symbols.
	    rb_ary_push(ox_sym_bank, sym);
	    *slot = sym;
	}
	h->obj = sym;
	break;
    }
    case RegexpCode:
	if ('/' == *text) {
	    h->obj = parse_regexp(text);
	} else {
	    unsigned long	str_size = b64_orig_size(text);
	    char		*str = ALLOCA_N(char, str_size + 1);
	
	    from_base64(text, (uchar*)str);
	    h->obj = parse_regexp(str);
	}
	break;
    case BignumCode:
	h->obj = rb_cstr_to_inum(text, 10, 1);
	break;
    case BigDecimalCode:
#if HAS_BIGDECIMAL
	h->obj = rb_funcall(ox_bigdecimal_class, ox_new_id, 1, rb_str_new2(text));
#else
	h->obj = Qnil;
#endif
	break;
    default:
	h->obj = Qnil;
	break;
    }
}