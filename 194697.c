static void textview_write_line(TextView *textview, const gchar *str,
				CodeConverter *conv, gboolean do_quote_folding)
{
	GtkTextView *text;
	GtkTextBuffer *buffer;
	GtkTextIter iter;
	gchar buf[BUFFSIZE];
	gchar *fg_color;
	gint quotelevel = -1, real_quotelevel = -1;
	gchar quote_tag_str[10];

	text = GTK_TEXT_VIEW(textview->text);
	buffer = gtk_text_view_get_buffer(text);
	gtk_text_buffer_get_end_iter(buffer, &iter);

	if (!conv)
		strncpy2(buf, str, sizeof(buf));
	else if (conv_convert(conv, buf, sizeof(buf), str) < 0)
		conv_localetodisp(buf, sizeof(buf), str);
		
	strcrchomp(buf);
	fg_color = NULL;

	/* change color of quotation
	   >, foo>, _> ... ok, <foo>, foo bar>, foo-> ... ng
	   Up to 3 levels of quotations are detected, and each
	   level is colored using a different color. */
	if (prefs_common.enable_color
	    && !textview->is_attachment
	    && line_has_quote_char(buf, prefs_common.quote_chars)) {
		real_quotelevel = get_quote_level(buf, prefs_common.quote_chars);
		quotelevel = real_quotelevel;
		/* set up the correct foreground color */
		if (quotelevel > 2) {
			/* recycle colors */
			if (prefs_common.recycle_quote_colors)
				quotelevel %= 3;
			else
				quotelevel = 2;
		}
	}

	if (quotelevel == -1)
		fg_color = NULL;
	else {
		g_snprintf(quote_tag_str, sizeof(quote_tag_str),
			   "quote%d", quotelevel);
		fg_color = quote_tag_str;
	}

	if (prefs_common.enable_color) {
		if (textview->is_diff || textview->is_in_git_patch) {
			if (strncmp(buf, "+++ ", 4) == 0)
				fg_color = "diff-add-file";
			else if (buf[0] == '+')
				fg_color = "diff-add";
			else if (strncmp(buf, "--- ", 4) == 0)
				fg_color = "diff-del-file";
			else if (buf[0] == '-')
				fg_color = "diff-del";
			else if (strncmp(buf, "@@ ", 3) == 0 &&
				 strstr(&buf[3], " @@"))
				fg_color = "diff-hunk";

			if (account_sigsep_matchlist_nchar_found(buf, "%s\n")) {
				textview->is_in_git_patch = FALSE;
				textview->is_in_signature = TRUE;
				fg_color = "signature";
			}
		} else if (account_sigsep_matchlist_str_found(buf, "%s\n")
				|| account_sigsep_matchlist_str_found(buf, "- %s\n")
				|| textview->is_in_signature) {
			fg_color = "signature";
			textview->is_in_signature = TRUE;
		} else if (strncmp(buf, "diff --git ", 11) == 0) {
			textview->is_in_git_patch = TRUE;
		}
	}

	if (!textview->is_attachment && real_quotelevel > -1 && do_quote_folding) {
		if (!g_utf8_validate(buf, -1, NULL)) {
			gchar *utf8buf = NULL;
			utf8buf = g_malloc(BUFFSIZE);
			conv_localetodisp(utf8buf, BUFFSIZE, buf);
			strncpy2(buf, utf8buf, BUFFSIZE-1);
			g_free(utf8buf);
		}
do_quote:
		if ( textview->prev_quote_level != real_quotelevel ) {
			ClickableText *uri;
			uri = g_new0(ClickableText, 1);
			uri->uri = g_strdup("");
			uri->data = g_strdup(buf);
			uri->data_len = strlen(uri->data);
			uri->start = gtk_text_iter_get_offset(&iter);
			uri->is_quote = TRUE;
			uri->quote_level = real_quotelevel;
			uri->fg_color = g_strdup(fg_color);

			gtk_text_buffer_insert_with_tags_by_name
					(buffer, &iter, " [...]", -1,
					 "qlink", fg_color, NULL);
			uri->end = gtk_text_iter_get_offset(&iter);
			gtk_text_buffer_insert(buffer, &iter, "  \n", -1);
			
			uri->filename = NULL;
			textview->uri_list =
				g_slist_prepend(textview->uri_list, uri);
		
			textview->prev_quote_level = real_quotelevel;
		} else {
			GSList *last = textview->uri_list;
			ClickableText *lasturi = NULL;
			gint e_len = 0, n_len = 0;
			
			if (textview->uri_list) {
				lasturi = (ClickableText *)last->data;
			} else {
				g_print("oops (%d %d)\n",
					real_quotelevel, textview->prev_quote_level);
			}	
			if (lasturi) {	
				if (lasturi->is_quote == FALSE) {
					textview->prev_quote_level = -1;
					goto do_quote;
				}
				e_len = lasturi->data ? lasturi->data_len:0;
				n_len = strlen(buf);
				lasturi->data = g_realloc((gchar *)lasturi->data, e_len + n_len + 1);
				strcpy((gchar *)lasturi->data + e_len, buf);
				*((gchar *)lasturi->data + e_len + n_len) = '\0';
				lasturi->data_len += n_len;
			}
		}
	} else {
		textview_make_clickable_parts(textview, fg_color, "link", buf, FALSE);
		textview->prev_quote_level = -1;
	}
}