qword_to_hex_punct(char *out, guint64 qword, char punct)
{
	out = dword_to_hex_punct(out, (guint32)(qword >> 32), punct);
	*out++ = punct;
	out = dword_to_hex_punct(out, (guint32)(qword & 0xffffffff), punct);
	return out;
}