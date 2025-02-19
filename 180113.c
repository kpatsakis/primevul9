ves_icall_System_Reflection_Assembly_GetNamespaces (MonoReflectionAssembly *assembly) 
{
	MonoImage *img = assembly->assembly->image;
	MonoArray *res;
	NameSpaceInfo info;
	int len;

	MONO_ARCH_SAVE_REGS;

	mono_image_lock (img);
	mono_image_init_name_cache (img);

RETRY_LEN:
	len = g_hash_table_size (img->name_cache);
	mono_image_unlock (img);

	/*we can't create objects holding the image lock */
	res = mono_array_new (mono_object_domain (assembly), mono_defaults.string_class, len);

	mono_image_lock (img);
	/*len might have changed, create a new array*/
	if (len != g_hash_table_size (img->name_cache))
		goto RETRY_LEN;

	info.res = res;
	info.idx = 0;
	g_hash_table_foreach (img->name_cache, (GHFunc)foreach_namespace, &info);
	mono_image_unlock (img);

	return res;
}