static void grant_fsp_oplock_type(files_struct *fsp,
				  struct share_mode_lock *lck,
				  int oplock_request)
{
	bool allow_level2 = (global_client_caps & CAP_LEVEL_II_OPLOCKS) &&
		            lp_level2_oplocks(SNUM(fsp->conn));
	bool got_level2_oplock, got_a_none_oplock;
	uint32_t i;

	/* Start by granting what the client asked for,
	   but ensure no SAMBA_PRIVATE bits can be set. */
	fsp->oplock_type = (oplock_request & ~SAMBA_PRIVATE_OPLOCK_MASK);

	if (oplock_request & INTERNAL_OPEN_ONLY) {
		/* No oplocks on internal open. */
		fsp->oplock_type = NO_OPLOCK;
		DEBUG(10,("grant_fsp_oplock_type: oplock type 0x%x on file %s\n",
			fsp->oplock_type, fsp_str_dbg(fsp)));
		return;
	}

	if (lp_locking(fsp->conn->params) && file_has_brlocks(fsp)) {
		DEBUG(10,("grant_fsp_oplock_type: file %s has byte range locks\n",
			fsp_str_dbg(fsp)));
		fsp->oplock_type = NO_OPLOCK;
	}

	if (is_stat_open(fsp->access_mask)) {
		/* Leave the value already set. */
		DEBUG(10,("grant_fsp_oplock_type: oplock type 0x%x on file %s\n",
			fsp->oplock_type, fsp_str_dbg(fsp)));
		return;
	}

	got_level2_oplock = false;
	got_a_none_oplock = false;

	for (i=0; i<lck->data->num_share_modes; i++) {
		int op_type = lck->data->share_modes[i].op_type;

		if (LEVEL_II_OPLOCK_TYPE(op_type)) {
			got_level2_oplock = true;
		}
		if (op_type == NO_OPLOCK) {
			got_a_none_oplock = true;
		}
	}

	/*
	 * Match what was requested (fsp->oplock_type) with
 	 * what was found in the existing share modes.
 	 */

	if (got_level2_oplock || got_a_none_oplock) {
		if (EXCLUSIVE_OPLOCK_TYPE(fsp->oplock_type)) {
			fsp->oplock_type = LEVEL_II_OPLOCK;
		}
	}

	/*
	 * Don't grant level2 to clients that don't want them
	 * or if we've turned them off.
	 */
	if (fsp->oplock_type == LEVEL_II_OPLOCK && !allow_level2) {
		fsp->oplock_type = NO_OPLOCK;
	}

	if (fsp->oplock_type == LEVEL_II_OPLOCK && !got_level2_oplock) {
		/*
		 * We're the first level2 oplock. Indicate that in brlock.tdb.
		 */
		struct byte_range_lock *brl;

		brl = brl_get_locks(talloc_tos(), fsp);
		if (brl != NULL) {
			brl_set_have_read_oplocks(brl, true);
			TALLOC_FREE(brl);
		}
	}

	DEBUG(10,("grant_fsp_oplock_type: oplock type 0x%x on file %s\n",
		  fsp->oplock_type, fsp_str_dbg(fsp)));
}