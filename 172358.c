add_initial_untagged_descriptor (GHashTable *untagged_handlers,
                                 guint untagged_id)
{
	const CamelIMAPXUntaggedRespHandlerDesc *prev = NULL;
	const CamelIMAPXUntaggedRespHandlerDesc *cur = NULL;

	g_return_if_fail (untagged_handlers != NULL);
	g_return_if_fail (untagged_id < IMAPX_UNTAGGED_LAST_ID);

	cur = &(_untagged_descr[untagged_id]);
	prev = replace_untagged_descriptor (
		untagged_handlers,
		cur->untagged_response,
		cur);
	/* there must not be any previous handler here */
	g_return_if_fail (prev == NULL);
}