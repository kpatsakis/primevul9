static void textview_show_contact_pic(TextView *textview)
{
#ifndef USE_ALT_ADDRBOOK
	MsgInfo *msginfo = textview->messageview->msginfo;
	GtkTextView *text = GTK_TEXT_VIEW(textview->text);
	int x = 0;
	gchar *filename = NULL;
	GError *error = NULL;
	GdkPixbuf *picture = NULL;
	gint w, h;
	GtkAllocation allocation;

	if (prefs_common.display_header_pane
		|| !prefs_common.display_xface)
		goto bail;
	
	if (msginfo->extradata && msginfo->extradata->avatars)
		return;

	if (textview->image) 
		gtk_widget_destroy(textview->image);

	filename = addrindex_get_picture_file(msginfo->from);
	
	if (!filename)
		goto bail;
	if (!is_file_exist(filename)) {
		g_free(filename);
		goto bail;
	}

	gdk_pixbuf_get_file_info(filename, &w, &h);
	
	if (w > 48 || h > 48)
		picture = gdk_pixbuf_new_from_file_at_scale(filename, 
						48, 48, TRUE, &error);
	else
		picture = gdk_pixbuf_new_from_file(filename, &error);

	if (error) {
		debug_print("Failed to import image: %s\n",
				error->message);
		g_error_free(error);
		goto bail;
	}
	g_free(filename);

	if (picture) {
		textview->image = gtk_image_new_from_pixbuf(picture);
		g_object_unref(picture);
	}
	cm_return_if_fail(textview->image != NULL);

	gtk_widget_show(textview->image);
	
	gtk_widget_get_allocation(textview->text, &allocation);
	x = allocation.width - WIDTH -5;

	gtk_text_view_add_child_in_window(text, textview->image, 
		GTK_TEXT_WINDOW_TEXT, x, 5);

	gtk_widget_show_all(textview->text);
	
	return;
bail:
	if (textview->image) 
		gtk_widget_destroy(textview->image);
	textview->image = NULL;
	textview->avatar_type = 0;
#else
	/* new address book */
#endif	
}