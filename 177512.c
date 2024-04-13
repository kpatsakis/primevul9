char_autolink_www(struct buf *ob, struct sd_markdown *rndr, uint8_t *data, size_t offset, size_t size)
{
	struct buf *link, *link_url, *link_text;
	size_t link_len, rewind;

	if (!rndr->cb.link || rndr->in_link_body)
		return 0;

	link = rndr_newbuf(rndr, BUFFER_SPAN);

	if ((link_len = sd_autolink__www(&rewind, link, data, offset, size, 0)) > 0) {
		link_url = rndr_newbuf(rndr, BUFFER_SPAN);
		BUFPUTSL(link_url, "http://");
		bufput(link_url, link->data, link->size);

		ob->size -= rewind;
		if (rndr->cb.normal_text) {
			link_text = rndr_newbuf(rndr, BUFFER_SPAN);
			rndr->cb.normal_text(link_text, link, rndr->opaque);
			rndr->cb.link(ob, link_url, NULL, link_text, rndr->opaque);
			rndr_popbuf(rndr, BUFFER_SPAN);
		} else {
			rndr->cb.link(ob, link_url, NULL, link, rndr->opaque);
		}
		rndr_popbuf(rndr, BUFFER_SPAN);
	}

	rndr_popbuf(rndr, BUFFER_SPAN);
	return link_len;
}