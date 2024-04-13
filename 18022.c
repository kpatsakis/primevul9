static bool validate_oplock_types(struct share_mode_lock *lck)
{
	struct share_mode_data *d = lck->data;
	bool batch = false;
	bool ex_or_batch = false;
	bool level2 = false;
	bool no_oplock = false;
	uint32_t num_non_stat_opens = 0;
	uint32_t i;

	for (i=0; i<d->num_share_modes; i++) {
		struct share_mode_entry *e = &d->share_modes[i];

		if (!is_valid_share_mode_entry(e)) {
			continue;
		}

		if (e->op_type == NO_OPLOCK && is_stat_open(e->access_mask)) {
			/* We ignore stat opens in the table - they
			   always have NO_OPLOCK and never get or
			   cause breaks. JRA. */
			continue;
		}

		num_non_stat_opens += 1;

		if (BATCH_OPLOCK_TYPE(e->op_type)) {
			/* batch - can only be one. */
			if (share_mode_stale_pid(d, i)) {
				DEBUG(10, ("Found stale batch oplock\n"));
				continue;
			}
			if (ex_or_batch || batch || level2 || no_oplock) {
				DEBUG(0, ("Bad batch oplock entry %u.",
					  (unsigned)i));
				return false;
			}
			batch = true;
		}

		if (EXCLUSIVE_OPLOCK_TYPE(e->op_type)) {
			if (share_mode_stale_pid(d, i)) {
				DEBUG(10, ("Found stale duplicate oplock\n"));
				continue;
			}
			/* Exclusive or batch - can only be one. */
			if (ex_or_batch || level2 || no_oplock) {
				DEBUG(0, ("Bad exclusive or batch oplock "
					  "entry %u.", (unsigned)i));
				return false;
			}
			ex_or_batch = true;
		}

		if (LEVEL_II_OPLOCK_TYPE(e->op_type)) {
			if (batch || ex_or_batch) {
				if (share_mode_stale_pid(d, i)) {
					DEBUG(10, ("Found stale LevelII "
						   "oplock\n"));
					continue;
				}
				DEBUG(0, ("Bad levelII oplock entry %u.",
					  (unsigned)i));
				return false;
			}
			level2 = true;
		}

		if (e->op_type == NO_OPLOCK) {
			if (batch || ex_or_batch) {
				if (share_mode_stale_pid(d, i)) {
					DEBUG(10, ("Found stale NO_OPLOCK "
						   "entry\n"));
					continue;
				}
				DEBUG(0, ("Bad no oplock entry %u.",
					  (unsigned)i));
				return false;
			}
			no_oplock = true;
		}
	}

	remove_stale_share_mode_entries(d);

	if ((batch || ex_or_batch) && (num_non_stat_opens != 1)) {
		DEBUG(1, ("got batch (%d) or ex (%d) non-exclusively (%d)\n",
			  (int)batch, (int)ex_or_batch,
			  (int)d->num_share_modes));
		return false;
	}

	return true;
}