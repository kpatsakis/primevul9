do_store_static_field (VerifyContext *ctx, int token) {
	MonoClassField *field;
	MonoClass *klass;
	ILStackDesc *value;
	CLEAR_PREFIX (ctx, PREFIX_VOLATILE);

	if (!check_underflow (ctx, 1))
		return;

	value = stack_pop (ctx);

	if (!(field = verifier_load_field (ctx, token, &klass, "stsfld")))
		return;

	if (!(field->type->attrs & FIELD_ATTRIBUTE_STATIC)) { 
		ADD_VERIFY_ERROR (ctx, g_strdup_printf ("Cannot store non static field at 0x%04x", ctx->ip_offset));
		return;
	}

	if (field->type->type == MONO_TYPE_TYPEDBYREF) {
		ADD_VERIFY_ERROR (ctx, g_strdup_printf ("Typedbyref field is an unverfiable type in store static field at 0x%04x", ctx->ip_offset));
		return;
	}

	if (!IS_SKIP_VISIBILITY (ctx) && !mono_method_can_access_field_full (ctx->method, field, NULL))
		CODE_NOT_VERIFIABLE2 (ctx, g_strdup_printf ("Type at stack is not accessible at 0x%04x", ctx->ip_offset), MONO_EXCEPTION_FIELD_ACCESS);

	if (!verify_stack_type_compatibility (ctx, field->type, value))
		CODE_NOT_VERIFIABLE (ctx, g_strdup_printf ("Incompatible type %s in static field store at 0x%04x", stack_slot_get_name (value), ctx->ip_offset));	
}