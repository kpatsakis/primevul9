input_top_bit_set(struct input_ctx *ictx)
{
	struct screen_write_ctx	*sctx = &ictx->ctx;
	struct utf8_data	*ud = &ictx->utf8data;

	ictx->last = -1;

	if (!ictx->utf8started) {
		if (utf8_open(ud, ictx->ch) != UTF8_MORE)
			return (0);
		ictx->utf8started = 1;
		return (0);
	}

	switch (utf8_append(ud, ictx->ch)) {
	case UTF8_MORE:
		return (0);
	case UTF8_ERROR:
		ictx->utf8started = 0;
		return (0);
	case UTF8_DONE:
		break;
	}
	ictx->utf8started = 0;

	log_debug("%s %hhu '%*s' (width %hhu)", __func__, ud->size,
	    (int)ud->size, ud->data, ud->width);

	utf8_copy(&ictx->cell.cell.data, ud);
	screen_write_collect_add(sctx, &ictx->cell.cell);

	return (0);
}