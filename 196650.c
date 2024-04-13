do_stobj (VerifyContext *ctx, int token) 
{
	ILStackDesc *dest, *src;
	MonoType *type = get_boxable_mono_type (ctx, token, "stobj");
	CLEAR_PREFIX (ctx, PREFIX_UNALIGNED | PREFIX_VOLATILE);

	if (!type)
		return;

	if (!check_underflow (ctx, 2))
		return;

	src = stack_pop (ctx);
	dest = stack_pop (ctx);

	if (stack_slot_is_managed_mutability_pointer (dest))
		CODE_NOT_VERIFIABLE (ctx, g_strdup_printf ("Cannot use a readonly pointer with stobj at 0x%04x", ctx->ip_offset));

	if (!stack_slot_is_managed_pointer (dest)) 
		CODE_NOT_VERIFIABLE (ctx, g_strdup_printf ("Invalid destination of stobj operation at 0x%04x", ctx->ip_offset));

	if (stack_slot_is_boxed_value (src) && !MONO_TYPE_IS_REFERENCE (src->type) && !MONO_TYPE_IS_REFERENCE (type))
		CODE_NOT_VERIFIABLE (ctx, g_strdup_printf ("Cannot use stobj with a boxed source value that is not a reference type at 0x%04x", ctx->ip_offset));

	if (!verify_stack_type_compatibility (ctx, type, src))
		CODE_NOT_VERIFIABLE (ctx, g_strdup_printf ("Token and source types of stobj don't match at 0x%04x", ctx->ip_offset));

	if (!verify_type_compatibility (ctx, mono_type_get_type_byval (dest->type), type))
		CODE_NOT_VERIFIABLE (ctx, g_strdup_printf ("Destination and token types of stobj don't match at 0x%04x", ctx->ip_offset));
}