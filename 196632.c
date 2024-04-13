stack_init (VerifyContext *ctx, ILCodeDesc *state) 
{
	if (state->flags & IL_CODE_FLAG_STACK_INITED)
		return;
	state->size = 0;
	state->flags |= IL_CODE_FLAG_STACK_INITED;
	if (!state->stack)
		state->stack = g_new0 (ILStackDesc, ctx->max_stack);
}