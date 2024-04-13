static bool housekeeping_fn(const struct timeval *now, void *private_data)
{
	change_to_root_user();

	/* update printer queue caches if necessary */
	update_monitored_printq_cache();

	/* check if we need to reload services */
	check_reload(time(NULL));

	/* Change machine password if neccessary. */
	attempt_machine_password_change();

        /*
	 * Force a log file check.
	 */
	force_check_log_size();
	check_log_size();
	return true;
}