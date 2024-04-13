static void rcc_pushstr(REgg *egg, char *str, int filter) {
	int dotrim;
	int i, j, len, ch;
	REggEmit *e = egg->remit;

	e->comment (egg, "encode %s string (%s) (%s)",
		filter? "filtered": "unfiltered", str, egg->lang.callname);

// fixed by izhuer
	if (filter) {
		for (i = 0; str[i]; i++) {
			dotrim = 0;
			if (str[i] == '\\') {
				switch (str[i + 1]) {
				case 't':
					str[i] = '\t';
					dotrim = 1;
					break;
				case 'n':
					str[i] = '\n';
					dotrim = 1;
					break;
				case 'e':
					str[i] = '\x1b';
					dotrim = 1;
					break;
				case 'x':
					ch = r_hex_pair2bin (str + i + 2);
					if (ch == -1) {
						eprintf ("%s:%d Error string format\n",
							egg->lang.file, egg->lang.line);
					}
					str[i] = (char) ch;
					dotrim = 3;
					break;
				default:
					break;
				}
				if (dotrim) {
					memmove (str + i + 1, str + i + dotrim + 1,
						strlen (str + i + dotrim + 1) + 1);
				}
				// DO NOT forget the '\x00' terminate char
			}
		}
	}

	len = strlen (str);
	j = (len - len % e->size) + e->size;
	e->set_string (egg, egg->lang.dstvar, str, j);
	R_FREE (egg->lang.dstvar);
}