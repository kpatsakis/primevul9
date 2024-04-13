mono_image_verify_tables (MonoImage *image, int level)
{
	GSList *error_list = NULL;

	error_list = verify_assembly_table (image, error_list, level);
	/* 
	 * AssemblyOS, AssemblyProcessor, AssemblyRefOs and
	 * AssemblyRefProcessor should be ignored, 
	 * though we may want to emit a warning, since it should not 
	 * be present in a PE file.
	 */
	error_list = verify_assemblyref_table (image, error_list, level);
	error_list = verify_class_layout_table (image, error_list, level);
	error_list = verify_constant_table (image, error_list, level);
	/*
	 * cutom attribute, declsecurity 
	 */
	error_list = verify_event_map_table (image, error_list, level);
	error_list = verify_event_table (image, error_list, level);
	error_list = verify_field_table (image, error_list, level);
	error_list = verify_file_table (image, error_list, level);
	error_list = verify_moduleref_table (image, error_list, level);
	error_list = verify_standalonesig_table (image, error_list, level);

	return g_slist_reverse (error_list);
}