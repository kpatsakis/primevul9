ves_icall_System_Reflection_Assembly_InternalGetAssemblyName (MonoString *fname, MonoReflectionAssemblyName *aname)
{
	char *filename;
	MonoImageOpenStatus status = MONO_IMAGE_OK;
	gboolean res;
	MonoImage *image;
	MonoAssemblyName name;
	char *dirname

	MONO_ARCH_SAVE_REGS;

	filename = mono_string_to_utf8 (fname);

	dirname = g_path_get_dirname (filename);
	replace_shadow_path (mono_domain_get (), dirname, &filename);
	g_free (dirname);

	image = mono_image_open (filename, &status);

	if (!image){
		MonoException *exc;

		g_free (filename);
		if (status == MONO_IMAGE_IMAGE_INVALID)
			exc = mono_get_exception_bad_image_format2 (NULL, fname);
		else
			exc = mono_get_exception_file_not_found2 (NULL, fname);
		mono_raise_exception (exc);
	}

	res = mono_assembly_fill_assembly_name (image, &name);
	if (!res) {
		mono_image_close (image);
		g_free (filename);
		mono_raise_exception (mono_get_exception_argument ("assemblyFile", "The file does not contain a manifest"));
	}

	fill_reflection_assembly_name (mono_domain_get (), aname, &name, filename,
		TRUE, mono_framework_version () == 1,
		mono_framework_version () >= 2);

	g_free (filename);
	mono_image_close (image);
}