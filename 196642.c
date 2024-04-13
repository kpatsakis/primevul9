verifier_load_field (VerifyContext *ctx, int token, MonoClass **out_klass, const char *opcode) {
	MonoClassField *field;
	MonoClass *klass = NULL;

	if (!IS_FIELD_DEF_OR_REF (token) || !token_bounds_check (ctx->image, token)) {
		ADD_VERIFY_ERROR2 (ctx, g_strdup_printf ("Invalid field token 0x%08x for %s at 0x%04x", token, opcode, ctx->ip_offset), MONO_EXCEPTION_BAD_IMAGE);
		return NULL;
	}

	field = mono_field_from_token (ctx->image, token, &klass, ctx->generic_context);
	if (!field || !field->parent || !klass) {
		ADD_VERIFY_ERROR2 (ctx, g_strdup_printf ("Cannot load field from token 0x%08x for %s at 0x%04x", token, opcode, ctx->ip_offset), MONO_EXCEPTION_BAD_IMAGE);
		return NULL;
	}

	if (!mono_type_is_valid_in_context (ctx, &klass->byval_arg))
		return NULL;

	*out_klass = klass;
	return field;
}