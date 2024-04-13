static void reply_to_uri_cb (GtkAction *action, TextView *textview)
{
	ClickableText *uri = g_object_get_data(G_OBJECT(textview->mail_popup_menu),
					   "menu_button");
	if (!textview->messageview || !uri)
		return;

	compose_reply_to_address (textview->messageview,
				  textview->messageview->msginfo, uri->uri+7);
}