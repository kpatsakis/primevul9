int sc_hex_to_bin(const char *in, u8 *out, size_t *outlen)
{
	int err = SC_SUCCESS;
	size_t left, count = 0, in_len;

	if (in == NULL || out == NULL || outlen == NULL) {
		return SC_ERROR_INVALID_ARGUMENTS;
	}
	left = *outlen;
	in_len = strlen(in);

	while (*in != '\0') {
		int byte = 0, nybbles = 2;

		while (nybbles-- && *in && *in != ':' && *in != ' ') {
			char c;
			byte <<= 4;
			c = *in++;
			if ('0' <= c && c <= '9')
				c -= '0';
			else
			if ('a' <= c && c <= 'f')
				c = c - 'a' + 10;
			else
			if ('A' <= c && c <= 'F')
				c = c - 'A' + 10;
			else {
				err = SC_ERROR_INVALID_ARGUMENTS;
				goto out;
			}
			byte |= c;
		}

		/* Detect premature end of string before byte is complete */
		if (in_len > 1 && *in == '\0' && nybbles >= 0) {
			err = SC_ERROR_INVALID_ARGUMENTS;
			break;
		}

		if (*in == ':' || *in == ' ')
			in++;
		if (left <= 0) {
			err = SC_ERROR_BUFFER_TOO_SMALL;
			break;
		}
		out[count++] = (u8) byte;
		left--;
	}

out:
	*outlen = count;
	return err;
}