ves_icall_System_Reflection_Assembly_GetManifestResourceNames (MonoReflectionAssembly *assembly) 
{
	MonoTableInfo *table = &assembly->assembly->image->tables [MONO_TABLE_MANIFESTRESOURCE];
	MonoArray *result = mono_array_new (mono_object_domain (assembly), mono_defaults.string_class, table->rows);
	int i;
	const char *val;

	MONO_ARCH_SAVE_REGS;

	for (i = 0; i < table->rows; ++i) {
		val = mono_metadata_string_heap (assembly->assembly->image, mono_metadata_decode_row_col (table, i, MONO_MANIFEST_NAME));
		mono_array_setref (result, i, mono_string_new (mono_object_domain (assembly), val));
	}
	return result;
}