e_enum_to_string (GType enum_type,
                  gint enum_value)
{
	GEnumClass *enum_class;
	const gchar *string = NULL;
	guint i;

	enum_class = g_type_class_ref (enum_type);

	g_return_val_if_fail (enum_class != NULL, NULL);

	for (i = 0; i < enum_class->n_values; i++) {
		if (enum_value == enum_class->values[i].value) {
			string = enum_class->values[i].value_nick;
			break;
		}
	}

	g_type_class_unref (enum_class);

	return string;
}