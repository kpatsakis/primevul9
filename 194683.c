static GPtrArray *textview_scan_header(TextView *textview, FILE *fp)
{
	gchar buf[BUFFSIZE];
	GPtrArray *headers, *sorted_headers;
	GSList *disphdr_list;
	Header *header;
	gint i;

	cm_return_val_if_fail(fp != NULL, NULL);

	if (prefs_common.show_all_headers) {
		headers = procheader_get_header_array_asis(fp);
		sorted_headers = g_ptr_array_new();
		for (i = 0; i < headers->len; i++) {
			header = g_ptr_array_index(headers, i);
			if (!procheader_header_is_internal(header->name))
				g_ptr_array_add(sorted_headers, header);
			else
				procheader_header_free(header);
		}
		g_ptr_array_free(headers, TRUE);
		return sorted_headers;
	}

	if (!prefs_common.display_header) {
		while (claws_fgets(buf, sizeof(buf), fp) != NULL)
			if (buf[0] == '\r' || buf[0] == '\n') break;
		return NULL;
	}

	headers = procheader_get_header_array_asis(fp);

	sorted_headers = g_ptr_array_new();

	for (disphdr_list = prefs_common.disphdr_list; disphdr_list != NULL;
	     disphdr_list = disphdr_list->next) {
		DisplayHeaderProp *dp =
			(DisplayHeaderProp *)disphdr_list->data;

		for (i = 0; i < headers->len; i++) {
			header = g_ptr_array_index(headers, i);

			if (procheader_headername_equal(header->name,
							dp->name)) {
				if (dp->hidden)
					procheader_header_free(header);
				else
					g_ptr_array_add(sorted_headers, header);

				g_ptr_array_remove_index(headers, i);
				i--;
			}
		}
	}

	if (prefs_common.show_other_header) {
		for (i = 0; i < headers->len; i++) {
			header = g_ptr_array_index(headers, i);
			if (!procheader_header_is_internal(header->name)) {
				g_ptr_array_add(sorted_headers, header);
			} else {
				procheader_header_free(header);
			}
		}
		g_ptr_array_free(headers, TRUE);
	} else
		procheader_header_array_destroy(headers);


	return sorted_headers;
}