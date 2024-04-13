char_autolink_url(struct buf *ob, struct sd_markdown *rndr, uint8_t *data, size_t offset, size_t size)
{
	struct buf *link;
	size_t link_len, rewind;

	if (!rndr->cb.autolink || rndr->in_link_body)
		return 0;

	link = rndr_newbuf(rndr, BUFFER_SPAN);

	if ((link_len = sd_autolink__url(&rewind, link, data, offset, size, SD_AUTOLINK_SHORT_DOMAINS)) > 0) {
		ob->size -= rewind;
		rndr->cb.autolink(ob, link, MKDA_NORMAL, rndr->opaque);
	}

	rndr_popbuf(rndr, BUFFER_SPAN);
	return link_len;
}