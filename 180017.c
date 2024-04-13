ves_icall_System_Reflection_Module_ResolveFieldToken (MonoImage *image, guint32 token, MonoArray *type_args, MonoArray *method_args, MonoResolveTokenError *error)
{
	MonoClass *klass;
	int table = mono_metadata_token_table (token);
	int index = mono_metadata_token_index (token);
	MonoGenericContext context;
	MonoClassField *field;

	*error = ResolveTokenError_Other;

	/* Validate token */
	if ((table != MONO_TABLE_FIELD) && (table != MONO_TABLE_MEMBERREF)) {
		*error = ResolveTokenError_BadTable;
		return NULL;
	}

	if (image->dynamic) {
		if (type_args || method_args)
			mono_raise_exception (mono_get_exception_not_implemented (NULL));
		/* FIXME: validate memberref token type */
		return mono_lookup_dynamic_token_class (image, token, FALSE, NULL, NULL);
	}

	if ((index <= 0) || (index > image->tables [table].rows)) {
		*error = ResolveTokenError_OutOfRange;
		return NULL;
	}
	if ((table == MONO_TABLE_MEMBERREF) && (mono_metadata_memberref_is_method (image, token))) {
		*error = ResolveTokenError_BadTable;
		return NULL;
	}

	init_generic_context_from_args (&context, type_args, method_args);
	field = mono_field_from_token (image, token, &klass, &context);

	if (mono_loader_get_last_error ())
		mono_raise_exception (mono_loader_error_prepare_exception (mono_loader_get_last_error ()));
	
	return field;
}