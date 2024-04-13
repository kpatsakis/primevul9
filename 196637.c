mono_type_is_valid_in_context (VerifyContext *ctx, MonoType *type)
{
	MonoClass *klass;

	if (type == NULL) {
		ADD_VERIFY_ERROR2 (ctx, g_strdup_printf ("Invalid null type at 0x%04x", ctx->ip_offset), MONO_EXCEPTION_BAD_IMAGE);
		return FALSE;
	}

	if (!is_valid_type_in_context (ctx, type)) {
		char *str = mono_type_full_name (type);
		ADD_VERIFY_ERROR2 (ctx, g_strdup_printf ("Invalid generic type (%s%s) (argument out of range or %s is not generic) at 0x%04x",
			type->type == MONO_TYPE_VAR ? "!" : "!!",
			str,
			type->type == MONO_TYPE_VAR ? "class" : "method",
			ctx->ip_offset),
			MONO_EXCEPTION_BAD_IMAGE);		
		g_free (str);
		return FALSE;
	}

	klass = mono_class_from_mono_type (type);
	mono_class_init (klass);
	if (mono_loader_get_last_error () || klass->exception_type != MONO_EXCEPTION_NONE) {
		if (klass->generic_class && !mono_class_is_valid_generic_instantiation (NULL, klass))
			ADD_VERIFY_ERROR2 (ctx, g_strdup_printf ("Invalid generic instantiation of type %s.%s at 0x%04x", klass->name_space, klass->name, ctx->ip_offset), MONO_EXCEPTION_TYPE_LOAD);
		else
			ADD_VERIFY_ERROR2 (ctx, g_strdup_printf ("Could not load type %s.%s at 0x%04x", klass->name_space, klass->name, ctx->ip_offset), MONO_EXCEPTION_TYPE_LOAD);
		return FALSE;
	}

	if (klass->generic_class && klass->generic_class->container_class->exception_type != MONO_EXCEPTION_NONE) {
		ADD_VERIFY_ERROR2 (ctx, g_strdup_printf ("Could not load type %s.%s at 0x%04x", klass->name_space, klass->name, ctx->ip_offset), MONO_EXCEPTION_TYPE_LOAD);
		return FALSE;
	}

	if (!klass->generic_class)
		return TRUE;

	if (!mono_class_is_valid_generic_instantiation (ctx, klass)) {
		ADD_VERIFY_ERROR2 (ctx, g_strdup_printf ("Invalid generic type instantiation of type %s.%s at 0x%04x", klass->name_space, klass->name, ctx->ip_offset), MONO_EXCEPTION_TYPE_LOAD);
		return FALSE;
	}

	if (!mono_class_repect_method_constraints (ctx, klass)) {
		ADD_VERIFY_ERROR2 (ctx, g_strdup_printf ("Invalid generic type instantiation of type %s.%s (generic args don't respect target's constraints) at 0x%04x", klass->name_space, klass->name, ctx->ip_offset), MONO_EXCEPTION_TYPE_LOAD);
		return FALSE;
	}

	return TRUE;
}