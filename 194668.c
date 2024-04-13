static void textview_show_avatar(TextView *textview)
{
	GtkAllocation allocation;
	GtkTextView *text = GTK_TEXT_VIEW(textview->text);
	MsgInfo *msginfo = textview->messageview->msginfo;
	int x = 0;
	AvatarRender *avatarr;
	
	if (prefs_common.display_header_pane || !prefs_common.display_xface)
		goto bail;
	
	avatarr = avatars_avatarrender_new(msginfo);
	hooks_invoke(AVATAR_IMAGE_RENDER_HOOKLIST, avatarr);

	if (!avatarr->image) {
		avatars_avatarrender_free(avatarr);
		goto bail;
	}

	if (textview->image) 
		gtk_widget_destroy(textview->image);
	
	textview->image = avatarr->image;
	textview->avatar_type = avatarr->type;
	avatarr->image = NULL; /* avoid destroying */
	avatars_avatarrender_free(avatarr);

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
}