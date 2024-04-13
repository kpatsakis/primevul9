static void textview_make_clickable_parts(TextView *textview,
					  const gchar *fg_tag,
					  const gchar *uri_tag,
					  const gchar *linebuf,
					  gboolean hdr)
{
	GtkTextView *text = GTK_TEXT_VIEW(textview->text);
	GtkTextBuffer *buffer = gtk_text_view_get_buffer(text);
	GtkTextIter iter;
	gchar *mybuf = g_strdup(linebuf);
	
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
		{"gopher://",strcasestr, get_uri_part,   make_uri_string},
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

	if (!g_utf8_validate(linebuf, -1, NULL)) {
		g_free(mybuf);
		mybuf = g_malloc(strlen(linebuf)*2 +1);
		conv_localetodisp(mybuf, strlen(linebuf)*2 +1, linebuf);
	}

	gtk_text_buffer_get_end_iter(buffer, &iter);

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
			if (parser[last_index].parse(walk, scanpos, &bp, &ep, hdr)
			    && (size_t) (ep - bp - 1) > strlen(parser[last_index].needle)) {
					ADD_TXT_POS(bp, ep, last_index);
					walk = ep;
			} else
				walk = scanpos +
					strlen(parser[last_index].needle);
		} else
			break;
	}

	/* colorize this line */
	if (head.next) {
		const gchar *normal_text = mybuf;

		/* insert URIs */
		for (last = head.next; last != NULL;
		     normal_text = last->ep, last = last->next) {
			ClickableText *uri;
			uri = g_new0(ClickableText, 1);
			if (last->bp - normal_text > 0)
				gtk_text_buffer_insert_with_tags_by_name
					(buffer, &iter,
					 normal_text,
					 last->bp - normal_text,
					 fg_tag, NULL);
			uri->uri = parser[last->pti].build_uri(last->bp,
							       last->ep);
			uri->start = gtk_text_iter_get_offset(&iter);
			gtk_text_buffer_insert_with_tags_by_name
				(buffer, &iter, last->bp, last->ep - last->bp,
				 uri_tag, fg_tag, NULL);
			uri->end = gtk_text_iter_get_offset(&iter);
			uri->filename = NULL;
			textview->uri_list =
				g_slist_prepend(textview->uri_list, uri);
		}

		if (*normal_text)
			gtk_text_buffer_insert_with_tags_by_name
				(buffer, &iter, normal_text, -1, fg_tag, NULL);
	} else {
		gtk_text_buffer_insert_with_tags_by_name
			(buffer, &iter, mybuf, -1, fg_tag, NULL);
	}
	g_free(mybuf);
}