   Decode a modified UTF-7 string */
PHP_FUNCTION(imap_utf7_decode)
{
	/* author: Andrew Skalski <askalski@chek.com> */
	zend_string *arg;
	const unsigned char *in, *inp, *endp;
	unsigned char *out, *outp;
	unsigned char c;
	int inlen, outlen;
	enum {
		ST_NORMAL,	/* printable text */
		ST_DECODE0,	/* encoded text rotation... */
		ST_DECODE1,
		ST_DECODE2,
		ST_DECODE3
	} state;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &arg) == FAILURE) {
		return;
	}

	in = (const unsigned char *) ZSTR_VAL(arg);
	inlen = ZSTR_LEN(arg);

	/* validate and compute length of output string */
	outlen = 0;
	state = ST_NORMAL;
	for (endp = (inp = in) + inlen; inp < endp; inp++) {
		if (state == ST_NORMAL) {
			/* process printable character */
			if (SPECIAL(*inp)) {
				php_error_docref(NULL, E_WARNING, "Invalid modified UTF-7 character: `%c'", *inp);
				RETURN_FALSE;
			} else if (*inp != '&') {
				outlen++;
			} else if (inp + 1 == endp) {
				php_error_docref(NULL, E_WARNING, "Unexpected end of string");
				RETURN_FALSE;
			} else if (inp[1] != '-') {
				state = ST_DECODE0;
			} else {
				outlen++;
				inp++;
			}
		} else if (*inp == '-') {
			/* return to NORMAL mode */
			if (state == ST_DECODE1) {
				php_error_docref(NULL, E_WARNING, "Stray modified base64 character: `%c'", *--inp);
				RETURN_FALSE;
			}
			state = ST_NORMAL;
		} else if (!B64CHAR(*inp)) {
			php_error_docref(NULL, E_WARNING, "Invalid modified base64 character: `%c'", *inp);
			RETURN_FALSE;
		} else {
			switch (state) {
				case ST_DECODE3:
					outlen++;
					state = ST_DECODE0;
					break;
				case ST_DECODE2:
				case ST_DECODE1:
					outlen++;
				case ST_DECODE0:
					state++;
				case ST_NORMAL:
					break;
			}
		}
	}

	/* enforce end state */
	if (state != ST_NORMAL) {
		php_error_docref(NULL, E_WARNING, "Unexpected end of string");
		RETURN_FALSE;
	}

	/* allocate output buffer */
	out = emalloc(outlen + 1);

	/* decode input string */
	outp = out;
	state = ST_NORMAL;
	for (endp = (inp = in) + inlen; inp < endp; inp++) {
		if (state == ST_NORMAL) {
			if (*inp == '&' && inp[1] != '-') {
				state = ST_DECODE0;
			}
			else if ((*outp++ = *inp) == '&') {
				inp++;
			}
		}
		else if (*inp == '-') {
			state = ST_NORMAL;
		}
		else {
			/* decode input character */
			switch (state) {
			case ST_DECODE0:
				*outp = UNB64(*inp) << 2;
				state = ST_DECODE1;
				break;
			case ST_DECODE1:
				outp[1] = UNB64(*inp);
				c = outp[1] >> 4;
				*outp++ |= c;
				*outp <<= 4;
				state = ST_DECODE2;
				break;
			case ST_DECODE2:
				outp[1] = UNB64(*inp);
				c = outp[1] >> 2;
				*outp++ |= c;
				*outp <<= 6;
				state = ST_DECODE3;
				break;
			case ST_DECODE3:
				*outp++ |= UNB64(*inp);
				state = ST_DECODE0;
			case ST_NORMAL:
				break;
			}
		}
	}

	*outp = 0;

#if PHP_DEBUG
	/* warn if we computed outlen incorrectly */
	if (outp - out != outlen) {
		php_error_docref(NULL, E_WARNING, "outp - out [%zd] != outlen [%d]", outp - out, outlen);
	}
#endif

	RETURN_STRINGL((char*)out, outlen);