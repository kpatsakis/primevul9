imapx_untagged_preauth (CamelIMAPXServer *is,
                        GInputStream *input_stream,
                        GCancellable *cancellable,
                        GError **error)
{
	g_return_val_if_fail (CAMEL_IS_IMAPX_SERVER (is), FALSE);

	c (is->priv->tagprefix, "preauthenticated\n");
	if (is->priv->state < IMAPX_AUTHENTICATED)
		is->priv->state = IMAPX_AUTHENTICATED;

	return TRUE;
}