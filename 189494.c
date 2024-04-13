e_enum_from_string (GType enum_type,
                    const gchar *string,
                    gint *enum_value)
{
	GEnumClass *enum_class;
	GEnumValue *ev;
	gchar *endptr;
	gint value;
	gboolean retval = TRUE;

	g_return_val_if_fail (G_TYPE_IS_ENUM (enum_type), FALSE);
	g_return_val_if_fail (string != NULL, FALSE);

	value = g_ascii_strtoull (string, &endptr, 0);
	if (endptr != string)
		/* parsed a number */
		*enum_value = value;
	else {
		enum_class = g_type_class_ref (enum_type);
		ev = g_enum_get_value_by_name (enum_class, string);
		if (!ev)
			ev = g_enum_get_value_by_nick (enum_class, string);

		if (ev)
			*enum_value = ev->value;
		else
			retval = FALSE;

		g_type_class_unref (enum_class);
	}

	return retval;
}