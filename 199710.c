input_osc_52(struct input_ctx *ictx, const char *p)
{
	struct window_pane	*wp = ictx->wp;
	char			*end;
	const char		*buf;
	size_t			 len;
	u_char			*out;
	int			 outlen, state;
	struct screen_write_ctx	 ctx;
	struct paste_buffer	*pb;

	if (wp == NULL)
		return;
	state = options_get_number(global_options, "set-clipboard");
	if (state != 2)
		return;

	if ((end = strchr(p, ';')) == NULL)
		return;
	end++;
	if (*end == '\0')
		return;
	log_debug("%s: %s", __func__, end);

	if (strcmp(end, "?") == 0) {
		if ((pb = paste_get_top(NULL)) != NULL) {
			buf = paste_buffer_data(pb, &len);
			outlen = 4 * ((len + 2) / 3) + 1;
			out = xmalloc(outlen);
			if ((outlen = b64_ntop(buf, len, out, outlen)) == -1) {
				free(out);
				return;
			}
		} else {
			outlen = 0;
			out = NULL;
		}
		bufferevent_write(ictx->event, "\033]52;;", 6);
		if (outlen != 0)
			bufferevent_write(ictx->event, out, outlen);
		if (ictx->input_end == INPUT_END_BEL)
			bufferevent_write(ictx->event, "\007", 1);
		else
			bufferevent_write(ictx->event, "\033\\", 2);
		free(out);
		return;
	}

	len = (strlen(end) / 4) * 3;
	if (len == 0)
		return;

	out = xmalloc(len);
	if ((outlen = b64_pton(end, out, len)) == -1) {
		free(out);
		return;
	}

	screen_write_start_pane(&ctx, wp, NULL);
	screen_write_setselection(&ctx, out, outlen);
	screen_write_stop(&ctx);
	notify_pane("pane-set-clipboard", wp);

	paste_add(NULL, out, outlen);
}