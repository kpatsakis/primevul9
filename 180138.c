foreach_namespace (const char* key, gconstpointer val, NameSpaceInfo *info)
{
	MonoString *name = mono_string_new (mono_object_domain (info->res), key);

	mono_array_setref (info->res, info->idx, name);
	info->idx++;
}