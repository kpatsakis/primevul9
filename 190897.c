   Encode a string in modified UTF-7 */
PHP_FUNCTION(imap_utf7_encode)
{
	/* author: Andrew Skalski <askalski@chek.com> */
	zend_string *arg;
	const unsigned char *in, *inp, *endp;
	zend_string *out;
	unsigned char *outp;
	unsigned char c;
	int inlen, outlen;
	enum {
		ST_NORMAL,	/* printable text */
		ST_ENCODE0,	/* encoded text rotation... */
		ST_ENCODE1,
		ST_ENCODE2
	} state;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &arg) == FAILURE) {
		return;
	}

	in = (const unsigned char *) ZSTR_VAL(arg);
	inlen = ZSTR_LEN(arg);

	/* compute the length of the result string */
	outlen = 0;
	state = ST_NORMAL;
	endp = (inp = in) + inlen;
	while (inp < endp) {
		if (state == ST_NORMAL) {
			if (SPECIAL(*inp)) {
				state = ST_ENCODE0;
				outlen++;
			} else if (*inp++ == '&') {
				outlen++;
			}
			outlen++;
		} else if (!SPECIAL(*inp)) {
			state = ST_NORMAL;
		} else {
			/* ST_ENCODE0 -> ST_ENCODE1	- two chars
			 * ST_ENCODE1 -> ST_ENCODE2	- one char
			 * ST_ENCODE2 -> ST_ENCODE0	- one char
			 */
			if (state == ST_ENCODE2) {
				state = ST_ENCODE0;
			}
			else if (state++ == ST_ENCODE0) {
				outlen++;
			}
			outlen++;
			inp++;
		}
	}

	/* allocate output buffer */
	out = zend_string_safe_alloc(1, outlen, 0, 0);

	/* encode input string */
	outp = (unsigned char*)ZSTR_VAL(out);
	state = ST_NORMAL;
	endp = (inp = in) + inlen;
	while (inp < endp || state != ST_NORMAL) {
		if (state == ST_NORMAL) {
			if (SPECIAL(*inp)) {
				/* begin encoding */
				*outp++ = '&';
				state = ST_ENCODE0;
			} else if ((*outp++ = *inp++) == '&') {
				*outp++ = '-';
			}
		} else if (inp == endp || !SPECIAL(*inp)) {
			/* flush overflow and terminate region */
			if (state != ST_ENCODE0) {
				c = B64(*outp);
				*outp++ = c;
			}
			*outp++ = '-';
			state = ST_NORMAL;
		} else {
			/* encode input character */
			switch (state) {
				case ST_ENCODE0:
					*outp++ = B64(*inp >> 2);
					*outp = *inp++ << 4;
					state = ST_ENCODE1;
					break;
				case ST_ENCODE1:
					c = B64(*outp | *inp >> 4);
					*outp++ = c;
					*outp = *inp++ << 2;
					state = ST_ENCODE2;
					break;
				case ST_ENCODE2:
					c = B64(*outp | *inp >> 6);
					*outp++ = c;
					*outp++ = B64(*inp++);
					state = ST_ENCODE0;
				case ST_NORMAL:
					break;
			}
		}
	}

	*outp = 0;

	RETURN_STR(out);