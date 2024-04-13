input_utf8_add(struct input_ctx *ictx)
{
	log_debug("%s", __func__);

	utf8_append(&ictx->utf8data, ictx->ch);
	return (0);
}