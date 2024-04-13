sd_markdown_render(struct buf *ob, const uint8_t *document, size_t doc_size, struct sd_markdown *md)
{
#define MARKDOWN_GROW(x) ((x) + ((x) >> 1))
	static const char UTF8_BOM[] = {0xEF, 0xBB, 0xBF};

	struct buf *text;
	size_t beg, end;

	text = bufnew(64);
	if (!text)
		return;

	/* Preallocate enough space for our buffer to avoid expanding while copying */
	bufgrow(text, doc_size);

	/* reset the references table */
	memset(&md->refs, 0x0, REF_TABLE_SIZE * sizeof(void *));

	int footnotes_enabled = md->ext_flags & MKDEXT_FOOTNOTES;

	/* reset the footnotes lists */
	if (footnotes_enabled) {
		memset(&md->footnotes_found, 0x0, sizeof(md->footnotes_found));
		memset(&md->footnotes_used, 0x0, sizeof(md->footnotes_used));
	}

	/* first pass: looking for references, copying everything else */
	beg = 0;

	/* Skip a possible UTF-8 BOM, even though the Unicode standard
	 * discourages having these in UTF-8 documents */
	if (doc_size >= 3 && memcmp(document, UTF8_BOM, 3) == 0)
		beg += 3;

	while (beg < doc_size) /* iterating over lines */
		if (footnotes_enabled && is_footnote(document, beg, doc_size, &end, &md->footnotes_found))
			beg = end;
		else if (is_ref(document, beg, doc_size, &end, md->refs))
			beg = end;
		else { /* skipping to the next line */
			end = beg;
			while (end < doc_size && document[end] != '\n' && document[end] != '\r')
				end++;

			/* adding the line body if present */
			if (end > beg)
				expand_tabs(text, document + beg, end - beg);

			while (end < doc_size && (document[end] == '\n' || document[end] == '\r')) {
				/* add one \n per newline */
				if (document[end] == '\n' || (end + 1 < doc_size && document[end + 1] != '\n'))
					bufputc(text, '\n');
				end++;
			}

			beg = end;
		}

	/* pre-grow the output buffer to minimize allocations */
	bufgrow(ob, MARKDOWN_GROW(text->size));

	/* second pass: actual rendering */
	if (md->cb.doc_header)
		md->cb.doc_header(ob, md->opaque);

	if (text->size) {
		/* adding a final newline if not already present */
		if (text->data[text->size - 1] != '\n' &&  text->data[text->size - 1] != '\r')
			bufputc(text, '\n');

		parse_block(ob, md, text->data, text->size);
	}

	/* footnotes */
	if (footnotes_enabled)
		parse_footnote_list(ob, md, &md->footnotes_used);

	if (md->cb.doc_footer)
		md->cb.doc_footer(ob, md->opaque);

	/* clean-up */
	bufrelease(text);
	free_link_refs(md->refs);
	if (footnotes_enabled) {
		free_footnote_list(&md->footnotes_found, 1);
		free_footnote_list(&md->footnotes_used, 0);
	}

	assert(md->work_bufs[BUFFER_SPAN].size == 0);
	assert(md->work_bufs[BUFFER_BLOCK].size == 0);
}