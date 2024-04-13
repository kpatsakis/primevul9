ves_icall_System_Reflection_Assembly_GetReferencedAssemblies (MonoReflectionAssembly *assembly) 
{
	static MonoClass *System_Reflection_AssemblyName;
	MonoArray *result;
	MonoDomain *domain = mono_object_domain (assembly);
	int i, count = 0;
	static MonoMethod *create_culture = NULL;
	MonoImage *image = assembly->assembly->image;
	MonoTableInfo *t;

	MONO_ARCH_SAVE_REGS;

	if (!System_Reflection_AssemblyName)
		System_Reflection_AssemblyName = mono_class_from_name (
			mono_defaults.corlib, "System.Reflection", "AssemblyName");

	t = &assembly->assembly->image->tables [MONO_TABLE_ASSEMBLYREF];
	count = t->rows;

	result = mono_array_new (domain, System_Reflection_AssemblyName, count);

	if (count > 0 && !create_culture) {
		MonoMethodDesc *desc = mono_method_desc_new (
			"System.Globalization.CultureInfo:CreateCulture(string,bool)", TRUE);
		create_culture = mono_method_desc_search_in_image (desc, mono_defaults.corlib);
		g_assert (create_culture);
		mono_method_desc_free (desc);
	}

	for (i = 0; i < count; i++) {
		MonoReflectionAssemblyName *aname;
		guint32 cols [MONO_ASSEMBLYREF_SIZE];

		mono_metadata_decode_row (t, i, cols, MONO_ASSEMBLYREF_SIZE);

		aname = (MonoReflectionAssemblyName *) mono_object_new (
			domain, System_Reflection_AssemblyName);

		MONO_OBJECT_SETREF (aname, name, mono_string_new (domain, mono_metadata_string_heap (image, cols [MONO_ASSEMBLYREF_NAME])));

		aname->major = cols [MONO_ASSEMBLYREF_MAJOR_VERSION];
		aname->minor = cols [MONO_ASSEMBLYREF_MINOR_VERSION];
		aname->build = cols [MONO_ASSEMBLYREF_BUILD_NUMBER];
		aname->revision = cols [MONO_ASSEMBLYREF_REV_NUMBER];
		aname->flags = cols [MONO_ASSEMBLYREF_FLAGS];
		aname->versioncompat = 1; /* SameMachine (default) */
		aname->hashalg = ASSEMBLY_HASH_SHA1; /* SHA1 (default) */
		MONO_OBJECT_SETREF (aname, version, create_version (domain, aname->major, aname->minor, aname->build, aname->revision));

		if (create_culture) {
			gpointer args [2];
			MonoBoolean assembly_ref = 1;
			args [0] = mono_string_new (domain, mono_metadata_string_heap (image, cols [MONO_ASSEMBLYREF_CULTURE]));
			args [1] = &assembly_ref;
			MONO_OBJECT_SETREF (aname, cultureInfo, mono_runtime_invoke (create_culture, NULL, args, NULL));
		}
		
		if (cols [MONO_ASSEMBLYREF_PUBLIC_KEY]) {
			const gchar *pkey_ptr = mono_metadata_blob_heap (image, cols [MONO_ASSEMBLYREF_PUBLIC_KEY]);
			guint32 pkey_len = mono_metadata_decode_blob_size (pkey_ptr, &pkey_ptr);

			if ((cols [MONO_ASSEMBLYREF_FLAGS] & ASSEMBLYREF_FULL_PUBLIC_KEY_FLAG)) {
				/* public key token isn't copied - the class library will 
		   		automatically generate it from the public key if required */
				MONO_OBJECT_SETREF (aname, publicKey, mono_array_new (domain, mono_defaults.byte_class, pkey_len));
				memcpy (mono_array_addr (aname->publicKey, guint8, 0), pkey_ptr, pkey_len);
			} else {
				MONO_OBJECT_SETREF (aname, keyToken, mono_array_new (domain, mono_defaults.byte_class, pkey_len));
				memcpy (mono_array_addr (aname->keyToken, guint8, 0), pkey_ptr, pkey_len);
			}
		} else {
			MONO_OBJECT_SETREF (aname, keyToken, mono_array_new (domain, mono_defaults.byte_class, 0));
		}
		
		/* note: this function doesn't return the codebase on purpose (i.e. it can
		         be used under partial trust as path information isn't present). */

		mono_array_setref (result, i, aname);
	}
	return result;
}