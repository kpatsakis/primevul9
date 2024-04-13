void cli_bytecode_context_destroy(struct cli_bc_ctx *ctx)
{
   cli_bytecode_context_clear(ctx);
   free(ctx);
}