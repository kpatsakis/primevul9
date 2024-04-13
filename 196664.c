init_stack_with_value_at_exception_boundary (VerifyContext *ctx, ILCodeDesc *code, MonoClass *klass)
{
	MonoError error;
	MonoType *type = mono_class_inflate_generic_type_checked (&klass->byval_arg, ctx->generic_context, &error);

	if (!mono_error_ok (&error)) {
		char *name = mono_type_get_full_name (klass);
		ADD_VERIFY_ERROR (ctx, g_strdup_printf ("Invalid class %s used for exception", name));
		g_free (name);
		mono_error_cleanup (&error);
		return;
	}

	if (!ctx->max_stack) {
		ADD_VERIFY_ERROR (ctx, g_strdup_printf ("Stack overflow at 0x%04x", ctx->ip_offset));
		return;
	}

	stack_init (ctx, code);
	set_stack_value (ctx, code->stack, type, FALSE);
	ctx->exception_types = g_slist_prepend (ctx->exception_types, type);
	code->size = 1;
	code->flags |= IL_CODE_FLAG_WAS_TARGET;
	if (mono_type_is_generic_argument (type))
		code->stack->stype |= BOXED_MASK;
}