parse_footnote_list(struct buf *ob, struct sd_markdown *rndr, struct footnote_list *footnotes)
{
	struct buf *work = 0;
	struct footnote_item *item;
	struct footnote_ref *ref;

	if (footnotes->count == 0)
		return;

	work = rndr_newbuf(rndr, BUFFER_BLOCK);

	item = footnotes->head;
	while (item) {
		ref = item->ref;
		parse_footnote_def(work, rndr, ref->num, ref->contents->data, ref->contents->size);
		item = item->next;
	}

	if (rndr->cb.footnotes)
		rndr->cb.footnotes(ob, work, rndr->opaque);
	rndr_popbuf(rndr, BUFFER_BLOCK);
}