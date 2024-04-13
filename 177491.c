parse_block(struct buf *ob, struct sd_markdown *rndr, uint8_t *data, size_t size)
{
	size_t beg, end, i;
	uint8_t *txt_data;
	beg = 0;

	if (rndr->work_bufs[BUFFER_SPAN].size +
		rndr->work_bufs[BUFFER_BLOCK].size > rndr->max_nesting)
		return;

	while (beg < size) {
		txt_data = data + beg;
		end = size - beg;

		if (is_atxheader(rndr, txt_data, end))
			beg += parse_atxheader(ob, rndr, txt_data, end);

		else if (data[beg] == '<' && rndr->cb.blockhtml &&
				(i = parse_htmlblock(ob, rndr, txt_data, end, 1)) != 0)
			beg += i;

		else if ((i = is_empty(txt_data, end)) != 0)
			beg += i;

		else if (is_hrule(txt_data, end)) {
			if (rndr->cb.hrule)
				rndr->cb.hrule(ob, rndr->opaque);

			while (beg < size && data[beg] != '\n')
				beg++;

			beg++;
		}

		else if ((rndr->ext_flags & MKDEXT_FENCED_CODE) != 0 &&
			(i = parse_fencedcode(ob, rndr, txt_data, end)) != 0)
			beg += i;

		else if ((rndr->ext_flags & MKDEXT_TABLES) != 0 &&
			(i = parse_table(ob, rndr, txt_data, end)) != 0)
			beg += i;

		else if (prefix_quote(txt_data, end))
			beg += parse_blockquote(ob, rndr, txt_data, end);

		else if (!(rndr->ext_flags & MKDEXT_DISABLE_INDENTED_CODE) && prefix_code(txt_data, end))
			beg += parse_blockcode(ob, rndr, txt_data, end);

		else if (prefix_uli(txt_data, end))
			beg += parse_list(ob, rndr, txt_data, end, 0);

		else if (prefix_oli(txt_data, end))
			beg += parse_list(ob, rndr, txt_data, end, MKD_LIST_ORDERED);

		else
			beg += parse_paragraph(ob, rndr, txt_data, end);
	}
}