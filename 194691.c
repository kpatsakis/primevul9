static void scrolled_cb (GtkAdjustment *adj, TextView *textview)
{
#ifndef WIDTH
#  define WIDTH 48
#  define HEIGHT 48
#endif
	if (textview->image) {
		GtkAllocation allocation;
		gint x, y, x1;
		gtk_widget_get_allocation(textview->text, &allocation);
		x1 = allocation.width - WIDTH - 5;
		gtk_text_view_buffer_to_window_coords(
			GTK_TEXT_VIEW(textview->text),
			GTK_TEXT_WINDOW_TEXT, x1, 5, &x, &y);
		gtk_text_view_move_child(GTK_TEXT_VIEW(textview->text), 
			textview->image, x1, y);
	}
}