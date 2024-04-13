word_to_hex_punct(char *out, guint16 word, char punct)
{
	out = byte_to_hex(out, word >> 8);
	*out++ = punct;
	out = byte_to_hex(out, word);
	return out;
}