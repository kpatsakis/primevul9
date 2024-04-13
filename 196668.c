verifier_load_type (VerifyContext *ctx, int token, const char *opcode) {
	MonoType* type;
	
	if (!IS_TYPE_DEF_OR_REF_OR_SPEC (token) || !token_bounds_check (ctx->image, token)) {
		ADD_VERIFY_ERROR2 (ctx, g_strdup_printf ("Invalid type token 0x%08x at 0x%04x", token, ctx->ip_offset), MONO_EXCEPTION_BAD_IMAGE);
		return NULL;
	}

	type = mono_type_get_full (ctx->image, token, ctx->generic_context);

	if (!type) {
		ADD_VERIFY_ERROR2 (ctx, g_strdup_printf ("Cannot load type from token 0x%08x for %s at 0x%04x", token, opcode, ctx->ip_offset), MONO_EXCEPTION_BAD_IMAGE);
		return NULL;
	}

	if (!mono_type_is_valid_in_context (ctx, type))
		return NULL;

	return type;
}