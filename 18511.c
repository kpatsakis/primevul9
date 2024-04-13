sign_pe(context *ctx, int infd, int outfd, int attached)
{
	Pe *inpe = NULL;

	int rc = set_up_inpe(ctx, infd, &inpe);
	if (rc < 0)
		goto finish;

	if (attached) {
		Pe *outpe = NULL;
		rc = set_up_outpe(ctx, outfd, inpe, &outpe);
		if (rc < 0)
			goto finish;

		rc = generate_digest(ctx->cms, outpe, 1);
		if (rc < 0) {
err_attached:
			pe_end(outpe);
			ftruncate(outfd, 0);
			goto finish;
		}
		ssize_t sigspace = calculate_signature_space(ctx->cms, outpe);
		if (sigspace < 0)
			goto err_attached;
		allocate_signature_space(outpe, sigspace);
		rc = generate_digest(ctx->cms, outpe, 1);
		if (rc < 0)
			goto err_attached;
		rc = generate_signature(ctx->cms);
		if (rc < 0)
			goto err_attached;
		insert_signature(ctx->cms, ctx->cms->num_signatures);
		finalize_signatures(ctx->cms->signatures,
				ctx->cms->num_signatures, outpe);
		pe_end(outpe);
	} else {
		ftruncate(outfd, 0);
		rc = generate_digest(ctx->cms, inpe, 1);
		if (rc < 0) {
err_detached:
			ftruncate(outfd, 0);
			goto finish;
		}
		rc = generate_signature(ctx->cms);
		if (rc < 0)
			goto err_detached;
		rc = export_signature(ctx->cms, outfd, 0);
		if (rc >= 0)
			ftruncate(outfd, rc);
		else if (rc < 0)
			goto err_detached;
	}

finish:
	if (inpe)
		pe_end(inpe);

	return rc;
}