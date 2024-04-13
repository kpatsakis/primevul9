imapx_server_cinfo_has_attachment_cb (CamelMessageContentInfo *ci,
				      gint depth,
				      gpointer user_data)
{
	gboolean *pbool = user_data;

	g_return_val_if_fail (pbool != NULL, FALSE);

	*pbool = camel_content_disposition_is_attachment_ex (ci->disposition, ci->type, ci->parent ? ci->parent->type : NULL);

	return !*pbool;
}