ves_icall_System_Reflection_Module_ResolveTypeToken (MonoImage *image, guint32 token, MonoArray *type_args, MonoArray *method_args, MonoResolveTokenError *error)
{
	MonoClass *klass;
	int table = mono_metadata_token_table (token);
	int index = mono_metadata_token_index (token);
	MonoGenericContext context;

	*error = ResolveTokenError_Other;

	/* Validate token */
	if ((table != MONO_TABLE_TYPEDEF) && (table != MONO_TABLE_TYPEREF) && 
		(table != MONO_TABLE_TYPESPEC)) {
		*error = ResolveTokenError_BadTable;
		return NULL;
	}

	if (image->dynamic) {
		if (type_args || method_args)
			mono_raise_exception (mono_get_exception_not_implemented (NULL));
		klass = mono_lookup_dynamic_token_class (image, token, FALSE, NULL, NULL);
		if (!klass)
			return NULL;
		return &klass->byval_arg;
	}

	if ((index <= 0) || (index > image->tables [table].rows)) {
		*error = ResolveTokenError_OutOfRange;
		return NULL;
	}

	init_generic_context_from_args (&context, type_args, method_args);
	klass = mono_class_get_full (image, token, &context);

	if (mono_loader_get_last_error ())
		mono_raise_exception (mono_loader_error_prepare_exception (mono_loader_get_last_error ()));

	if (klass)
		return &klass->byval_arg;
	else
		return NULL;
}