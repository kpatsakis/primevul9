static int proc_root_readdir(struct file *file, struct dir_context *ctx)
{
	if (ctx->pos < FIRST_PROCESS_ENTRY) {
		int error = proc_readdir(file, ctx);
		if (unlikely(error <= 0))
			return error;
		ctx->pos = FIRST_PROCESS_ENTRY;
	}

	return proc_pid_readdir(file, ctx);
}