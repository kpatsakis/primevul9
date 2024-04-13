static void textview_make_clickable_parts_later(TextView *textview,
					  gint start, gint end)
{
	GtkTextView *text = GTK_TEXT_VIEW(textview->text);
	GtkTextBuffer *buffer = gtk_text_view_get_buffer(text);
	GtkTextIter start_iter, end_iter;
	gchar *mybuf;
	gint offset = 0;
	/* parse table - in order of priority */
	struct table {
		const gchar *needle; /* token */

		/* token search function */
		gchar    *(*search)	(const gchar *haystack,
					 const gchar *needle);
		/* part parsing function */
		gboolean  (*parse)	(const gchar *start,
					 const gchar *scanpos,
					 const gchar **bp_,
					 const gchar **ep_,
					 gboolean hdr);
		/* part to URI function */
		gchar    *(*build_uri)	(const gchar *bp,
					 const gchar *ep);
	};

	static struct table parser[] = {
		{"http://",  strcasestr, get_uri_part,   make_uri_string},
		{"https://", strcasestr, get_uri_part,   make_uri_string},
		{"ftp://",   strcasestr, get_uri_part,   make_uri_string},
		{"sftp://",  strcasestr, get_uri_part,   make_uri_string},
		{"www.",     strcasestr, get_uri_part,   make_http_string},
		{"mailto:",  strcasestr, get_uri_part,   make_uri_string},
		{"@",        strcasestr, get_email_part, make_email_string}
	};
	const gint PARSE_ELEMS = sizeof parser / sizeof parser[0];

	gint  n;
	const gchar *walk, *bp, *ep;

	struct txtpos {
		const gchar	*bp, *ep;	/* text position */
		gint		 pti;		/* index in parse table */
		struct txtpos	*next;		/* next */
	} head = {NULL, NULL, 0,  NULL}, *last = &head;

	gtk_text_buffer_get_iter_at_offset(buffer, &start_iter, start);
	gtk_text_buffer_get_iter_at_offset(buffer, &end_iter, end);
	mybuf = gtk_text_buffer_get_text(buffer, &start_iter, &end_iter, FALSE);
	offset = gtk_text_iter_get_offset(&start_iter);

	/* parse for clickable parts, and build a list of begin and end positions  */
	for (walk = mybuf;;) {
		gint last_index = PARSE_ELEMS;
		gchar *scanpos = NULL;

		/* FIXME: this looks phony. scanning for anything in the parse table */
		for (n = 0; n < PARSE_ELEMS; n++) {
			gchar *tmp;

			tmp = parser[n].search(walk, parser[n].needle);
			if (tmp) {
				if (scanpos == NULL || tmp < scanpos) {
					scanpos = tmp;
					last_index = n;
				}
			}					
		}

		if (scanpos) {
			/* check if URI can be parsed */
			if (parser[last_index].parse(walk, scanpos, &bp, &ep, FALSE)
			    && (size_t) (ep - bp - 1) > strlen(parser[last_index].needle)) {
					ADD_TXT_POS_LATER(bp, ep, last_index);
					walk = ep;
			} else
				walk = scanpos +
					strlen(parser[last_index].needle);
		} else
			break;
	}

	/* colorize this line */
	if (head.next) {
		/* insert URIs */
		for (last = head.next; last != NULL; last = last->next) {
			ClickableText *uri;
			gint start_offset, end_offset;
			gchar *tmp_str;
			gchar old_char;
			uri = g_new0(ClickableText, 1);
			uri->uri = parser[last->pti].build_uri(last->bp,
							       last->ep);
			
			tmp_str = mybuf;
			old_char = tmp_str[last->ep - mybuf];
			tmp_str[last->ep - mybuf] = '\0';				       
			end_offset = g_utf8_strlen(tmp_str, -1);
			tmp_str[last->ep - mybuf] = old_char;
			
			old_char = tmp_str[last->bp - mybuf];
			tmp_str[last->bp - mybuf] = '\0';				       
			start_offset = g_utf8_strlen(tmp_str, -1);
			tmp_str[last->bp - mybuf] = old_char;
			
			gtk_text_buffer_get_iter_at_offset(buffer, &start_iter, start_offset + offset);
			gtk_text_buffer_get_iter_at_offset(buffer, &end_iter, end_offset + offset);
			
			uri->start = gtk_text_iter_get_offset(&start_iter);
			
			gtk_text_buffer_apply_tag_by_name(buffer, "link", &start_iter, &end_iter);

			uri->end = gtk_text_iter_get_offset(&end_iter);
			uri->filename = NULL;
			textview->uri_list =
				g_slist_prepend(textview->uri_list, uri);
		}
	} 

	g_free(mybuf);
}