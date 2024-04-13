static void textview_create_tags(GtkTextView *text, TextView *textview)
{
	GtkTextBuffer *buffer;
	GtkTextTag *tag, *qtag;
	static PangoFontDescription *font_desc, *bold_font_desc;
	
	if (!font_desc)
		font_desc = pango_font_description_from_string
			(NORMAL_FONT);

	if (!bold_font_desc) {
		if (prefs_common.derive_from_normal_font || !BOLD_FONT) {
			bold_font_desc = pango_font_description_from_string
				(NORMAL_FONT);
			pango_font_description_set_weight
				(bold_font_desc, PANGO_WEIGHT_BOLD);
		} else {
			bold_font_desc = pango_font_description_from_string
				(BOLD_FONT);
		}
	}

	buffer = gtk_text_view_get_buffer(text);

	gtk_text_buffer_create_tag(buffer, "header",
				   "pixels-above-lines", 0,
				   "pixels-above-lines-set", TRUE,
				   "pixels-below-lines", 0,
				   "pixels-below-lines-set", TRUE,
				   "font-desc", font_desc,
				   "left-margin", 3,
				   "left-margin-set", TRUE,
				   NULL);
	gtk_text_buffer_create_tag(buffer, "header_title",
				   "font-desc", bold_font_desc,
				   NULL);
	tag = gtk_text_buffer_create_tag(buffer, "hlink",
				   "pixels-above-lines", 0,
				   "pixels-above-lines-set", TRUE,
				   "pixels-below-lines", 0,
				   "pixels-below-lines-set", TRUE,
				   "font-desc", font_desc,
				   "left-margin", 3,
				   "left-margin-set", TRUE,
				   "foreground-gdk", &uri_color,
				   NULL);
	g_signal_connect(G_OBJECT(tag), "event",
                         G_CALLBACK(textview_uri_button_pressed), textview);
	if (prefs_common.enable_bgcolor) {
		gtk_text_buffer_create_tag(buffer, "quote0",
				"foreground-gdk", &quote_colors[0],
				"paragraph-background-gdk", &quote_bgcolors[0],
				NULL);
		gtk_text_buffer_create_tag(buffer, "quote1",
				"foreground-gdk", &quote_colors[1],
				"paragraph-background-gdk", &quote_bgcolors[1],
				NULL);
		gtk_text_buffer_create_tag(buffer, "quote2",
				"foreground-gdk", &quote_colors[2],
				"paragraph-background-gdk", &quote_bgcolors[2],
				NULL);
	} else {
		gtk_text_buffer_create_tag(buffer, "quote0",
				"foreground-gdk", &quote_colors[0],
				NULL);
		gtk_text_buffer_create_tag(buffer, "quote1",
				"foreground-gdk", &quote_colors[1],
				NULL);
		gtk_text_buffer_create_tag(buffer, "quote2",
				"foreground-gdk", &quote_colors[2],
				NULL);
	}
	gtk_text_buffer_create_tag(buffer, "tags",
			"foreground-gdk", &tags_color,
			"paragraph-background-gdk", &tags_bgcolor,
			NULL);
	gtk_text_buffer_create_tag(buffer, "emphasis",
			"foreground-gdk", &emphasis_color,
			NULL);
	gtk_text_buffer_create_tag(buffer, "signature",
			"foreground-gdk", &signature_color,
			NULL);
	tag = gtk_text_buffer_create_tag(buffer, "link",
			"foreground-gdk", &uri_color,
			NULL);
	qtag = gtk_text_buffer_create_tag(buffer, "qlink",
			NULL);
	gtk_text_buffer_create_tag(buffer, "link-hover",
			"underline", PANGO_UNDERLINE_SINGLE,
			NULL);
	gtk_text_buffer_create_tag(buffer, "diff-add",
			"foreground-gdk", &diff_added_color,
			NULL);
	gtk_text_buffer_create_tag(buffer, "diff-del",
			"foreground-gdk", &diff_deleted_color,
			NULL);
	gtk_text_buffer_create_tag(buffer, "diff-add-file",
			"foreground-gdk", &diff_added_color,
			"weight", PANGO_WEIGHT_BOLD,
			NULL);
	gtk_text_buffer_create_tag(buffer, "diff-del-file",
			"foreground-gdk", &diff_deleted_color,
			"weight", PANGO_WEIGHT_BOLD,
			NULL);
	gtk_text_buffer_create_tag(buffer, "diff-hunk",
			"foreground-gdk", &diff_hunk_color,
			"weight", PANGO_WEIGHT_BOLD,
			NULL);
	g_signal_connect(G_OBJECT(qtag), "event",
                         G_CALLBACK(textview_uri_button_pressed), textview);
	g_signal_connect(G_OBJECT(tag), "event",
                         G_CALLBACK(textview_uri_button_pressed), textview);
/*	if (font_desc)
		pango_font_description_free(font_desc);
	if (bold_font_desc)
		pango_font_description_free(bold_font_desc);*/
 }