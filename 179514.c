bytes_to_hexstr(char *out, const guint8 *ad, guint32 len)
{
	guint32 i;

	if (!ad)
		REPORT_DISSECTOR_BUG("Null pointer passed to bytes_to_hexstr()");

	for (i = 0; i < len; i++)
		out = byte_to_hex(out, ad[i]);
	return out;
}