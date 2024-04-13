static int setcos_process_fci(sc_card_t *card, sc_file_t *file,
		       const u8 *buf, size_t buflen)
{
	int r = iso_ops->process_fci(card, file, buf, buflen);

	/* SetCOS 4.4: RSA key file is an internal EF but it's
	 * file descriptor doesn't seem to follow ISO7816. */
	if (r >= 0 && (card->type == SC_CARD_TYPE_SETCOS_44 ||
	               SETCOS_IS_EID_APPLET(card))) {
		const u8 *tag;
		size_t taglen = 1;
		tag = (u8 *) sc_asn1_find_tag(card->ctx, buf, buflen, 0x82, &taglen);
		if (tag != NULL && taglen == 1 && *tag == 0x11)
			file->type = SC_FILE_TYPE_INTERNAL_EF;
	}

	return r;
}