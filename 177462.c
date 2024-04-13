parse_blockquote(struct buf *ob, struct sd_markdown *rndr, uint8_t *data, size_t size)
{
	size_t beg, end = 0, pre, work_size = 0;
	uint8_t *work_data = 0;
	struct buf *out = 0;

	out = rndr_newbuf(rndr, BUFFER_BLOCK);
	beg = 0;
	while (beg < size) {
		for (end = beg + 1; end < size && data[end - 1] != '\n'; end++);

		pre = prefix_quote(data + beg, end - beg);

		if (pre)
			beg += pre; /* skipping prefix */

		/* empty line followed by non-quote line */
		else if (is_empty(data + beg, end - beg) &&
				(end >= size || (prefix_quote(data + end, size - end) == 0 &&
				!is_empty(data + end, size - end))))
			break;

		if (beg < end) { /* copy into the in-place working buffer */
			/* bufput(work, data + beg, end - beg); */
			if (!work_data)
				work_data = data + beg;
			else if (data + beg != work_data + work_size)
				memmove(work_data + work_size, data + beg, end - beg);
			work_size += end - beg;
		}
		beg = end;
	}

	parse_block(out, rndr, work_data, work_size);
	if (rndr->cb.blockquote)
		rndr->cb.blockquote(ob, out, rndr->opaque);
	rndr_popbuf(rndr, BUFFER_BLOCK);
	return end;
}