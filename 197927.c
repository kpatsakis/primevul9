finish_bgprint(fz_context *ctx)
{
	int status;

	if (!bgprint.active)
		return;

	/* If we are using it, then wait for it to finish. */
	status = wait_for_bgprint_to_finish();
	if (status == RENDER_OK)
	{
		/* The background bgprint completed successfully. */
		return;
	}

	if (status == RENDER_FATAL)
	{
		/* We failed because of not being able to output. No point in retrying. */
		fz_throw(ctx, FZ_ERROR_GENERIC, "Failed to render page");
	}
	bgprint.started = 1;
	bgprint.solo = 1;
	mu_trigger_semaphore(&bgprint.start);
	status = wait_for_bgprint_to_finish();
	if (status != 0)
	{
		/* Hard failure */
		fz_throw(ctx, FZ_ERROR_GENERIC, "Failed to render page");
	}
}