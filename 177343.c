void conv_encode_header(gchar *dest, gint len, const gchar *src,
			gint header_len, gboolean addr_field)
{
	conv_encode_header_full(dest,len,src,header_len,addr_field,NULL);
}