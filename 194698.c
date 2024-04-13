void textview_set_font(TextView *textview, const gchar *codeset)
{
	GtkTextTag *tag;
	GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview->text));
	GtkTextTagTable *tags = gtk_text_buffer_get_tag_table(buffer);
	PangoFontDescription *font_desc, *bold_font_desc;

	font_desc = pango_font_description_from_string
					(NORMAL_FONT);
	if (font_desc) {
		gtk_widget_modify_font(textview->text, font_desc);
		CHANGE_TAG_FONT("header", font_desc);
		CHANGE_TAG_FONT("hlink", font_desc);
		pango_font_description_free(font_desc);
	}
	if (prefs_common.derive_from_normal_font || !BOLD_FONT) {
		bold_font_desc = pango_font_description_from_string
						(NORMAL_FONT);
		if (bold_font_desc)
			pango_font_description_set_weight
				(bold_font_desc, PANGO_WEIGHT_BOLD);
	} else {
		bold_font_desc = pango_font_description_from_string
						(BOLD_FONT);
	}
	if (bold_font_desc) {
		CHANGE_TAG_FONT("header_title", bold_font_desc);
		pango_font_description_free(bold_font_desc);
	}

	if (prefs_common.textfont) {
		PangoFontDescription *font_desc;

		font_desc = pango_font_description_from_string
						(prefs_common.textfont);
		if (font_desc) {
			gtk_widget_modify_font(textview->text, font_desc);
			pango_font_description_free(font_desc);
		}
	}
	gtk_text_view_set_pixels_above_lines(GTK_TEXT_VIEW(textview->text),
					     prefs_common.line_space / 2);
	gtk_text_view_set_pixels_below_lines(GTK_TEXT_VIEW(textview->text),
					     prefs_common.line_space / 2);
}