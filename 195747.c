int cli_bytecode_context_getresult_file(struct cli_bc_ctx *ctx, char **tempfilename)
{
    int fd;
    *tempfilename = ctx->tempfile;
    fd  = ctx->outfd;
    ctx->tempfile = NULL;
    ctx->outfd = 0;
    return fd;
}