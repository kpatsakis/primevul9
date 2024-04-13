static void destroy_contexts(mm_context_t *ctx)
{
	int index, context_id;

	for (index = 0; index < ARRAY_SIZE(ctx->extended_id); index++) {
		context_id = ctx->extended_id[index];
		if (context_id)
			ida_free(&mmu_context_ida, context_id);
	}
}