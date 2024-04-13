mono_method_body_get_object (MonoDomain *domain, MonoMethod *method)
{
	static MonoClass *System_Reflection_MethodBody = NULL;
	static MonoClass *System_Reflection_LocalVariableInfo = NULL;
	static MonoClass *System_Reflection_ExceptionHandlingClause = NULL;
	MonoReflectionMethodBody *ret;
	MonoMethodNormal *mn;
	MonoMethodHeader *header;
	MonoImage *image;
	guint32 method_rva, local_var_sig_token;
    char *ptr;
	unsigned char format, flags;
	int i;

	if (!System_Reflection_MethodBody)
		System_Reflection_MethodBody = mono_class_from_name (mono_defaults.corlib, "System.Reflection", "MethodBody");
	if (!System_Reflection_LocalVariableInfo)
		System_Reflection_LocalVariableInfo = mono_class_from_name (mono_defaults.corlib, "System.Reflection", "LocalVariableInfo");
	if (!System_Reflection_ExceptionHandlingClause)
		System_Reflection_ExceptionHandlingClause = mono_class_from_name (mono_defaults.corlib, "System.Reflection", "ExceptionHandlingClause");

	CHECK_OBJECT (MonoReflectionMethodBody *, method, NULL);

	if ((method->flags & METHOD_ATTRIBUTE_PINVOKE_IMPL) ||
		(method->flags & METHOD_ATTRIBUTE_ABSTRACT) ||
	    (method->iflags & METHOD_IMPL_ATTRIBUTE_INTERNAL_CALL) ||
	    (method->iflags & METHOD_IMPL_ATTRIBUTE_RUNTIME))
		return NULL;

	mn = (MonoMethodNormal *)method;
	image = method->klass->image;
	header = mono_method_get_header (method);

	if (!image->dynamic) {
		/* Obtain local vars signature token */
		method_rva = mono_metadata_decode_row_col (&image->tables [MONO_TABLE_METHOD], mono_metadata_token_index (method->token) - 1, MONO_METHOD_RVA);
		ptr = mono_image_rva_map (image, method_rva);
		flags = *(const unsigned char *) ptr;
		format = flags & METHOD_HEADER_FORMAT_MASK;
		switch (format){
		case METHOD_HEADER_TINY_FORMAT:
			local_var_sig_token = 0;
			break;
		case METHOD_HEADER_FAT_FORMAT:
			ptr += 2;
			ptr += 2;
			ptr += 4;
			local_var_sig_token = read32 (ptr);
			break;
		default:
			g_assert_not_reached ();
		}
	} else
		local_var_sig_token = 0; //FIXME

	ret = (MonoReflectionMethodBody*)mono_object_new (domain, System_Reflection_MethodBody);

	ret->init_locals = header->init_locals;
	ret->max_stack = header->max_stack;
	ret->local_var_sig_token = local_var_sig_token;
	MONO_OBJECT_SETREF (ret, il, mono_array_new_cached (domain, mono_defaults.byte_class, header->code_size));
	memcpy (mono_array_addr (ret->il, guint8, 0), header->code, header->code_size);

	/* Locals */
	MONO_OBJECT_SETREF (ret, locals, mono_array_new_cached (domain, System_Reflection_LocalVariableInfo, header->num_locals));
	for (i = 0; i < header->num_locals; ++i) {
		MonoReflectionLocalVariableInfo *info = (MonoReflectionLocalVariableInfo*)mono_object_new (domain, System_Reflection_LocalVariableInfo);
		MONO_OBJECT_SETREF (info, local_type, mono_type_get_object (domain, header->locals [i]));
		info->is_pinned = header->locals [i]->pinned;
		info->local_index = i;
		mono_array_setref (ret->locals, i, info);
	}

	/* Exceptions */
	MONO_OBJECT_SETREF (ret, clauses, mono_array_new_cached (domain, System_Reflection_ExceptionHandlingClause, header->num_clauses));
	for (i = 0; i < header->num_clauses; ++i) {
		MonoReflectionExceptionHandlingClause *info = (MonoReflectionExceptionHandlingClause*)mono_object_new (domain, System_Reflection_ExceptionHandlingClause);
		MonoExceptionClause *clause = &header->clauses [i];

		info->flags = clause->flags;
		info->try_offset = clause->try_offset;
		info->try_length = clause->try_len;
		info->handler_offset = clause->handler_offset;
		info->handler_length = clause->handler_len;
		if (clause->flags == MONO_EXCEPTION_CLAUSE_FILTER)
			info->filter_offset = clause->data.filter_offset;
		else if (clause->data.catch_class)
			MONO_OBJECT_SETREF (info, catch_type, mono_type_get_object (mono_domain_get (), &clause->data.catch_class->byval_arg));

		mono_array_setref (ret->clauses, i, info);
	}

	CACHE_OBJECT (MonoReflectionMethodBody *, method, ret, NULL);
	return ret;
}