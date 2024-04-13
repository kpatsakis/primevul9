dword_to_hex(char *out, guint32 dword)
{
	out = word_to_hex(out, dword >> 16);
	out = word_to_hex(out, dword);
	return out;
}