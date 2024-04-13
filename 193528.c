read_from_buf (SoupFilterInputStream *fstream, gpointer buffer, gsize count)
{
	GByteArray *buf = fstream->priv->buf;

	if (buf->len < count)
		count = buf->len;
	memcpy (buffer, buf->data, count);

	if (count == buf->len) {
		g_byte_array_free (buf, TRUE);
		fstream->priv->buf = NULL;
	} else {
		memmove (buf->data, buf->data + count,
			 buf->len - count);
		g_byte_array_set_size (buf, buf->len - count);
	}

	return count;
}