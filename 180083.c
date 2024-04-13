mono_type_get (const char *str)
{
	char *copy = g_strdup (str);
	MonoReflectionType *type = type_from_name (copy, FALSE);

	g_free (copy);
	return type;
}