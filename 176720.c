static int ldb_parse_hex2char(const char *x)
{
	if (isxdigit(x[0]) && isxdigit(x[1])) {
		const char h1 = x[0], h2 = x[1];
		int c = 0;

		if (h1 >= 'a') c = h1 - (int)'a' + 10;
		else if (h1 >= 'A') c = h1 - (int)'A' + 10;
		else if (h1 >= '0') c = h1 - (int)'0';
		c = c << 4;
		if (h2 >= 'a') c += h2 - (int)'a' + 10;
		else if (h2 >= 'A') c += h2 - (int)'A' + 10;
		else if (h2 >= '0') c += h2 - (int)'0';

		return c;
	}

	return -1;
}