dword_to_hex_punct(char *out, guint32 dword, char punct)
{
	out = word_to_hex_punct(out, dword >> 16, punct);
	*out++ = punct;
	out = word_to_hex_punct(out, dword, punct);
	return out;
}