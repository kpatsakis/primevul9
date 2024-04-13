imapx_untagged_flags (CamelIMAPXServer *is,
                      GInputStream *input_stream,
                      GCancellable *cancellable,
                      GError **error)
{
	guint32 flags = 0;
	gboolean success;

	g_return_val_if_fail (CAMEL_IS_IMAPX_SERVER (is), FALSE);

	success = imapx_parse_flags (
		CAMEL_IMAPX_INPUT_STREAM (input_stream),
		&flags, NULL, cancellable, error);

	c (is->priv->tagprefix, "flags: %08x\n", flags);

	return success;
}