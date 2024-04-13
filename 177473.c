char_langle_tag(struct buf *ob, struct sd_markdown *rndr, uint8_t *data, size_t offset, size_t size)
{
	enum mkd_autolink altype = MKDA_NOT_AUTOLINK;
	size_t end = tag_length(data, size, &altype);
	struct buf work = { data, end, 0, 0 };
	int ret = 0;

	if (end > 2) {
		if (rndr->cb.autolink && altype != MKDA_NOT_AUTOLINK) {
			struct buf *u_link = rndr_newbuf(rndr, BUFFER_SPAN);
			work.data = data + 1;
			work.size = end - 2;
			unscape_text(u_link, &work);
			ret = rndr->cb.autolink(ob, u_link, altype, rndr->opaque);
			rndr_popbuf(rndr, BUFFER_SPAN);
		}
		else if (rndr->cb.raw_html_tag)
			ret = rndr->cb.raw_html_tag(ob, &work, rndr->opaque);
	}

	if (!ret) return 0;
	else return end;
}