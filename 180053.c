property_hash (gconstpointer data)
{
	MonoProperty *prop = (MonoProperty*)data;

	return g_str_hash (prop->name);
}