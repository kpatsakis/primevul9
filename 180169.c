ves_icall_System_Reflection_Assembly_GetModulesInternal (MonoReflectionAssembly *assembly)
{
	MonoDomain *domain = mono_domain_get();
	MonoArray *res;
	MonoClass *klass;
	int i, j, file_count = 0;
	MonoImage **modules;
	guint32 module_count, real_module_count;
	MonoTableInfo *table;
	guint32 cols [MONO_FILE_SIZE];
	MonoImage *image = assembly->assembly->image;

	g_assert (image != NULL);
	g_assert (!assembly->assembly->dynamic);

	table = &image->tables [MONO_TABLE_FILE];
	file_count = table->rows;

	modules = image->modules;
	module_count = image->module_count;

	real_module_count = 0;
	for (i = 0; i < module_count; ++i)
		if (modules [i])
			real_module_count ++;

	klass = mono_class_from_name (mono_defaults.corlib, "System.Reflection", "Module");
	res = mono_array_new (domain, klass, 1 + real_module_count + file_count);

	mono_array_setref (res, 0, mono_module_get_object (domain, image));
	j = 1;
	for (i = 0; i < module_count; ++i)
		if (modules [i]) {
			mono_array_setref (res, j, mono_module_get_object (domain, modules[i]));
			++j;
		}

	for (i = 0; i < file_count; ++i, ++j) {
		mono_metadata_decode_row (table, i, cols, MONO_FILE_SIZE);
		if (cols [MONO_FILE_FLAGS] && FILE_CONTAINS_NO_METADATA)
			mono_array_setref (res, j, mono_module_file_get_object (domain, image, i));
		else {
			MonoImage *m = mono_image_load_file_for_image (image, i + 1);
			if (!m) {
				MonoString *fname = mono_string_new (mono_domain_get (), mono_metadata_string_heap (image, cols [MONO_FILE_NAME]));
				mono_raise_exception (mono_get_exception_file_not_found2 (NULL, fname));
			}
			mono_array_setref (res, j, mono_module_get_object (domain, m));
		}
	}

	return res;
}