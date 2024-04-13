imapx_untagged_capability (CamelIMAPXServer *is,
                           GInputStream *input_stream,
                           GCancellable *cancellable,
                           GError **error)
{
	struct _capability_info *cinfo;

	g_return_val_if_fail (CAMEL_IS_IMAPX_SERVER (is), FALSE);

	g_mutex_lock (&is->priv->stream_lock);

	if (is->priv->cinfo != NULL) {
		imapx_free_capability (is->priv->cinfo);
		is->priv->cinfo = NULL;
	}

	g_mutex_unlock (&is->priv->stream_lock);

	cinfo = imapx_parse_capability (CAMEL_IMAPX_INPUT_STREAM (input_stream), cancellable, error);

	if (!cinfo)
		return FALSE;

	g_mutex_lock (&is->priv->stream_lock);

	if (is->priv->cinfo != NULL)
		imapx_free_capability (is->priv->cinfo);
	is->priv->cinfo = cinfo;

	c (is->priv->tagprefix, "got capability flags %08x\n", is->priv->cinfo->capa);

	imapx_server_stash_command_arguments (is);

	g_mutex_unlock (&is->priv->stream_lock);

	return TRUE;
}