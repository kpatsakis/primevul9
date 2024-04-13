ves_icall_System_Reflection_Assembly_GetFilesInternal (MonoReflectionAssembly *assembly, MonoString *name, MonoBoolean resource_modules) 
{
	MonoTableInfo *table = &assembly->assembly->image->tables [MONO_TABLE_FILE];
	MonoArray *result = NULL;
	int i, count;
	const char *val;
	char *n;

	MONO_ARCH_SAVE_REGS;

	/* check hash if needed */
	if (name) {
		n = mono_string_to_utf8 (name);
		for (i = 0; i < table->rows; ++i) {
			val = mono_metadata_string_heap (assembly->assembly->image, mono_metadata_decode_row_col (table, i, MONO_FILE_NAME));
			if (strcmp (val, n) == 0) {
				MonoString *fn;
				g_free (n);
				n = g_concat_dir_and_file (assembly->assembly->basedir, val);
				fn = mono_string_new (mono_object_domain (assembly), n);
				g_free (n);
				return (MonoObject*)fn;
			}
		}
		g_free (n);
		return NULL;
	}

	count = 0;
	for (i = 0; i < table->rows; ++i) {
		if (resource_modules || !(mono_metadata_decode_row_col (table, i, MONO_FILE_FLAGS) & FILE_CONTAINS_NO_METADATA))
			count ++;
	}

	result = mono_array_new (mono_object_domain (assembly), mono_defaults.string_class, count);

	count = 0;
	for (i = 0; i < table->rows; ++i) {
		if (resource_modules || !(mono_metadata_decode_row_col (table, i, MONO_FILE_FLAGS) & FILE_CONTAINS_NO_METADATA)) {
			val = mono_metadata_string_heap (assembly->assembly->image, mono_metadata_decode_row_col (table, i, MONO_FILE_NAME));
			n = g_concat_dir_and_file (assembly->assembly->basedir, val);
			mono_array_setref (result, count, mono_string_new (mono_object_domain (assembly), n));
			g_free (n);
			count ++;
		}
	}
	return (MonoObject*)result;
}