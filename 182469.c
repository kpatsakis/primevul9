static int __smb2_reconnect(const struct nls_table *nlsc,
			    struct cifs_tcon *tcon)
{
	int rc;
	struct dfs_cache_tgt_list tl;
	struct dfs_cache_tgt_iterator *it = NULL;
	char *tree;
	const char *tcp_host;
	size_t tcp_host_len;
	const char *dfs_host;
	size_t dfs_host_len;

	tree = kzalloc(MAX_TREE_SIZE, GFP_KERNEL);
	if (!tree)
		return -ENOMEM;

	if (tcon->ipc) {
		scnprintf(tree, MAX_TREE_SIZE, "\\\\%s\\IPC$",
			  tcon->ses->server->hostname);
		rc = SMB2_tcon(0, tcon->ses, tree, tcon, nlsc);
		goto out;
	}

	if (!tcon->dfs_path) {
		rc = SMB2_tcon(0, tcon->ses, tcon->treeName, tcon, nlsc);
		goto out;
	}

	rc = dfs_cache_noreq_find(tcon->dfs_path + 1, NULL, &tl);
	if (rc)
		goto out;

	extract_unc_hostname(tcon->ses->server->hostname, &tcp_host,
			     &tcp_host_len);

	for (it = dfs_cache_get_tgt_iterator(&tl); it;
	     it = dfs_cache_get_next_tgt(&tl, it)) {
		const char *tgt = dfs_cache_get_tgt_name(it);

		extract_unc_hostname(tgt, &dfs_host, &dfs_host_len);

		if (dfs_host_len != tcp_host_len
		    || strncasecmp(dfs_host, tcp_host, dfs_host_len) != 0) {
			cifs_dbg(FYI, "%s: skipping %.*s, doesn't match %.*s",
				 __func__,
				 (int)dfs_host_len, dfs_host,
				 (int)tcp_host_len, tcp_host);
			continue;
		}

		scnprintf(tree, MAX_TREE_SIZE, "\\%s", tgt);

		rc = SMB2_tcon(0, tcon->ses, tree, tcon, nlsc);
		if (!rc)
			break;
		if (rc == -EREMOTE)
			break;
	}

	if (!rc) {
		if (it)
			rc = dfs_cache_noreq_update_tgthint(tcon->dfs_path + 1,
							    it);
		else
			rc = -ENOENT;
	}
	dfs_cache_free_tgts(&tl);
out:
	kfree(tree);
	return rc;
}