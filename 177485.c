parse_listitem(struct buf *ob, struct sd_markdown *rndr, uint8_t *data, size_t size, int *flags)
{
	struct buf *work = 0, *inter = 0;
	size_t beg = 0, end, pre, sublist = 0, orgpre = 0, i;
	int in_empty = 0, has_inside_empty = 0, in_fence = 0;

	/* keeping track of the first indentation prefix */
	while (orgpre < 3 && orgpre < size && data[orgpre] == ' ')
		orgpre++;

	beg = prefix_uli(data, size);
	if (!beg)
		beg = prefix_oli(data, size);

	if (!beg)
		return 0;

	/* skipping to the beginning of the following line */
	end = beg;
	while (end < size && data[end - 1] != '\n')
		end++;

	/* getting working buffers */
	work = rndr_newbuf(rndr, BUFFER_SPAN);
	inter = rndr_newbuf(rndr, BUFFER_SPAN);

	/* putting the first line into the working buffer */
	bufput(work, data + beg, end - beg);
	beg = end;

	/* process the following lines */
	while (beg < size) {
		size_t has_next_uli = 0, has_next_oli = 0;

		end++;

		while (end < size && data[end - 1] != '\n')
			end++;

		/* process an empty line */
		if (is_empty(data + beg, end - beg)) {
			in_empty = 1;
			beg = end;
			continue;
		}

		/* calculating the indentation */
		i = 0;
		while (i < 4 && beg + i < end && data[beg + i] == ' ')
			i++;

		pre = i;

		if (rndr->ext_flags & MKDEXT_FENCED_CODE) {
			if (is_codefence(data + beg + i, end - beg - i, NULL) != 0)
				in_fence = !in_fence;
		}

		/* Only check for new list items if we are **not** inside
		 * a fenced code block */
		if (!in_fence) {
			has_next_uli = prefix_uli(data + beg + i, end - beg - i);
			has_next_oli = prefix_oli(data + beg + i, end - beg - i);
		}

		/* checking for ul/ol switch */
		if (in_empty && (
			((*flags & MKD_LIST_ORDERED) && has_next_uli) ||
			(!(*flags & MKD_LIST_ORDERED) && has_next_oli))){
			*flags |= MKD_LI_END;
			break; /* the following item must have same list type */
		}

		/* checking for a new item */
		if ((has_next_uli && !is_hrule(data + beg + i, end - beg - i)) || has_next_oli) {
			if (in_empty)
				has_inside_empty = 1;

			if (pre == orgpre) /* the following item must have */
				break;             /* the same indentation */

			if (!sublist)
				sublist = work->size;
		}
		/* joining only indented stuff after empty lines */
		else if (in_empty && i < 4 && data[beg] != '\t') {
			*flags |= MKD_LI_END;
			break;
		}
		else if (in_empty) {
			bufputc(work, '\n');
			has_inside_empty = 1;
		}

		in_empty = 0;

		/* adding the line without prefix into the working buffer */
		bufput(work, data + beg + i, end - beg - i);
		beg = end;
	}

	/* render of li contents */
	if (has_inside_empty)
		*flags |= MKD_LI_BLOCK;

	if (*flags & MKD_LI_BLOCK) {
		/* intermediate render of block li */
		if (sublist && sublist < work->size) {
			parse_block(inter, rndr, work->data, sublist);
			parse_block(inter, rndr, work->data + sublist, work->size - sublist);
		}
		else
			parse_block(inter, rndr, work->data, work->size);
	} else {
		/* intermediate render of inline li */
		if (sublist && sublist < work->size) {
			parse_inline(inter, rndr, work->data, sublist);
			parse_block(inter, rndr, work->data + sublist, work->size - sublist);
		}
		else
			parse_inline(inter, rndr, work->data, work->size);
	}

	/* render of li itself */
	if (rndr->cb.listitem)
		rndr->cb.listitem(ob, inter, *flags, rndr->opaque);

	rndr_popbuf(rndr, BUFFER_SPAN);
	rndr_popbuf(rndr, BUFFER_SPAN);
	return beg;
}