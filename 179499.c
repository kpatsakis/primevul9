word_to_hex(char *out, guint16 word)
{
	out = byte_to_hex(out, word >> 8);
	out = byte_to_hex(out, word);
	return out;
}