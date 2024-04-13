ves_icall_System_MonoMethodInfo_get_retval_marshal (MonoMethod *method)
{
	MonoDomain *domain = mono_domain_get (); 
	MonoReflectionMarshal* res = NULL;
	MonoMarshalSpec **mspecs;
	int i;

	mspecs = g_new (MonoMarshalSpec*, mono_method_signature (method)->param_count + 1);
	mono_method_get_marshal_info (method, mspecs);

	if (mspecs [0])
		res = mono_reflection_marshal_from_marshal_spec (domain, method->klass, mspecs [0]);
		
	for (i = mono_method_signature (method)->param_count; i >= 0; i--)
		if (mspecs [i])
			mono_metadata_free_marshal_spec (mspecs [i]);
	g_free (mspecs);

	return res;
}