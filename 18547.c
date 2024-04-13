generate_digest_begin(cms_context *cms)
{
	struct digest *digests = NULL;

	if (cms->digests) {
		digests = cms->digests;
	} else {
		digests = PORT_ZAlloc(n_digest_params * sizeof (*digests));
		if (digests == NULL)
			cmsreterr(-1, cms, "could not allocate digest context");
	}

	for (int i = 0; i < n_digest_params; i++) {
		digests[i].pk11ctx = PK11_CreateDigestContext(
						digest_params[i].digest_tag);
		if (!digests[i].pk11ctx) {
			cms->log(cms, LOG_ERR, "%s:%s:%d could not create "
				"digest context: %s",
				__FILE__, __func__, __LINE__,
				PORT_ErrorToString(PORT_GetError()));
			goto err;
		}

		PK11_DigestBegin(digests[i].pk11ctx);
	}

	cms->digests = digests;
	return 0;

err:
	for (int i = 0; i < n_digest_params; i++) {
		if (digests[i].pk11ctx)
			PK11_DestroyContext(digests[i].pk11ctx, PR_TRUE);
	}

	free(digests);
	return -1;
}