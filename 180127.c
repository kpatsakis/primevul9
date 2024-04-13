ves_icall_type_from_name (MonoString *name,
			  MonoBoolean throwOnError,
			  MonoBoolean ignoreCase)
{
	char *str = mono_string_to_utf8 (name);
	MonoReflectionType *type;

	type = type_from_name (str, ignoreCase);
	g_free (str);
	if (type == NULL){
		MonoException *e = NULL;
		
		if (throwOnError)
			e = mono_get_exception_type_load (name, NULL);

		mono_loader_clear_error ();
		if (e != NULL)
			mono_raise_exception (e);
	}
	
	return type;
}