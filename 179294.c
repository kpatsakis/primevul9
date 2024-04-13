camel_imapx_server_class_init (CamelIMAPXServerClass *class)
{
	GObjectClass *object_class;

	g_type_class_add_private (class, sizeof (CamelIMAPXServerPrivate));

	object_class = G_OBJECT_CLASS (class);
	object_class->set_property = imapx_server_set_property;
	object_class->get_property = imapx_server_get_property;
	object_class->finalize = imapx_server_finalize;
	object_class->dispose = imapx_server_dispose;
	object_class->constructed = imapx_server_constructed;

	g_object_class_install_property (
		object_class,
		PROP_STORE,
		g_param_spec_object (
			"store",
			"Store",
			"IMAPX store for this server",
			CAMEL_TYPE_IMAPX_STORE,
			G_PARAM_READWRITE |
			G_PARAM_CONSTRUCT_ONLY |
			G_PARAM_STATIC_STRINGS));

	signals[REFRESH_MAILBOX] = g_signal_new (
		"refresh-mailbox",
		G_OBJECT_CLASS_TYPE (class),
		G_SIGNAL_RUN_LAST,
		G_STRUCT_OFFSET (CamelIMAPXServerClass, refresh_mailbox),
		NULL, NULL, NULL,
		G_TYPE_NONE, 1,
		CAMEL_TYPE_IMAPX_MAILBOX);
}