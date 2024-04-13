ves_icall_System_Reflection_Assembly_GetManifestResourceInfoInternal (MonoReflectionAssembly *assembly, MonoString *name, MonoManifestResourceInfo *info)
{
	MonoTableInfo *table = &assembly->assembly->image->tables [MONO_TABLE_MANIFESTRESOURCE];
	int i;
	guint32 cols [MONO_MANIFEST_SIZE];
	guint32 file_cols [MONO_FILE_SIZE];
	const char *val;
	char *n;

	MONO_ARCH_SAVE_REGS;

	n = mono_string_to_utf8 (name);
	for (i = 0; i < table->rows; ++i) {
		mono_metadata_decode_row (table, i, cols, MONO_MANIFEST_SIZE);
		val = mono_metadata_string_heap (assembly->assembly->image, cols [MONO_MANIFEST_NAME]);
		if (strcmp (val, n) == 0)
			break;
	}
	g_free (n);
	if (i == table->rows)
		return FALSE;

	if (!cols [MONO_MANIFEST_IMPLEMENTATION]) {
		info->location = RESOURCE_LOCATION_EMBEDDED | RESOURCE_LOCATION_IN_MANIFEST;
	}
	else {
		switch (cols [MONO_MANIFEST_IMPLEMENTATION] & MONO_IMPLEMENTATION_MASK) {
		case MONO_IMPLEMENTATION_FILE:
			i = cols [MONO_MANIFEST_IMPLEMENTATION] >> MONO_IMPLEMENTATION_BITS;
			table = &assembly->assembly->image->tables [MONO_TABLE_FILE];
			mono_metadata_decode_row (table, i - 1, file_cols, MONO_FILE_SIZE);
			val = mono_metadata_string_heap (assembly->assembly->image, file_cols [MONO_FILE_NAME]);
			MONO_OBJECT_SETREF (info, filename, mono_string_new (mono_object_domain (assembly), val));
			if (file_cols [MONO_FILE_FLAGS] && FILE_CONTAINS_NO_METADATA)
				info->location = 0;
			else
				info->location = RESOURCE_LOCATION_EMBEDDED;
			break;

		case MONO_IMPLEMENTATION_ASSEMBLYREF:
			i = cols [MONO_MANIFEST_IMPLEMENTATION] >> MONO_IMPLEMENTATION_BITS;
			mono_assembly_load_reference (assembly->assembly->image, i - 1);
			if (assembly->assembly->image->references [i - 1] == (gpointer)-1) {
				char *msg = g_strdup_printf ("Assembly %d referenced from assembly %s not found ", i - 1, assembly->assembly->image->name);
				MonoException *ex = mono_get_exception_file_not_found2 (msg, NULL);
				g_free (msg);
				mono_raise_exception (ex);
			}
			MONO_OBJECT_SETREF (info, assembly, mono_assembly_get_object (mono_domain_get (), assembly->assembly->image->references [i - 1]));

			/* Obtain info recursively */
			ves_icall_System_Reflection_Assembly_GetManifestResourceInfoInternal (info->assembly, name, info);
			info->location |= RESOURCE_LOCATION_ANOTHER_ASSEMBLY;
			break;

		case MONO_IMPLEMENTATION_EXP_TYPE:
			g_assert_not_reached ();
			break;
		}
	}

	return TRUE;
}