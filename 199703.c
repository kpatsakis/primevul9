input_c0_dispatch(struct input_ctx *ictx)
{
	struct screen_write_ctx	*sctx = &ictx->ctx;
	struct window_pane	*wp = ictx->wp;
	struct screen		*s = sctx->s;

	ictx->utf8started = 0; /* can't be valid UTF-8 */

	log_debug("%s: '%c'", __func__, ictx->ch);

	switch (ictx->ch) {
	case '\000':	/* NUL */
		break;
	case '\007':	/* BEL */
		if (wp != NULL)
			alerts_queue(wp->window, WINDOW_BELL);
		break;
	case '\010':	/* BS */
		screen_write_backspace(sctx);
		break;
	case '\011':	/* HT */
		/* Don't tab beyond the end of the line. */
		if (s->cx >= screen_size_x(s) - 1)
			break;

		/* Find the next tab point, or use the last column if none. */
		do {
			s->cx++;
			if (bit_test(s->tabs, s->cx))
				break;
		} while (s->cx < screen_size_x(s) - 1);
		break;
	case '\012':	/* LF */
	case '\013':	/* VT */
	case '\014':	/* FF */
		screen_write_linefeed(sctx, 0, ictx->cell.cell.bg);
		if (s->mode & MODE_CRLF)
			screen_write_carriagereturn(sctx);
		break;
	case '\015':	/* CR */
		screen_write_carriagereturn(sctx);
		break;
	case '\016':	/* SO */
		ictx->cell.set = 1;
		break;
	case '\017':	/* SI */
		ictx->cell.set = 0;
		break;
	default:
		log_debug("%s: unknown '%c'", __func__, ictx->ch);
		break;
	}

	ictx->last = -1;
	return (0);
}