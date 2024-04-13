parse_blockcode(struct buf *ob, struct sd_markdown *rndr, uint8_t *data, size_t size)
{
	size_t beg, end, pre;
	struct buf *work = 0;

	work = rndr_newbuf(rndr, BUFFER_BLOCK);

	beg = 0;
	while (beg < size) {
		for (end = beg + 1; end < size && data[end - 1] != '\n'; end++) {};
		pre = prefix_code(data + beg, end - beg);

		if (pre)
			beg += pre; /* skipping prefix */
		else if (!is_empty(data + beg, end - beg))
			/* non-empty non-prefixed line breaks the pre */
			break;

		if (beg < end) {
			/* verbatim copy to the working buffer,
				escaping entities */
			if (is_empty(data + beg, end - beg))
				bufputc(work, '\n');
			else bufput(work, data + beg, end - beg);
		}
		beg = end;
	}

	while (work->size && work->data[work->size - 1] == '\n')
		work->size -= 1;

	bufputc(work, '\n');

	if (rndr->cb.blockcode)
		rndr->cb.blockcode(ob, work, NULL, rndr->opaque);

	rndr_popbuf(rndr, BUFFER_BLOCK);
	return beg;
}