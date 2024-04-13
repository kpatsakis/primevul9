parse_list(struct buf *ob, struct sd_markdown *rndr, uint8_t *data, size_t size, int flags)
{
	struct buf *work = 0;
	size_t i = 0, j;

	work = rndr_newbuf(rndr, BUFFER_BLOCK);

	while (i < size) {
		j = parse_listitem(work, rndr, data + i, size - i, &flags);
		i += j;

		if (!j || (flags & MKD_LI_END))
			break;
	}

	if (rndr->cb.list)
		rndr->cb.list(ob, work, flags, rndr->opaque);
	rndr_popbuf(rndr, BUFFER_BLOCK);
	return i;
}