create_version (MonoDomain *domain, guint32 major, guint32 minor, guint32 build, guint32 revision)
{
	static MonoClass *System_Version = NULL;
	static MonoMethod *create_version = NULL;
	MonoObject *result;
	gpointer args [4];
	
	if (!System_Version) {
		System_Version = mono_class_from_name (mono_defaults.corlib, "System", "Version");
		g_assert (System_Version);
	}

	if (!create_version) {
		MonoMethodDesc *desc = mono_method_desc_new (":.ctor(int,int,int,int)", FALSE);
		create_version = mono_method_desc_search_in_class (desc, System_Version);
		g_assert (create_version);
		mono_method_desc_free (desc);
	}

	args [0] = &major;
	args [1] = &minor;
	args [2] = &build;
	args [3] = &revision;
	result = mono_object_new (domain, System_Version);
	mono_runtime_invoke (create_version, result, args, NULL);

	return result;
}