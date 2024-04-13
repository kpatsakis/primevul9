int cli_bytecode_context_setpe(struct cli_bc_ctx *ctx, const struct cli_pe_hook_data *data, const struct cli_exe_section *sections)
{
    ctx->sections = sections;
    ctx->hooks.pedata = data;
    return 0;
}