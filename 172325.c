imapx_server_get_property (GObject *object,
                           guint property_id,
                           GValue *value,
                           GParamSpec *pspec)
{
	switch (property_id) {
		case PROP_STORE:
			g_value_take_object (
				value,
				camel_imapx_server_ref_store (
				CAMEL_IMAPX_SERVER (object)));
			return;
	}

	G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
}