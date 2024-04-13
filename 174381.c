ber_wrap_hdr_data(int tagval, STREAM in)
{
	STREAM out;
	int size = s_length(in) + 16;

	out = xmalloc(sizeof(struct stream));
	memset(out, 0, sizeof(struct stream));
	out->data = xmalloc(size);
	out->size = size;
	out->p = out->data;

	ber_out_header(out, tagval, s_length(in));
	out_uint8p(out, in->data, s_length(in));
	s_mark_end(out);

	return out;
}