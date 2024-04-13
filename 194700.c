static void textview_update_message_colors(TextView *textview)
{
	GdkColor black = {0, 0, 0, 0};
	GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview->text));

	GtkTextTagTable *tags = gtk_text_buffer_get_tag_table(buffer);
	GtkTextTag *tag = NULL;

	quote_bgcolors[0] = quote_bgcolors[1] = quote_bgcolors[2] = black;
	quote_colors[0] = quote_colors[1] = quote_colors[2] = black;
	uri_color = emphasis_color = signature_color = diff_added_color =
		diff_deleted_color = diff_hunk_color = black;
	tags_bgcolor = tags_color = black;

	if (prefs_common.enable_color) {
		/* grab the quote colors, converting from an int to a GdkColor */
		gtkut_convert_int_to_gdk_color(prefs_common.color[COL_QUOTE_LEVEL1],
					       &quote_colors[0]);
		gtkut_convert_int_to_gdk_color(prefs_common.color[COL_QUOTE_LEVEL2],
					       &quote_colors[1]);
		gtkut_convert_int_to_gdk_color(prefs_common.color[COL_QUOTE_LEVEL3],
					       &quote_colors[2]);
		gtkut_convert_int_to_gdk_color(prefs_common.color[COL_URI],
					       &uri_color);
		gtkut_convert_int_to_gdk_color(prefs_common.color[COL_SIGNATURE],
					       &signature_color);
		gtkut_convert_int_to_gdk_color(prefs_common.color[COL_EMPHASIS],
					       &emphasis_color);
		gtkut_convert_int_to_gdk_color(prefs_common.color[COL_DIFF_ADDED],
					       &diff_added_color);
		gtkut_convert_int_to_gdk_color(prefs_common.color[COL_DIFF_DELETED],
					       &diff_deleted_color);
		gtkut_convert_int_to_gdk_color(prefs_common.color[COL_DIFF_HUNK],
					       &diff_hunk_color);
	}
	if (prefs_common.enable_color && prefs_common.enable_bgcolor) {
		gtkut_convert_int_to_gdk_color(prefs_common.color[COL_QUOTE_LEVEL1_BG],
						   &quote_bgcolors[0]);
		gtkut_convert_int_to_gdk_color(prefs_common.color[COL_QUOTE_LEVEL2_BG],
						   &quote_bgcolors[1]);
		gtkut_convert_int_to_gdk_color(prefs_common.color[COL_QUOTE_LEVEL3_BG],
						   &quote_bgcolors[2]);
		CHANGE_TAG_COLOR("quote0", &quote_colors[0], &quote_bgcolors[0]);
		CHANGE_TAG_COLOR("quote1", &quote_colors[1], &quote_bgcolors[1]);
		CHANGE_TAG_COLOR("quote2", &quote_colors[2], &quote_bgcolors[2]);
	} else {
		CHANGE_TAG_COLOR("quote0", &quote_colors[0], NULL);
		CHANGE_TAG_COLOR("quote1", &quote_colors[1], NULL);
		CHANGE_TAG_COLOR("quote2", &quote_colors[2], NULL);
	}

	CHANGE_TAG_COLOR("emphasis", &emphasis_color, NULL);
	CHANGE_TAG_COLOR("signature", &signature_color, NULL);
	CHANGE_TAG_COLOR("link", &uri_color, NULL);
	CHANGE_TAG_COLOR("link-hover", &uri_color, NULL);
	CHANGE_TAG_COLOR("diff-add", &diff_added_color, NULL);
	CHANGE_TAG_COLOR("diff-del", &diff_deleted_color, NULL);
	CHANGE_TAG_COLOR("diff-add-file", &diff_added_color, NULL);
	CHANGE_TAG_COLOR("diff-del-file", &diff_deleted_color, NULL);
	CHANGE_TAG_COLOR("diff-hunk", &diff_hunk_color, NULL);

	gtkut_convert_int_to_gdk_color(prefs_common.color[COL_TAGS_BG],
					   &tags_bgcolor);
	gtkut_convert_int_to_gdk_color(prefs_common.color[COL_TAGS],
					   &tags_color);
}