gchar *conv_iconv_strdup_with_cd(const gchar *inbuf, iconv_t cd)
{
	const gchar *inbuf_p;
	gchar *outbuf;
	gchar *outbuf_p;
	size_t in_size;
	size_t in_left;
	size_t out_size;
	size_t out_left;
	size_t n_conv;
	size_t len;

	inbuf_p = inbuf;
	in_size = strlen(inbuf);
	in_left = in_size;
	out_size = (in_size + 1) * 2;
	outbuf = g_malloc(out_size);
	outbuf_p = outbuf;
	out_left = out_size;

#define EXPAND_BUF()				\
{						\
	len = outbuf_p - outbuf;		\
	out_size *= 2;				\
	outbuf = g_realloc(outbuf, out_size);	\
	outbuf_p = outbuf + len;		\
	out_left = out_size - len;		\
}

	while ((n_conv = iconv(cd, (ICONV_CONST gchar **)&inbuf_p, &in_left,
			       &outbuf_p, &out_left)) == (size_t)-1) {
		if (EILSEQ == errno) {
			if (strict_mode) {
				g_free(outbuf);
				return NULL;
			}
			//g_print("iconv(): at %d: %s\n", in_size - in_left, g_strerror(errno));
			inbuf_p++;
			in_left--;
			if (out_left == 0) {
				EXPAND_BUF();
			}
			*outbuf_p++ = SUBST_CHAR;
			out_left--;
		} else if (EINVAL == errno) {
			break;
		} else if (E2BIG == errno) {
			EXPAND_BUF();
		} else {
			g_warning("conv_iconv_strdup(): %s",
				  g_strerror(errno));
			break;
		}
	}

	while ((n_conv = iconv(cd, NULL, NULL, &outbuf_p, &out_left)) ==
	       (size_t)-1) {
		if (E2BIG == errno) {
			EXPAND_BUF();
		} else {
			g_warning("conv_iconv_strdup(): %s",
				  g_strerror(errno));
			break;
		}
	}

#undef EXPAND_BUF

	len = outbuf_p - outbuf;
	outbuf = g_realloc(outbuf, len + 1);
	outbuf[len] = '\0';

	return outbuf;
}