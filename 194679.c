void textview_show_icon(TextView *textview, const gchar *stock_id)
{
	GtkAllocation allocation;
	GtkTextView *text = GTK_TEXT_VIEW(textview->text);
	int x = 0;
	
	if (textview->image) 
		gtk_widget_destroy(textview->image);
	
	textview->image = gtk_image_new_from_stock(stock_id, GTK_ICON_SIZE_DIALOG);
	cm_return_if_fail(textview->image != NULL);

	gtk_widget_show(textview->image);
	
	gtk_widget_get_allocation(textview->text, &allocation);
	x = allocation.width - WIDTH -5;

	gtk_text_view_add_child_in_window(text, textview->image, 
		GTK_TEXT_WINDOW_TEXT, x, 5);

	gtk_widget_show_all(textview->text);
	

	return;
}