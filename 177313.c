void conv_encode_header_full(gchar *dest, gint len, const gchar *src,
			gint header_len, gboolean addr_field,
			const gchar *out_encoding_)
{
	const gchar *cur_encoding;
	const gchar *out_encoding;
	gint mimestr_len;
	gchar *mimesep_enc;
	gint left;
	const guchar *srcp = src;
	guchar *destp = dest;
	gboolean use_base64;

	cm_return_if_fail(g_utf8_validate(src, -1, NULL) == TRUE);
	cm_return_if_fail(destp != NULL);

	if (MB_CUR_MAX > 1) {
		use_base64 = TRUE;
		mimesep_enc = "?B?";
	} else {
		use_base64 = FALSE;
		mimesep_enc = "?Q?";
	}

	cur_encoding = CS_INTERNAL;

	if (out_encoding_)
		out_encoding = out_encoding_;
	else
		out_encoding = conv_get_outgoing_charset_str();

	if (!strcmp(out_encoding, CS_US_ASCII))
		out_encoding = CS_ISO_8859_1;

	mimestr_len = strlen(MIMESEP_BEGIN) + strlen(out_encoding) +
		strlen(mimesep_enc) + strlen(MIMESEP_END);

	left = MAX_LINELEN - header_len;

	while (*srcp) {
		LBREAK_IF_REQUIRED(left <= 0, TRUE);

		while (isspace(*srcp)) {
			*destp++ = *srcp++;
			left--;
			LBREAK_IF_REQUIRED(left <= 0, TRUE);
		}

		/* output as it is if the next word is ASCII string */
		if (!is_next_nonascii(srcp)) {
			gint word_len;

			word_len = get_next_word_len(srcp);
			LBREAK_IF_REQUIRED(left < word_len, TRUE);
			while (word_len > 0) {
				LBREAK_IF_REQUIRED(left + (MAX_HARD_LINELEN - MAX_LINELEN) <= 0, TRUE)
				*destp++ = *srcp++;
				left--;
				word_len--;
			}

			continue;
		}

		/* don't include parentheses and quotes in encoded strings */
		if (addr_field && (*srcp == '(' || *srcp == ')' || *srcp == '"')) {
			LBREAK_IF_REQUIRED(left < 2, FALSE);
			*destp++ = *srcp++;
			left--;
		}

		while (1) {
			gint mb_len = 0;
			gint cur_len = 0;
			gchar *part_str;
			gchar *out_str;
			gchar *enc_str;
			const guchar *p = srcp;
			gint out_str_len;
			gint out_enc_str_len;
			gint mime_block_len;
			gboolean cont = FALSE;

			while (*p != '\0') {
				if (isspace(*p) && !is_next_nonascii(p + 1))
					break;
				/* don't include parentheses in encoded
				   strings */
				if (addr_field && (*p == '(' || *p == ')' || *p == '"'))
					break;

				mb_len = g_utf8_skip[*p];

				Xstrndup_a(part_str, srcp, cur_len + mb_len, );
				out_str = conv_codeset_strdup
					(part_str, cur_encoding, out_encoding);
				if (!out_str) {
					if (strict_mode) {
						*dest = '\0';
						return;
					} else {
						g_warning("conv_encode_header(): code conversion failed");
						conv_unreadable_8bit(part_str);
						out_str = g_strdup(part_str);
					}
				}
				out_str_len = strlen(out_str);

				if (use_base64)
					out_enc_str_len = B64LEN(out_str_len);
				else
					out_enc_str_len =
						qp_get_q_encoding_len(out_str);

				g_free(out_str);

				if (mimestr_len + out_enc_str_len <= left) {
					cur_len += mb_len;
					p += mb_len;
				} else if (cur_len == 0) {
					left = 0;
					LBREAK_IF_REQUIRED(1, FALSE);
					continue;
				} else {
					cont = TRUE;
					break;
				}
			}

			if (cur_len > 0) {
				Xstrndup_a(part_str, srcp, cur_len, );
				out_str = conv_codeset_strdup
					(part_str, cur_encoding, out_encoding);
				if (!out_str) {
					g_warning("conv_encode_header(): code conversion failed");
					conv_unreadable_8bit(part_str);
					out_str = g_strdup(part_str);
				}
				out_str_len = strlen(out_str);

				if (use_base64)
					out_enc_str_len = B64LEN(out_str_len);
				else
					out_enc_str_len =
						qp_get_q_encoding_len(out_str);

				if (use_base64)
					enc_str = g_base64_encode(out_str, out_str_len);
				else {
					Xalloca(enc_str, out_enc_str_len + 1, );
					qp_q_encode(enc_str, out_str);
				}

				g_free(out_str);

				/* output MIME-encoded string block */
				mime_block_len = mimestr_len + strlen(enc_str);
				g_snprintf(destp, mime_block_len + 1,
					   MIMESEP_BEGIN "%s%s%s" MIMESEP_END,
					   out_encoding, mimesep_enc, enc_str);

				if (use_base64)
					g_free(enc_str);

				destp += mime_block_len;
				srcp += cur_len;

				left -= mime_block_len;
			}

			LBREAK_IF_REQUIRED(cont, FALSE);

			if (cur_len == 0)
				break;
		}
	}

	*destp = '\0';
}