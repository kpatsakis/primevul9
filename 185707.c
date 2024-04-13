soup_cookie_jar_request_queued (SoupSessionFeature *feature,
				SoupSession *session,
				SoupMessage *msg)
{
	g_signal_connect (msg, "starting",
			  G_CALLBACK (msg_starting_cb),
			  feature);

	soup_message_add_header_handler (msg, "got-headers",
					 "Set-Cookie",
					 G_CALLBACK (process_set_cookie_header),
					 feature);
}