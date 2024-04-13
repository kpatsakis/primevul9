static void textview_show_header(TextView *textview, GPtrArray *headers)
{
	GtkTextView *text = GTK_TEXT_VIEW(textview->text);
	GtkTextBuffer *buffer = gtk_text_view_get_buffer(text);
	GtkTextIter iter;
	Header *header;
	gint i;

	cm_return_if_fail(headers != NULL);

	for (i = 0; i < headers->len; i++) {
		header = g_ptr_array_index(headers, i);
		cm_return_if_fail(header->name != NULL);

		gtk_text_buffer_get_end_iter (buffer, &iter);
		if(prefs_common.trans_hdr == TRUE) {
			gchar *hdr = g_strndup(header->name, strlen(header->name) - 1);
			gchar *trans_hdr = gettext(hdr);
			gtk_text_buffer_insert_with_tags_by_name(buffer,
				&iter, trans_hdr, -1,
				"header_title", "header", NULL);
			gtk_text_buffer_insert_with_tags_by_name(buffer,
				&iter, ":", 1, "header_title", "header", NULL);
			g_free(hdr);
		} else {
			gtk_text_buffer_insert_with_tags_by_name(buffer,
				&iter, header->name,
				-1, "header_title", "header", NULL);
		}
		if (header->name[strlen(header->name) - 1] != ' ')
		gtk_text_buffer_insert_with_tags_by_name
				(buffer, &iter, " ", 1,
				 "header_title", "header", NULL);

		if (procheader_headername_equal(header->name, "Subject") ||
		    procheader_headername_equal(header->name, "From") ||
		    procheader_headername_equal(header->name, "To") ||
		    procheader_headername_equal(header->name, "Cc") ||
		    procheader_headername_equal(header->name, "Bcc") ||
		    procheader_headername_equal(header->name, "Reply-To") ||
		    procheader_headername_equal(header->name, "Sender") ||
		    procheader_headername_equal(header->name, "Resent-From") ||
		    procheader_headername_equal(header->name, "Resent-To"))
			unfold_line(header->body);
		
		if (procheader_headername_equal(header->name, "Date") &&
		    prefs_common.msgview_date_format) {
			gchar hbody[80];
			
			procheader_date_parse(hbody, header->body, sizeof(hbody));
			gtk_text_buffer_get_end_iter (buffer, &iter);
			gtk_text_buffer_insert_with_tags_by_name
				(buffer, &iter, hbody, -1, "header", NULL);
		} else if ((procheader_headername_equal(header->name, "X-Mailer") ||
				procheader_headername_equal(header->name,
						 "X-Newsreader")) &&
				(strstr(header->body, "Claws Mail") != NULL ||
				strstr(header->body, "Sylpheed-Claws") != NULL)) {
			gtk_text_buffer_get_end_iter (buffer, &iter);
			gtk_text_buffer_insert_with_tags_by_name
				(buffer, &iter, header->body, -1,
				 "header", "emphasis", NULL);
		} else {
			gboolean hdr = 
			  procheader_headername_equal(header->name, "From") ||
			  procheader_headername_equal(header->name, "To") ||
			  procheader_headername_equal(header->name, "Cc") ||
			  procheader_headername_equal(header->name, "Bcc") ||
			  procheader_headername_equal(header->name, "Reply-To") ||
			  procheader_headername_equal(header->name, "Sender") ||
			  procheader_headername_equal(header->name, "Resent-From") ||
			  procheader_headername_equal(header->name, "Resent-To");
			textview_make_clickable_parts(textview, "header", 
						      "hlink", header->body, 
						      hdr);
		}
		gtk_text_buffer_get_end_iter (buffer, &iter);
		gtk_text_buffer_insert_with_tags_by_name(buffer, &iter, "\n", 1,
							 "header", NULL);
	}
	
	textview_show_avatar(textview);
	if (prefs_common.save_xface)
		textview_save_contact_pic(textview);
	textview_show_contact_pic(textview);
}