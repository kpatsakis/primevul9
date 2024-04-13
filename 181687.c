des_cipher(const char *in, char *out, uint32_t salt, int count,
	struct php_crypt_extended_data *data)
{
	uint32_t	l_out, r_out, rawl, rawr;
	int	retval;

	setup_salt(salt, data);

	rawl =
		(uint32_t)(u_char)in[3] |
		((uint32_t)(u_char)in[2] << 8) |
		((uint32_t)(u_char)in[1] << 16) |
		((uint32_t)(u_char)in[0] << 24);
	rawr =
		(uint32_t)(u_char)in[7] |
		((uint32_t)(u_char)in[6] << 8) |
		((uint32_t)(u_char)in[5] << 16) |
		((uint32_t)(u_char)in[4] << 24);

	retval = do_des(rawl, rawr, &l_out, &r_out, count, data);

	out[0] = l_out >> 24;
	out[1] = l_out >> 16;
	out[2] = l_out >> 8;
	out[3] = l_out;
	out[4] = r_out >> 24;
	out[5] = r_out >> 16;
	out[6] = r_out >> 8;
	out[7] = r_out;

	return(retval);
}