static bool delay_for_oplock(files_struct *fsp,
			     int oplock_request,
			     struct share_mode_lock *lck,
			     bool have_sharing_violation,
			     uint32_t create_disposition)
{
	struct share_mode_data *d = lck->data;
	struct share_mode_entry *entry;
	uint32_t num_non_stat_opens = 0;
	uint32_t i;
	uint16_t break_to;

	if ((oplock_request & INTERNAL_OPEN_ONLY) || is_stat_open(fsp->access_mask)) {
		return false;
	}
	for (i=0; i<d->num_share_modes; i++) {
		struct share_mode_entry *e = &d->share_modes[i];
		if (e->op_type == NO_OPLOCK && is_stat_open(e->access_mask)) {
			continue;
		}
		num_non_stat_opens += 1;

		/*
		 * We found the a non-stat open, which in the exclusive/batch
		 * case will be inspected further down.
		 */
		entry = e;
	}
	if (num_non_stat_opens == 0) {
		/*
		 * Nothing to wait for around
		 */
		return false;
	}
	if (num_non_stat_opens != 1) {
		/*
		 * More than one open around. There can't be any exclusive or
		 * batch left, this is all level2.
		 */
		return false;
	}

	if (server_id_is_disconnected(&entry->pid)) {
		/*
		 * TODO: clean up.
		 * This could be achieved by sending a break message
		 * to ourselves. Special considerations for files
		 * with delete_on_close flag set!
		 *
		 * For now we keep it simple and do not
		 * allow delete on close for durable handles.
		 */
		return false;
	}

	switch (create_disposition) {
	case FILE_SUPERSEDE:
	case FILE_OVERWRITE_IF:
		break_to = NO_OPLOCK;
		break;
	default:
		break_to = LEVEL_II_OPLOCK;
		break;
	}

	if (have_sharing_violation && (entry->op_type & BATCH_OPLOCK)) {
		if (share_mode_stale_pid(d, 0)) {
			return false;
		}
		send_break_message(fsp->conn->sconn->msg_ctx, entry, break_to);
		return true;
	}
	if (have_sharing_violation) {
		/*
		 * Non-batch exclusive is not broken if we have a sharing
		 * violation
		 */
		return false;
	}
	if (LEVEL_II_OPLOCK_TYPE(entry->op_type) &&
	    (break_to == NO_OPLOCK)) {
		if (share_mode_stale_pid(d, 0)) {
			return false;
		}
		DEBUG(10, ("Asynchronously breaking level2 oplock for "
			   "create_disposition=%u\n",
			   (unsigned)create_disposition));
		send_break_message(fsp->conn->sconn->msg_ctx, entry, break_to);
		return false;
	}
	if (!EXCLUSIVE_OPLOCK_TYPE(entry->op_type)) {
		/*
		 * No break for NO_OPLOCK or LEVEL2_OPLOCK oplocks
		 */
		return false;
	}
	if (share_mode_stale_pid(d, 0)) {
		return false;
	}

	send_break_message(fsp->conn->sconn->msg_ctx, entry, break_to);
	return true;
}