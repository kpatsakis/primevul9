static int bnx2x_prev_unload_uncommon(struct bnx2x *bp)
{
	int rc;

	BNX2X_DEV_INFO("Uncommon unload Flow\n");

	/* Test if previous unload process was already finished for this path */
	if (bnx2x_prev_is_path_marked(bp))
		return bnx2x_prev_mcp_done(bp);

	BNX2X_DEV_INFO("Path is unmarked\n");

	/* Cannot proceed with FLR if UNDI is loaded, since FW does not match */
	if (bnx2x_prev_is_after_undi(bp))
		goto out;

	/* If function has FLR capabilities, and existing FW version matches
	 * the one required, then FLR will be sufficient to clean any residue
	 * left by previous driver
	 */
	rc = bnx2x_compare_fw_ver(bp, FW_MSG_CODE_DRV_LOAD_FUNCTION, false);

	if (!rc) {
		/* fw version is good */
		BNX2X_DEV_INFO("FW version matches our own. Attempting FLR\n");
		rc = bnx2x_do_flr(bp);
	}

	if (!rc) {
		/* FLR was performed */
		BNX2X_DEV_INFO("FLR successful\n");
		return 0;
	}

	BNX2X_DEV_INFO("Could not FLR\n");

out:
	/* Close the MCP request, return failure*/
	rc = bnx2x_prev_mcp_done(bp);
	if (!rc)
		rc = BNX2X_PREV_WAIT_NEEDED;

	return rc;
}