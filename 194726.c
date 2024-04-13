static ClickableText *textview_get_uri_from_range(TextView *textview,
					      GtkTextIter *iter,
					      GtkTextTag *tag,
					      GtkTextIter *start_iter,
					      GtkTextIter *end_iter)
{
	gint start_pos, end_pos, cur_pos;
	ClickableText *uri = NULL;
	GSList *cur;

	start_pos = gtk_text_iter_get_offset(start_iter);
	end_pos = gtk_text_iter_get_offset(end_iter);
	cur_pos = gtk_text_iter_get_offset(iter);

	for (cur = textview->uri_list; cur != NULL; cur = cur->next) {
		ClickableText *uri_ = (ClickableText *)cur->data;
		if (start_pos == uri_->start &&
		    end_pos ==  uri_->end) {
			uri = uri_;
			break;
		} 
	}
	for (cur = textview->uri_list; uri == NULL && cur != NULL; cur = cur->next) {
		ClickableText *uri_ = (ClickableText *)cur->data;
		if (start_pos == uri_->start ||
			   end_pos == uri_->end) {
			/* in case of contiguous links, textview_get_uri_range
			 * returns a broader range (start of 1st link to end
			 * of last link).
			 * In that case, correct link is the one covering
			 * current iter.
			 */
			if (uri_->start <= cur_pos && cur_pos <= uri_->end) {
				uri = uri_;
				break;
			}
		} 
	}

	return uri;
}