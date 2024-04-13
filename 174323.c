s_realloc(STREAM s, unsigned int size)
{
	unsigned char *data;

	if (s->size >= size)
		return;

	data = s->data;
	s->size = size;
	s->data = xrealloc(data, size);
	s->p = s->data + (s->p - data);
	s->end = s->data + (s->end - data);
	s->iso_hdr = s->data + (s->iso_hdr - data);
	s->mcs_hdr = s->data + (s->mcs_hdr - data);
	s->sec_hdr = s->data + (s->sec_hdr - data);
	s->rdp_hdr = s->data + (s->rdp_hdr - data);
	s->channel_hdr = s->data + (s->channel_hdr - data);
}