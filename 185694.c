soup_cookie_jar_session_feature_init (SoupSessionFeatureInterface *feature_interface,
				      gpointer interface_data)
{
	feature_interface->request_queued = soup_cookie_jar_request_queued;
	feature_interface->request_unqueued = soup_cookie_jar_request_unqueued;
}