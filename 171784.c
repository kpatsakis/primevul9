static void _pam_winbind_cleanup_func(pam_handle_t *pamh,
				      void *data,
				      int error_status)
{
	int ctrl = _pam_parse(pamh, 0, 0, NULL, NULL);
	if (_pam_log_is_debug_state_enabled(ctrl)) {
		__pam_log_debug(pamh, ctrl, LOG_DEBUG,
			       "[pamh: %p] CLEAN: cleaning up PAM data %p "
			       "(error_status = %d)", pamh, data,
			       error_status);
	}
	TALLOC_FREE(data);
}