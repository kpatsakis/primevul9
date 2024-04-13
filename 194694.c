static void textview_add_part(TextView *textview, MimeInfo *mimeinfo)
{
	GtkAllocation allocation;
	GtkTextView *text;
	GtkTextBuffer *buffer;
	GtkTextIter iter, start_iter;
	gchar buf[BUFFSIZE];
	GPtrArray *headers = NULL;
	const gchar *name;
	gchar *content_type;
	gint charcount;

	START_TIMING("");

	cm_return_if_fail(mimeinfo != NULL);
	text = GTK_TEXT_VIEW(textview->text);
	buffer = gtk_text_view_get_buffer(text);
	charcount = gtk_text_buffer_get_char_count(buffer);
	gtk_text_buffer_get_end_iter(buffer, &iter);
	
	if (textview->stop_loading) {
		return;
	}
	if (mimeinfo->type == MIMETYPE_MULTIPART) {
		END_TIMING();
		return;
	}

	textview->prev_quote_level = -1;

	if ((mimeinfo->type == MIMETYPE_MESSAGE) && !g_ascii_strcasecmp(mimeinfo->subtype, "rfc822")) {
		FILE *fp;
		if (mimeinfo->content == MIMECONTENT_MEM)
			fp = str_open_as_stream(mimeinfo->data.mem);
		else
			fp = claws_fopen(mimeinfo->data.filename, "rb");
		if (!fp) {
			FILE_OP_ERROR(mimeinfo->data.filename, "claws_fopen");
			END_TIMING();
			return;
		}
		if (fseek(fp, mimeinfo->offset, SEEK_SET) < 0) {
			FILE_OP_ERROR(mimeinfo->data.filename, "fseek");
			claws_fclose(fp);
			END_TIMING();
			return;
		}
		headers = textview_scan_header(textview, fp);
		if (headers) {
			if (charcount > 0)
				gtk_text_buffer_insert(buffer, &iter, "\n", 1);
			
			if (procmime_mimeinfo_parent(mimeinfo) == NULL)
				textview_show_tags(textview);
			textview_show_header(textview, headers);
			procheader_header_array_destroy(headers);
		}
		claws_fclose(fp);
		END_TIMING();
		return;
	}

	name = procmime_mimeinfo_get_parameter(mimeinfo, "filename");
	content_type = procmime_get_content_type_str(mimeinfo->type,
						     mimeinfo->subtype);
	if (name == NULL)
		name = procmime_mimeinfo_get_parameter(mimeinfo, "name");
	if (name != NULL)
		g_snprintf(buf, sizeof(buf), _("[%s  %s (%d bytes)]"),
			   name, content_type, mimeinfo->length);
	else
		g_snprintf(buf, sizeof(buf), _("[%s (%d bytes)]"),
			   content_type, mimeinfo->length);

	g_free(content_type);			   

	if (mimeinfo->disposition == DISPOSITIONTYPE_ATTACHMENT
	|| (mimeinfo->disposition == DISPOSITIONTYPE_INLINE && 
	    mimeinfo->type != MIMETYPE_TEXT)) {
		gtk_text_buffer_insert(buffer, &iter, "\n", 1);
		TEXTVIEW_INSERT_LINK(buf, "sc://select_attachment", mimeinfo);
		gtk_text_buffer_insert(buffer, &iter, " \n", -1);
		if (mimeinfo->type == MIMETYPE_IMAGE  &&
		    prefs_common.inline_img ) {
			GdkPixbuf *pixbuf;
			GError *error = NULL;
			ClickableText *uri;

			START_TIMING("inserting image");

			pixbuf = procmime_get_part_as_pixbuf(mimeinfo, &error);
			if (error != NULL) {
				g_warning("Can't load the image: %s\n", error->message);
				g_error_free(error);
				END_TIMING();
				return;
			}

			if (textview->stop_loading) {
				END_TIMING();
				return;
			}

			gtk_widget_get_allocation(textview->scrolledwin, &allocation);
			pixbuf = claws_load_pixbuf_fitting(pixbuf, prefs_common.inline_img,
					prefs_common.fit_img_height, allocation.width,
					allocation.height);

			if (textview->stop_loading) {
				END_TIMING();
				return;
			}

			uri = g_new0(ClickableText, 1);
			uri->uri = g_strdup("");
			uri->filename = g_strdup("sc://select_attachment");
			uri->data = mimeinfo;

			uri->start = gtk_text_iter_get_offset(&iter);
			gtk_text_buffer_insert_pixbuf(buffer, &iter, pixbuf);
			g_object_unref(pixbuf);
			if (textview->stop_loading) {
				g_free(uri);
				return;
			}
			uri->end = gtk_text_iter_get_offset(&iter);

			textview->uri_list =
				g_slist_prepend(textview->uri_list, uri);

			gtk_text_buffer_insert(buffer, &iter, " ", 1);
			gtk_text_buffer_get_iter_at_offset(buffer, &start_iter, uri->start);
			gtk_text_buffer_apply_tag_by_name(buffer, "link",
						&start_iter, &iter);

			END_TIMING();
			GTK_EVENTS_FLUSH();
		}
	} else if (mimeinfo->type == MIMETYPE_TEXT) {
		if (prefs_common.display_header && (charcount > 0))
			gtk_text_buffer_insert(buffer, &iter, "\n", 1);

		if (!gtk_text_buffer_get_mark(buffer, "body_start")) {
			gtk_text_buffer_get_end_iter(buffer, &iter);
			gtk_text_buffer_create_mark(buffer, "body_start", &iter, TRUE);
		}

		textview_write_body(textview, mimeinfo);

		if (!gtk_text_buffer_get_mark(buffer, "body_end")) {
			gtk_text_buffer_get_end_iter(buffer, &iter);
			gtk_text_buffer_create_mark(buffer, "body_end", &iter, TRUE);
		}
	}
	END_TIMING();
}