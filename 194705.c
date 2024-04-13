static gint textview_key_pressed(GtkWidget *widget, GdkEventKey *event,
				 TextView *textview)
{
	GdkWindow *window = NULL;
	SummaryView *summaryview = NULL;
	MessageView *messageview = textview->messageview;
	gboolean mod_pressed;

	if (!event) return FALSE;
	if (messageview->mainwin)
		summaryview = messageview->mainwin->summaryview;

	switch (event->keyval) {
	case GDK_KEY_Tab:
	case GDK_KEY_Left:
	case GDK_KEY_Up:
	case GDK_KEY_Right:
	case GDK_KEY_Down:
	case GDK_KEY_Control_L:
	case GDK_KEY_Control_R:
		return FALSE;
	case GDK_KEY_Home:
	case GDK_KEY_End:
		textview_scroll_max(textview,(event->keyval == GDK_KEY_Home));
		return TRUE;
	case GDK_KEY_space:
		mod_pressed = ((event->state & (GDK_SHIFT_MASK|GDK_MOD1_MASK)) != 0);
		if (!mimeview_scroll_page(messageview->mimeview, mod_pressed) &&
				summaryview != NULL) {
			if (mod_pressed)
				summary_select_prev_unread(summaryview);
			else
				summary_select_next_unread(summaryview);
		}
		break;
	case GDK_KEY_Page_Down:
		mimeview_scroll_page(messageview->mimeview, FALSE);
		break;
	case GDK_KEY_Page_Up:
	case GDK_KEY_BackSpace:
		mimeview_scroll_page(messageview->mimeview, TRUE);
		break;
	case GDK_KEY_Return:
	case GDK_KEY_KP_Enter:
		mimeview_scroll_one_line
			(messageview->mimeview, (event->state &
				    (GDK_SHIFT_MASK|GDK_MOD1_MASK)) != 0);
		break;
	case GDK_KEY_Delete:
		if (summaryview)
			summary_pass_key_press_event(summaryview, event);
		break;
	default:
		if (messageview->mainwin) {
			window = gtk_widget_get_window(messageview->mainwin->window);
			if (summaryview &&
			    event->window != window) {
				GdkEventKey tmpev = *event;

				tmpev.window = window;
				KEY_PRESS_EVENT_STOP();
				gtk_widget_event(messageview->mainwin->window,
						 (GdkEvent *)&tmpev);
			}
		}
		break;
	}

	return TRUE;
}