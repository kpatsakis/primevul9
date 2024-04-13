input_utf8_open(struct input_ctx *ictx)
{
	if (!options_get_number(&ictx->wp->window->options, "utf8")) {
		/* Print, and do not switch state. */
		input_print(ictx);
		return (-1);
	}
	log_debug("%s", __func__);

	utf8_open(&ictx->utf8data, ictx->ch);
	return (0);
}