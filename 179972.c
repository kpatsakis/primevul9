ves_icall_get_resources_ptr (MonoReflectionAssembly *assembly, gpointer *result, gint32 *size)
{
	MonoPEResourceDataEntry *entry;
	MonoImage *image;

	MONO_ARCH_SAVE_REGS;

	if (!assembly || !result || !size)
		return FALSE;

	*result = NULL;
	*size = 0;
	image = assembly->assembly->image;
	entry = mono_image_lookup_resource (image, MONO_PE_RESOURCE_ID_ASPNET_STRING, 0, NULL);
	if (!entry)
		return FALSE;

	*result = mono_image_rva_map (image, entry->rde_data_offset);
	if (!(*result)) {
		g_free (entry);
		return FALSE;
	}
	*size = entry->rde_size;
	g_free (entry);
	return TRUE;
}