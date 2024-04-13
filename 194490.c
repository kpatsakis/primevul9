add_element(PInfo pi, const char *ename, Attr attrs, int hasChildren) {
    Attr		a;
    Helper		h;
    unsigned long	id;

    if (TRACE <= pi->options->trace) {
	char	buf[1024];
	char	indent[128];
	char	*s = buf;
	char	*end = buf + sizeof(buf) - 2;

	s += snprintf(s, end - s, " <%s%s", (hasChildren) ? "" : "/", ename);
	for (a = attrs; 0 != a->name; a++) {
	    s += snprintf(s, end - s, " %s=%s", a->name, a->value);
	}
	*s++ = '>';
	*s++ = '\0';
	if (DEBUG <= pi->options->trace) {
	    printf("===== add element stack(%d) =====\n", helper_stack_depth(&pi->helpers));
	    debug_stack(pi, buf);
	} else {
	    fill_indent(pi, indent, sizeof(indent));
	    printf("%s%s\n", indent, buf);
	}
    }
    if (helper_stack_empty(&pi->helpers)) { /* top level object */
	if (0 != (id = get_id_from_attrs(pi, attrs))) {
	    pi->circ_array = circ_array_new();
	}
    }
    if ('\0' != ename[1]) {
	set_error(&pi->err, "Invalid element name", pi->str, pi->s);
	return;
    }
    h = helper_stack_push(&pi->helpers, get_var_sym_from_attrs(attrs, (void*)pi->options->rb_enc), Qundef, *ename);
    switch (h->type) {
    case NilClassCode:
	h->obj = Qnil;
	break;
    case TrueClassCode:
	h->obj = Qtrue;
	break;
    case FalseClassCode:
	h->obj = Qfalse;
	break;
    case StringCode:
	/* h->obj will be replaced by add_text if it is called */
	h->obj = ox_empty_string;
	if (0 != pi->circ_array) {
	    pi->id = get_id_from_attrs(pi, attrs);
	    circ_array_set(pi->circ_array, h->obj, pi->id);
	}
	break;
    case FixnumCode:
    case FloatCode:
    case SymbolCode:
    case Symbol64Code:
    case RegexpCode:
    case BignumCode:
    case BigDecimalCode:
    case ComplexCode:
    case DateCode:
    case TimeCode:
    case RationalCode: /* sub elements read next */
	/* value will be read in the following add_text */
	h->obj = Qundef;
	break;
    case String64Code:
	h->obj = Qundef;
	if (0 != pi->circ_array) {
	    pi->id = get_id_from_attrs(pi, attrs);
	}
	break;
    case ArrayCode:
	h->obj = rb_ary_new();
	if (0 != pi->circ_array) {
	    circ_array_set(pi->circ_array, h->obj, get_id_from_attrs(pi, attrs));
	}
	break;
    case HashCode:
	h->obj = rb_hash_new();
	if (0 != pi->circ_array) {
	    circ_array_set(pi->circ_array, h->obj, get_id_from_attrs(pi, attrs));
	}
	break;
    case RangeCode:
	h->obj = rb_range_new(ox_zero_fixnum, ox_zero_fixnum, Qfalse);
	break;
    case RawCode:
	if (hasChildren) {
	    h->obj = ox_parse(pi->s, ox_gen_callbacks, &pi->s, pi->options, &pi->err);
	    if (0 != pi->circ_array) {
		circ_array_set(pi->circ_array, h->obj, get_id_from_attrs(pi, attrs));
	    }
	} else {
	    h->obj = Qnil;
	}
	break;
    case ExceptionCode:
	if (Qundef == (h->obj = get_obj_from_attrs(attrs, pi, rb_eException))) {
	    return;
	}
	if (0 != pi->circ_array && Qnil != h->obj) {
	    circ_array_set(pi->circ_array, h->obj, get_id_from_attrs(pi, attrs));
	}
	break;
    case ObjectCode:
	if (Qundef == (h->obj = get_obj_from_attrs(attrs, pi, ox_bag_clas))) {
	    return;
	}
	if (0 != pi->circ_array && Qnil != h->obj) {
	    circ_array_set(pi->circ_array, h->obj, get_id_from_attrs(pi, attrs));
	}
	break;
    case StructCode:
#if HAS_RSTRUCT
	h->obj = get_struct_from_attrs(attrs);
	if (0 != pi->circ_array) {
	    circ_array_set(pi->circ_array, h->obj, get_id_from_attrs(pi, attrs));
	}
#else
	set_error(&pi->err, "Ruby structs not supported with this verion of Ruby", pi->str, pi->s);
	return;
#endif
	break;
    case ClassCode:
	if (Qundef == (h->obj = get_class_from_attrs(attrs, pi, ox_bag_clas))) {
	    return;
	}
	break;
    case RefCode:
	h->obj = Qundef;
	if (0 != pi->circ_array) {
	    h->obj = circ_array_get(pi->circ_array, get_id_from_attrs(pi, attrs));
	}
	if (Qundef == h->obj) {
	    set_error(&pi->err, "Invalid circular reference", pi->str, pi->s);
	    return;
	}
	break;
    default:
	set_error(&pi->err, "Invalid element name", pi->str, pi->s);
	return;
	break;
    }
    if (DEBUG <= pi->options->trace) {
	debug_stack(pi, "   -----------");
    }
}