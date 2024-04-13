do_shutdown(context *ctx, int nsockets, struct pollfd *pollfds)
{
	unlink(SOCKPATH);
	unlink(PIDFILE);

	for (int i = 0; i < ctx->ntokennames; i++)
		free(ctx->tokennames[i]);
	if (ctx->tokennames)
		free(ctx->tokennames);
	ctx->backup_cms->log(ctx->backup_cms, ctx->priority|LOG_NOTICE,
			"pesignd exiting (pid %d)", getpid());

	xfree(ctx->errstr);

	for (int i = 0; i < nsockets; i++)
		close(pollfds[i].fd);
	free(pollfds);
}