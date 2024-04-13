int sc_bin_to_hex(const u8 *in, size_t in_len, char *out, size_t out_len,
		  int in_sep)
{
	unsigned int	n, sep_len;
	char		*pos, *end, sep;

	sep = (char)in_sep;
	sep_len = sep > 0 ? 1 : 0;
	pos = out;
	end = out + out_len;
	for (n = 0; n < in_len; n++) {
		if (pos + 3 + sep_len >= end)
			return SC_ERROR_BUFFER_TOO_SMALL;
		if (n && sep_len)
			*pos++ = sep;
		sprintf(pos, "%02x", in[n]);
		pos += 2;
	}
	*pos = '\0';
	return SC_SUCCESS;
}