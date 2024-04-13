char_autolink_email(struct buf *ob, struct sd_markdown *rndr, uint8_t *data, size_t offset, size_t size)
{
	struct buf *link;
	size_t link_len, rewind;

	if (!rndr->cb.autolink || rndr->in_link_body)
		return 0;

	link = rndr_newbuf(rndr, BUFFER_SPAN);

	if ((link_len = sd_autolink__email(&rewind, link, data, offset, size, 0)) > 0) {
		ob->size -= rewind;
		rndr->cb.autolink(ob, link, MKDA_EMAIL, rndr->opaque);
	}

	rndr_popbuf(rndr, BUFFER_SPAN);
	return link_len;
}