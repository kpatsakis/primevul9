e_util_identity_can_send (ESourceRegistry *registry,
			  ESource *identity_source)
{
	ESourceMailSubmission *mail_submission;
	ESource *transport_source = NULL;
	const gchar *transport_uid;
	gboolean can_send = FALSE;

	g_return_val_if_fail (E_IS_SOURCE_REGISTRY (registry), FALSE);
	g_return_val_if_fail (E_IS_SOURCE (identity_source), FALSE);

	if (!e_source_has_extension (identity_source, E_SOURCE_EXTENSION_MAIL_IDENTITY) ||
	    !e_source_has_extension (identity_source, E_SOURCE_EXTENSION_MAIL_SUBMISSION))
		return FALSE;

	mail_submission = e_source_get_extension (identity_source, E_SOURCE_EXTENSION_MAIL_SUBMISSION);

	e_source_extension_property_lock (E_SOURCE_EXTENSION (mail_submission));

	transport_uid = e_source_mail_submission_get_transport_uid (mail_submission);
	if (transport_uid && *transport_uid)
		transport_source = e_source_registry_ref_source (registry, transport_uid);

	e_source_extension_property_unlock (E_SOURCE_EXTENSION (mail_submission));

	if (!transport_source)
		return FALSE;

	if (e_source_has_extension (transport_source, E_SOURCE_EXTENSION_MAIL_TRANSPORT)) {
		ESourceMailTransport *mail_transport;
		const gchar *backend_name;

		mail_transport = e_source_get_extension (transport_source, E_SOURCE_EXTENSION_MAIL_TRANSPORT);

		e_source_extension_property_lock (E_SOURCE_EXTENSION (mail_transport));

		backend_name = e_source_backend_get_backend_name (E_SOURCE_BACKEND (mail_transport));
		can_send = backend_name && *backend_name && g_strcmp0 (backend_name, "none") != 0;

		e_source_extension_property_unlock (E_SOURCE_EXTENSION (mail_transport));
	}

	g_object_unref (transport_source);

	return can_send;
}