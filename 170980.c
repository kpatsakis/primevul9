int ring_buffer_print_entry_header(struct trace_seq *s)
{
	trace_seq_puts(s, "# compressed entry header\n");
	trace_seq_puts(s, "\ttype_len    :    5 bits\n");
	trace_seq_puts(s, "\ttime_delta  :   27 bits\n");
	trace_seq_puts(s, "\tarray       :   32 bits\n");
	trace_seq_putc(s, '\n');
	trace_seq_printf(s, "\tpadding     : type == %d\n",
			 RINGBUF_TYPE_PADDING);
	trace_seq_printf(s, "\ttime_extend : type == %d\n",
			 RINGBUF_TYPE_TIME_EXTEND);
	trace_seq_printf(s, "\tdata max type_len  == %d\n",
			 RINGBUF_TYPE_DATA_TYPE_LEN_MAX);

	return !trace_seq_has_overflowed(s);
}