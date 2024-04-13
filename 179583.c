input_esc_dispatch(struct input_ctx *ictx)
{
	struct screen_write_ctx		*sctx = &ictx->ctx;
	struct screen			*s = sctx->s;
	struct input_table_entry	*entry;

	if (ictx->flags & INPUT_DISCARD)
		return (0);
	log_debug("%s: '%c', %s", __func__, ictx->ch, ictx->interm_buf);

	entry = bsearch(ictx, input_esc_table, nitems(input_esc_table),
	    sizeof input_esc_table[0], input_table_compare);
	if (entry == NULL) {
		log_debug("%s: unknown '%c'", __func__, ictx->ch);
		return (0);
	}

	switch (entry->type) {
	case INPUT_ESC_RIS:
		input_reset_cell(ictx);
		screen_write_reset(sctx);
		break;
	case INPUT_ESC_IND:
		screen_write_linefeed(sctx, 0);
		break;
	case INPUT_ESC_NEL:
		screen_write_carriagereturn(sctx);
		screen_write_linefeed(sctx, 0);
		break;
	case INPUT_ESC_HTS:
		if (s->cx < screen_size_x(s))
			bit_set(s->tabs, s->cx);
		break;
	case INPUT_ESC_RI:
		screen_write_reverseindex(sctx);
		break;
	case INPUT_ESC_DECKPAM:
		screen_write_mode_set(sctx, MODE_KKEYPAD);
		break;
	case INPUT_ESC_DECKPNM:
		screen_write_mode_clear(sctx, MODE_KKEYPAD);
		break;
	case INPUT_ESC_DECSC:
		memcpy(&ictx->old_cell, &ictx->cell, sizeof ictx->old_cell);
		ictx->old_cx = s->cx;
		ictx->old_cy = s->cy;
		break;
	case INPUT_ESC_DECRC:
		memcpy(&ictx->cell, &ictx->old_cell, sizeof ictx->cell);
		screen_write_cursormove(sctx, ictx->old_cx, ictx->old_cy);
		break;
	case INPUT_ESC_DECALN:
		screen_write_alignmenttest(sctx);
		break;
	case INPUT_ESC_SCSG0_ON:
		ictx->cell.g0set = 1;
		break;
	case INPUT_ESC_SCSG0_OFF:
		ictx->cell.g0set = 0;
		break;
	case INPUT_ESC_SCSG1_ON:
		ictx->cell.g1set = 1;
		break;
	case INPUT_ESC_SCSG1_OFF:
		ictx->cell.g1set = 0;
		break;
	}

	return (0);
}