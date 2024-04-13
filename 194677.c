static void add_uri_to_addrbook_cb (GtkAction *action, TextView *textview)
{
	gchar *fromname, *fromaddress;
	ClickableText *uri = g_object_get_data(G_OBJECT(textview->mail_popup_menu),
					   "menu_button");
	AvatarRender *avatarr = NULL;
	GdkPixbuf *picture = NULL;
	gboolean use_picture = FALSE;

	if (uri == NULL)
		return;

	/* extract url */
	fromaddress = g_strdup(uri->uri + 7);
	
	if (textview->messageview->msginfo &&
	   !g_strcmp0(fromaddress, textview->messageview->msginfo->from))
		use_picture = TRUE;

	fromname = procheader_get_fromname(fromaddress);
	extract_address(fromaddress);

	if (use_picture) {
		avatarr = avatars_avatarrender_new(textview->messageview->msginfo);
		hooks_invoke(AVATAR_IMAGE_RENDER_HOOKLIST, avatarr);
	}

	if (avatarr && avatarr->image) {
		picture = gtk_image_get_pixbuf(GTK_IMAGE(avatarr->image));
	}
	if (avatarr) {
		avatars_avatarrender_free(avatarr);
	}

#ifndef USE_ALT_ADDRBOOK
	addressbook_add_contact( fromname, fromaddress, NULL, picture);
#else
	if (addressadd_selection(fromname, fromaddress, NULL, picture)) {
		debug_print( "addressbook_add_contact - added\n" );
	}
#endif

	g_free(fromaddress);
	g_free(fromname);
}