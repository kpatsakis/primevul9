static bool has_delete_on_close(struct share_mode_lock *lck,
				uint32_t name_hash)
{
	struct share_mode_data *d = lck->data;
	uint32_t i;

	if (d->num_share_modes == 0) {
		return false;
	}
	if (!is_delete_on_close_set(lck, name_hash)) {
		return false;
	}
	for (i=0; i<d->num_share_modes; i++) {
		if (!share_mode_stale_pid(d, i)) {
			return true;
		}
	}
	return false;
}