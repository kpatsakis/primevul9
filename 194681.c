static gboolean textview_get_uri_range(TextView *textview,
				       GtkTextIter *iter,
				       GtkTextTag *tag,
				       GtkTextIter *start_iter,
				       GtkTextIter *end_iter)
{
	return get_tag_range(iter, tag, start_iter, end_iter);
}