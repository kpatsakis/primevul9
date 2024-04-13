qword_to_hex(char *out, guint64 qword)
{
	out = dword_to_hex(out, (guint32)(qword >> 32));
	out = dword_to_hex(out, (guint32)(qword & 0xffffffff));
	return out;
}