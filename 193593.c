static int gg_send_message_110(struct gg_session *sess,
	uin_t recipient, uint64_t chat_id,
	const char *message, int is_html)
{
	gg_tvbuilder_t *tvb;
	char *html_message_gen = NULL, *plain_message_gen = NULL;
	int seq;
	const char *html_message, *plain_message;

	gg_debug_session(sess, GG_DEBUG_FUNCTION,
		"** gg_send_message_110(%p, %u, %llu, %p, %d);\n",
		sess, recipient, chat_id, message, is_html);

	if (message == NULL)
		return -1;

	if ((recipient == 0) == (chat_id == 0))
		return -1;

	if (is_html) {
		html_message = message;

		if (sess->encoding != GG_ENCODING_UTF8) {
			html_message = html_message_gen = gg_encoding_convert(
				html_message, sess->encoding, GG_ENCODING_UTF8,
				-1, -1);
			if (html_message_gen == NULL)
				return -1;
		}

		plain_message = plain_message_gen =
			gg_message_html_to_text_110(html_message);
		if (plain_message_gen == NULL) {
			free(html_message_gen);
			return -1;
		}
	} else {
		plain_message = message;

		if (sess->encoding != GG_ENCODING_UTF8) {
			plain_message = plain_message_gen = gg_encoding_convert(
				plain_message, sess->encoding, GG_ENCODING_UTF8,
				-1, -1);
			if (plain_message_gen == NULL)
				return -1;
		}
		
		html_message = html_message_gen =
			gg_message_text_to_html_110(plain_message, -1);
		if (html_message_gen == NULL) {
			free(plain_message_gen);
			return -1;
		}
	}

	seq = ++sess->seq;

	tvb = gg_tvbuilder_new(sess, NULL);
	gg_tvbuilder_expected_size(tvb, 50 + 2 * strlen(html_message));

	if (recipient) {
		gg_tvbuilder_write_uint8(tvb, 0x0a);
		gg_tvbuilder_write_uin(tvb, recipient, 1);
	}

	gg_tvbuilder_write_uint8(tvb, 0x10);
	gg_tvbuilder_write_uint8(tvb, 0x08); /* magic */

	gg_tvbuilder_write_uint8(tvb, 0x18);
	gg_tvbuilder_write_packed_uint(tvb, seq);

	gg_tvbuilder_write_uint8(tvb, 0x2a);
	gg_tvbuilder_write_str(tvb, plain_message, -1);

	gg_tvbuilder_write_uint8(tvb, 0x32);
	gg_tvbuilder_write_str(tvb, html_message, -1);

	if (chat_id) {
		gg_tvbuilder_write_uint8(tvb, 0x3a);
		gg_tvbuilder_write_str(tvb, "", 0); /* magic */

		gg_tvbuilder_write_uint8(tvb, 0x51);
		gg_tvbuilder_write_uint64(tvb, chat_id);
	}

	free(html_message_gen);
	free(plain_message_gen);

	if (!gg_tvbuilder_send(tvb, recipient ?
		GG_SEND_MSG110 : GG_CHAT_SEND_MSG))
		return -1;

	return seq;
}