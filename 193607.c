static int gg_send_message_common(struct gg_session *sess, int msgclass, int recipients_count, uin_t *recipients, const unsigned char *message, const unsigned char *format, int formatlen, const unsigned char *html_message)
{
	struct gg_send_msg80 s80;
	const char *cp_msg = NULL, *utf_html_msg = NULL;
	char *recoded_msg = NULL, *recoded_html_msg = NULL;
	unsigned char *generated_format = NULL;
	int seq_no = -1;

	gg_debug_session(sess, GG_DEBUG_FUNCTION, "** gg_send_message_common(%p, %d, %d, %p, %p, %p, %d, %p);\n", sess, msgclass, recipients_count, recipients, message, format, formatlen, html_message);

	if (!sess) {
		errno = EFAULT;
		return -1;
	}

	if (sess->state != GG_STATE_CONNECTED) {
		errno = ENOTCONN;
		return -1;
	}

	if ((message == NULL && html_message == NULL) || recipients_count <= 0 || recipients_count > 0xffff || recipients == NULL || (format == NULL && formatlen != 0)) {
		errno = EINVAL;
		return -1;
	}

	if (sess->protocol_version >= GG_PROTOCOL_110 && recipients_count == 1)
	{
		int is_html = (html_message != NULL);
		seq_no = gg_send_message_110(sess, recipients[0], 0,
			(const char*)(is_html ? html_message : message),
			is_html);
		goto cleanup;
	}

	if (message == NULL) {
		char *tmp_msg;
		size_t len, fmt_len;
		uint16_t fixed_fmt_len;

		len = gg_message_html_to_text(NULL, NULL, &fmt_len, (const char*) html_message, sess->encoding);

		tmp_msg = malloc(len + 1);

		if (tmp_msg == NULL)
			goto cleanup;

		if (fmt_len != 0) {
			generated_format = malloc(fmt_len + 3);

			if (generated_format == NULL) {
				free(tmp_msg);
				goto cleanup;
			}

			generated_format[0] = '\x02';
			fixed_fmt_len = gg_fix16(fmt_len);
			memcpy(generated_format + 1, &fixed_fmt_len, sizeof(fixed_fmt_len));
			gg_message_html_to_text(tmp_msg, generated_format + 3, NULL, (const char*) html_message, sess->encoding);

			format = generated_format;
			formatlen = fmt_len + 3;
		} else {
			gg_message_html_to_text(tmp_msg, NULL, NULL, (const char*) html_message, sess->encoding);

			format = NULL;
			formatlen = 0;
		}

		if (sess->encoding == GG_ENCODING_UTF8) {
			cp_msg = recoded_msg = gg_encoding_convert(tmp_msg, sess->encoding, GG_ENCODING_CP1250, -1, -1);
			free(tmp_msg);

			if (cp_msg == NULL)
				goto cleanup;
		} else {
			cp_msg = recoded_msg = tmp_msg;
		}
	} else {
		if (sess->encoding == GG_ENCODING_UTF8) {
			cp_msg = recoded_msg = gg_encoding_convert((const char*) message, sess->encoding, GG_ENCODING_CP1250, -1, -1);

			if (cp_msg == NULL)
				goto cleanup;
		} else {
			cp_msg = (const char*) message;
		}
	}

	if (html_message == NULL) {
		size_t len;
		char *tmp;
		const char *utf_msg;
		const unsigned char *format_ = NULL;
		size_t formatlen_ = 0;

		if (sess->encoding == GG_ENCODING_UTF8) {
			utf_msg = (const char*) message;
		} else {
			utf_msg = recoded_msg = gg_encoding_convert((const char*) message, sess->encoding, GG_ENCODING_UTF8, -1, -1);

			if (utf_msg == NULL)
				goto cleanup;
		}

		if (format != NULL && formatlen >= 3) {
			format_ = format + 3;
			formatlen_ = formatlen - 3;
		}

		len = gg_message_text_to_html(NULL, utf_msg, GG_ENCODING_UTF8, format_, formatlen_);

		tmp = malloc(len + 1);

		if (tmp == NULL)
			goto cleanup;

		gg_message_text_to_html(tmp, utf_msg, GG_ENCODING_UTF8, format_, formatlen_);

		utf_html_msg = recoded_html_msg = tmp;
	} else {
		if (sess->encoding == GG_ENCODING_UTF8) {
			utf_html_msg = (const char*) html_message;
		} else {
			utf_html_msg = recoded_html_msg = gg_encoding_convert((const char*) html_message, sess->encoding, GG_ENCODING_UTF8, -1, -1);

			if (utf_html_msg == NULL)
				goto cleanup;
		}
	}

	/* Drobne odchylenie od protokołu. Jeśli wysyłamy kilka
	 * wiadomości w ciągu jednej sekundy, zwiększamy poprzednią
	 * wartość, żeby każda wiadomość miała unikalny numer.
	 */

	seq_no = time(NULL);

	if (seq_no <= sess->seq)
		seq_no = sess->seq + 1;

	sess->seq = seq_no;

	s80.seq = gg_fix32(seq_no);
	s80.msgclass = gg_fix32(msgclass);
	s80.offset_plain = gg_fix32(sizeof(s80) + strlen(utf_html_msg) + 1);
	s80.offset_attr = gg_fix32(sizeof(s80) + strlen(utf_html_msg) + 1 + strlen(cp_msg) + 1);

	if (recipients_count > 1) {
		struct gg_msg_recipients r;
		int i, j, k;
		uin_t *recps;

		r.flag = GG_MSG_OPTION_CONFERENCE;
		r.count = gg_fix32(recipients_count - 1);

		recps = malloc(sizeof(uin_t) * (recipients_count - 1));

		if (!recps) {
			seq_no = -1;
			goto cleanup;
		}

		for (i = 0; i < recipients_count; i++) {
			for (j = 0, k = 0; j < recipients_count; j++) {
				if (j != i) {
					recps[k] = gg_fix32(recipients[j]);
					k++;
				}
			}

			s80.recipient = gg_fix32(recipients[i]);

			if (gg_send_packet(sess, GG_SEND_MSG80, &s80, sizeof(s80), utf_html_msg, strlen(utf_html_msg) + 1, cp_msg, strlen(cp_msg) + 1, &r, sizeof(r), recps, (recipients_count - 1) * sizeof(uin_t), format, formatlen, NULL) == -1)
				seq_no = -1;
		}

		free(recps);
	} else {
		s80.recipient = gg_fix32(recipients[0]);

		if (gg_send_packet(sess, GG_SEND_MSG80, &s80, sizeof(s80), utf_html_msg, strlen(utf_html_msg) + 1, cp_msg, strlen(cp_msg) + 1, format, formatlen, NULL) == -1)
			seq_no = -1;
	}

cleanup:
	free(recoded_msg);
	free(recoded_html_msg);
	free(generated_format);

	return seq_no;
}