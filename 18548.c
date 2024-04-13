generate_digest_finish(cms_context *cms)
{
	void *mark = PORT_ArenaMark(cms->arena);

	for (int i = 0; i < n_digest_params; i++) {
		SECItem *digest = PORT_ArenaZAlloc(cms->arena,sizeof (SECItem));
		if (digest == NULL) {
			cms->log(cms, LOG_ERR, "%s:%s:%d could not allocate "
				"memory: %s", __FILE__, __func__, __LINE__,
				PORT_ErrorToString(PORT_GetError()));
			goto err;
		}

		digest->type = siBuffer;
		digest->len = digest_params[i].size;
		digest->data = PORT_ArenaZAlloc(cms->arena, digest_params[i].size);
		if (digest->data == NULL) {
			cms->log(cms, LOG_ERR, "%s:%s:%d could not allocate "
				"memory: %s", __FILE__, __func__, __LINE__,
				PORT_ErrorToString(PORT_GetError()));
			goto err;
		}

		PK11_DigestFinal(cms->digests[i].pk11ctx,
			digest->data, &digest->len, digest_params[i].size);
		PK11_Finalize(cms->digests[i].pk11ctx);
		PK11_DestroyContext(cms->digests[i].pk11ctx, PR_TRUE);
		cms->digests[i].pk11ctx = NULL;
		/* XXX sure seems like we should be freeing it here,
		 * but that's segfaulting, and we know it'll get
		 * cleaned up with PORT_FreeArena a couple of lines
		 * down.
		 */
		cms->digests[i].pe_digest = digest;
	}

	PORT_ArenaUnmark(cms->arena, mark);
	return 0;
err:
	for (int i = 0; i < n_digest_params; i++) {
		if (cms->digests[i].pk11ctx)
			PK11_DestroyContext(cms->digests[i].pk11ctx, PR_TRUE);
	}
	PORT_ArenaRelease(cms->arena, mark);
	return -1;
}