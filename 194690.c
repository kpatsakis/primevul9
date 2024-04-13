void textview_show_mime_part(TextView *textview, MimeInfo *partinfo)
{
	GtkTextView *text;
	GtkTextBuffer *buffer;
	GtkTextIter iter;
	const gchar *name;
	gchar *content_type;
	GtkUIManager *ui_manager;
#ifndef GENERIC_UMPC
	gchar *shortcut;
#endif

	if (!partinfo) return;

	if (textview->messageview->window != NULL)
		ui_manager = textview->messageview->ui_manager;
	else
		ui_manager = textview->messageview->mainwin->ui_manager;

	textview_set_font(textview, NULL);
	textview_clear(textview);

	text = GTK_TEXT_VIEW(textview->text);
	buffer = gtk_text_view_get_buffer(text);
	gtk_text_buffer_get_start_iter(buffer, &iter);

	TEXTVIEW_INSERT("\n");

	name = procmime_mimeinfo_get_parameter(partinfo, "filename");
	if (name == NULL)
		name = procmime_mimeinfo_get_parameter(partinfo, "name");
	if (name != NULL) {
		content_type = procmime_get_content_type_str(partinfo->type,
						     partinfo->subtype);
		TEXTVIEW_INSERT("  ");
		TEXTVIEW_INSERT_BOLD(name);
		TEXTVIEW_INSERT(" (");
		TEXTVIEW_INSERT(content_type);
		TEXTVIEW_INSERT(", ");
		TEXTVIEW_INSERT(to_human_readable((goffset)partinfo->length));
		TEXTVIEW_INSERT("):\n\n");
		
		g_free(content_type);
	}
	TEXTVIEW_INSERT(_("  The following can be performed on this part\n"));
#ifndef GENERIC_UMPC
	TEXTVIEW_INSERT(_("  by right-clicking the icon or list item:"));
#endif
	TEXTVIEW_INSERT("\n");

	TEXTVIEW_INSERT(_("     - To save, select "));
	TEXTVIEW_INSERT_LINK(_("'Save as...'"), "sc://save_as", NULL);
#ifndef GENERIC_UMPC
	TEXTVIEW_INSERT(_(" (Shortcut key: '"));
	shortcut = cm_menu_item_get_shortcut(ui_manager, "Menu/File/SavePartAs");
	TEXTVIEW_INSERT(shortcut);
	g_free(shortcut);
	TEXTVIEW_INSERT("')");
#endif
	TEXTVIEW_INSERT("\n");

	TEXTVIEW_INSERT(_("     - To display as text, select "));
	TEXTVIEW_INSERT_LINK(_("'Display as text'"), "sc://display_as_text", NULL);

#ifndef GENERIC_UMPC
	TEXTVIEW_INSERT(_(" (Shortcut key: '"));
	shortcut = cm_menu_item_get_shortcut(ui_manager, "Menu/View/Part/AsText");
	TEXTVIEW_INSERT(shortcut);
	g_free(shortcut);
	TEXTVIEW_INSERT("')");
#endif
	TEXTVIEW_INSERT("\n");

	TEXTVIEW_INSERT(_("     - To open with an external program, select "));
	TEXTVIEW_INSERT_LINK(_("'Open'"), "sc://open", NULL);

#ifndef GENERIC_UMPC
	TEXTVIEW_INSERT(_(" (Shortcut key: '"));
	shortcut = cm_menu_item_get_shortcut(ui_manager, "Menu/View/Part/Open");
	TEXTVIEW_INSERT(shortcut);
	g_free(shortcut);
	TEXTVIEW_INSERT("')\n");
	TEXTVIEW_INSERT(_("       (alternately double-click, or click the middle "));
	TEXTVIEW_INSERT(_("mouse button)\n"));
#ifndef G_OS_WIN32
	TEXTVIEW_INSERT(_("     - Or use "));
	TEXTVIEW_INSERT_LINK(_("'Open with...'"), "sc://open_with", NULL);
	TEXTVIEW_INSERT(_(" (Shortcut key: '"));
	shortcut = cm_menu_item_get_shortcut(ui_manager, "Menu/View/Part/OpenWith");
	TEXTVIEW_INSERT(shortcut);
	g_free(shortcut);
	TEXTVIEW_INSERT("')");
#endif
#endif
	TEXTVIEW_INSERT("\n");

	textview_show_icon(textview, GTK_STOCK_DIALOG_INFO);
}