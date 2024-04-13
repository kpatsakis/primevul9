parse_footnote_def(struct buf *ob, struct sd_markdown *rndr, unsigned int num, uint8_t *data, size_t size)
{
	struct buf *work = 0;
	work = rndr_newbuf(rndr, BUFFER_SPAN);

	parse_block(work, rndr, data, size);

	if (rndr->cb.footnote_def)
	rndr->cb.footnote_def(ob, work, num, rndr->opaque);
	rndr_popbuf(rndr, BUFFER_SPAN);
}