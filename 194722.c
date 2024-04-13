static ClickableText *textview_get_uri(TextView *textview,
				   GtkTextIter *iter,
				   GtkTextTag *tag)
{
	GtkTextIter start_iter, end_iter;
	ClickableText *uri = NULL;

	if (textview_get_uri_range(textview, iter, tag, &start_iter,
				   &end_iter))
		uri = textview_get_uri_from_range(textview, iter, tag,
						  &start_iter, &end_iter);

	return uri;
}