static void textview_save_contact_pic(TextView *textview)
{
#ifndef USE_ALT_ADDRBOOK
	MsgInfo *msginfo = textview->messageview->msginfo;
	gchar *filename = NULL;
	GError *error = NULL;
	GdkPixbuf *picture = NULL;

	if (!msginfo->extradata || !msginfo->extradata->avatars)
		return;

	if (textview->avatar_type > AVATAR_FACE)
		return;

	if (textview->image) 
		picture = gtk_image_get_pixbuf(GTK_IMAGE(textview->image));

	filename = addrindex_get_picture_file(msginfo->from);
	if (!filename)
		return;
	if (!is_file_exist(filename)) {
		gdk_pixbuf_save(picture, filename, "png", &error, NULL);
		if (error) {
			g_warning("Failed to save image: %s",
					error->message);
			g_error_free(error);
		}
	}
	g_free(filename);
#else
	/* new address book */
#endif
}