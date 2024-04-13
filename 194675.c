void textview_show_part(TextView *textview, MimeInfo *mimeinfo, FILE *fp)
{
	START_TIMING("");
	cm_return_if_fail(mimeinfo != NULL);
	cm_return_if_fail(fp != NULL);

	textview->loading = TRUE;
	textview->stop_loading = FALSE;

	textview_clear(textview);

	if (mimeinfo->type == MIMETYPE_MULTIPART ||
	    (mimeinfo->type == MIMETYPE_MESSAGE && !g_ascii_strcasecmp(mimeinfo->subtype, "rfc822"))) {
		textview_add_parts(textview, mimeinfo);
	} else {
		if (fseek(fp, mimeinfo->offset, SEEK_SET) < 0)
			perror("fseek");

		textview_write_body(textview, mimeinfo);
	}

	textview->loading = FALSE;
	textview->stop_loading = FALSE;
	textview_set_position(textview, 0);

	END_TIMING();
}