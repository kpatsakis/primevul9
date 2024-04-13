parse_fencedcode(struct buf *ob, struct sd_markdown *rndr, uint8_t *data, size_t size)
{
	size_t beg, end;
	struct buf *work = 0;
	struct buf lang = { 0, 0, 0, 0 };

	beg = is_codefence(data, size, &lang);
	if (beg == 0) return 0;

	work = rndr_newbuf(rndr, BUFFER_BLOCK);

	while (beg < size) {
		size_t fence_end;
		struct buf fence_trail = { 0, 0, 0, 0 };

		fence_end = is_codefence(data + beg, size - beg, &fence_trail);
		if (fence_end != 0 && fence_trail.size == 0) {
			beg += fence_end;
			break;
		}

		for (end = beg + 1; end < size && data[end - 1] != '\n'; end++);

		if (beg < end) {
			/* verbatim copy to the working buffer,
				escaping entities */
			if (is_empty(data + beg, end - beg))
				bufputc(work, '\n');
			else bufput(work, data + beg, end - beg);
		}
		beg = end;
	}

	if (work->size && work->data[work->size - 1] != '\n')
		bufputc(work, '\n');

	if (rndr->cb.blockcode)
		rndr->cb.blockcode(ob, work, lang.size ? &lang : NULL, rndr->opaque);

	rndr_popbuf(rndr, BUFFER_BLOCK);
	return beg;
}