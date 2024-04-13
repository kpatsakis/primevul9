stack_push (VerifyContext *ctx)
{
	g_assert (ctx->eval.size < ctx->max_stack);
	return & ctx->eval.stack [ctx->eval.size++];
}