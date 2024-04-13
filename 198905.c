static void append_filter_err(struct trace_array *tr,
			      struct filter_parse_error *pe,
			      struct event_filter *filter)
{
	struct trace_seq *s;
	int pos = pe->lasterr_pos;
	char *buf;
	int len;

	if (WARN_ON(!filter->filter_string))
		return;

	s = kmalloc(sizeof(*s), GFP_KERNEL);
	if (!s)
		return;
	trace_seq_init(s);

	len = strlen(filter->filter_string);
	if (pos > len)
		pos = len;

	/* indexing is off by one */
	if (pos)
		pos++;

	trace_seq_puts(s, filter->filter_string);
	if (pe->lasterr > 0) {
		trace_seq_printf(s, "\n%*s", pos, "^");
		trace_seq_printf(s, "\nparse_error: %s\n", err_text[pe->lasterr]);
		tracing_log_err(tr, "event filter parse error",
				filter->filter_string, err_text,
				pe->lasterr, pe->lasterr_pos);
	} else {
		trace_seq_printf(s, "\nError: (%d)\n", pe->lasterr);
		tracing_log_err(tr, "event filter parse error",
				filter->filter_string, err_text,
				FILT_ERR_ERRNO, 0);
	}
	trace_seq_putc(s, 0);
	buf = kmemdup_nul(s->buffer, s->seq.len, GFP_KERNEL);
	if (buf) {
		kfree(filter->filter_string);
		filter->filter_string = buf;
	}
	kfree(s);
}