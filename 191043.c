static bool create_outbuf(TALLOC_CTX *mem_ctx, struct smb_request *req,
			  const char *inbuf, char **outbuf, uint8_t num_words,
			  uint32_t num_bytes)
{
	/*
         * Protect against integer wrap
         */
	if ((num_bytes > 0xffffff)
	    || ((num_bytes + smb_size + num_words*2) > 0xffffff)) {
		char *msg;
		if (asprintf(&msg, "num_bytes too large: %u",
			     (unsigned)num_bytes) == -1) {
			msg = CONST_DISCARD(char *, "num_bytes too large");
		}
		smb_panic(msg);
	}

	*outbuf = TALLOC_ARRAY(mem_ctx, char,
			       smb_size + num_words*2 + num_bytes);
	if (*outbuf == NULL) {
		return false;
	}

	construct_reply_common(req, inbuf, *outbuf);
	srv_set_message(*outbuf, num_words, num_bytes, false);
	/*
	 * Zero out the word area, the caller has to take care of the bcc area
	 * himself
	 */
	if (num_words != 0) {
		memset(*outbuf + smb_vwv0, 0, num_words*2);
	}

	return true;
}