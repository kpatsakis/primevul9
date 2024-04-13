soup_cookie_jar_request_unqueued (SoupSessionFeature *feature,
				  SoupSession *session,
				  SoupMessage *msg)
{
	g_signal_handlers_disconnect_by_func (msg, process_set_cookie_header, feature);
}