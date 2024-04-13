void textview_get_selection_offsets(TextView *textview, gint *sel_start, gint *sel_end)
{
		GtkTextView *text = GTK_TEXT_VIEW(textview->text);
		GtkTextBuffer *buffer = gtk_text_view_get_buffer(text);
		GtkTextIter start, end;
		if (gtk_text_buffer_get_selection_bounds(buffer, &start, &end)) {
			if (sel_start)
				*sel_start = gtk_text_iter_get_offset(&start);
			if (sel_end)
				*sel_end = gtk_text_iter_get_offset(&end);
		} else {
			if (sel_start)
				*sel_start = -1;
			if (sel_end)
				*sel_end = -1;
		}
}