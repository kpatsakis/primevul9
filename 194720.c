static void textview_size_allocate_cb	(GtkWidget	*widget,
					 GtkAllocation	*allocation,
					 gpointer	 data)
{
	scrolled_cb(NULL, (TextView *)data);
}