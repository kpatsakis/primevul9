ves_icall_System_Reflection_Assembly_GetManifestResourceInternal (MonoReflectionAssembly *assembly, MonoString *name, gint32 *size, MonoReflectionModule **ref_module) 
{
	char *n = mono_string_to_utf8 (name);
	MonoTableInfo *table = &assembly->assembly->image->tables [MONO_TABLE_MANIFESTRESOURCE];
	guint32 i;
	guint32 cols [MONO_MANIFEST_SIZE];
	guint32 impl, file_idx;
	const char *val;
	MonoImage *module;

	MONO_ARCH_SAVE_REGS;

	for (i = 0; i < table->rows; ++i) {
		mono_metadata_decode_row (table, i, cols, MONO_MANIFEST_SIZE);
		val = mono_metadata_string_heap (assembly->assembly->image, cols [MONO_MANIFEST_NAME]);
		if (strcmp (val, n) == 0)
			break;
	}
	g_free (n);
	if (i == table->rows)
		return NULL;
	/* FIXME */
	impl = cols [MONO_MANIFEST_IMPLEMENTATION];
	if (impl) {
		/*
		 * this code should only be called after obtaining the 
		 * ResourceInfo and handling the other cases.
		 */
		g_assert ((impl & MONO_IMPLEMENTATION_MASK) == MONO_IMPLEMENTATION_FILE);
		file_idx = impl >> MONO_IMPLEMENTATION_BITS;

		module = mono_image_load_file_for_image (assembly->assembly->image, file_idx);
		if (!module)
			return NULL;
	}
	else
		module = assembly->assembly->image;

	mono_gc_wbarrier_generic_store (ref_module, (MonoObject*) mono_module_get_object (mono_domain_get (), module));

	return (void*)mono_image_get_resource (module, cols [MONO_MANIFEST_OFFSET], (guint32*)size);
}