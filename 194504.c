debug_stack(PInfo pi, const char *comment) {
    char	indent[128];
    Helper	h;

    fill_indent(pi, indent, sizeof(indent));
    printf("%s%s\n", indent, comment);
    if (!helper_stack_empty(&pi->helpers)) {
	for (h = pi->helpers.head; h < pi->helpers.tail; h++) {
	    const char	*clas = "---";
	    const char	*key = "---";

	    if (Qundef != h->obj) {
		VALUE	c =  rb_obj_class(h->obj);

		clas = rb_class2name(c);
	    }
	    if (Qundef != h->var) {
		if (HashCode == h->type) {
		    VALUE	v;
		    
		    v = rb_funcall2(h->var, rb_intern("to_s"), 0, 0);
		    key = StringValuePtr(v);
		} else if (ObjectCode == (h - 1)->type || ExceptionCode == (h - 1)->type || RangeCode == (h - 1)->type || StructCode == (h - 1)->type) {
		    key = rb_id2name(h->var);
		} else {
		    printf("%s*** corrupt stack ***\n", indent);
		}
	    }
	    printf("%s [%c] %s : %s\n", indent, h->type, clas, key);
	}
    }
}