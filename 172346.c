camel_imapx_server_set_tagprefix (CamelIMAPXServer *is,
				  gchar tagprefix)
{
	g_return_if_fail (CAMEL_IS_IMAPX_SERVER (is));
	g_return_if_fail ((tagprefix >= 'A' && tagprefix <= 'Z') || (tagprefix >= 'a' && tagprefix <= 'z'));

	is->priv->tagprefix = tagprefix;
}