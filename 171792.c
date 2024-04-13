static bool _pam_log_is_debug_state_enabled(int ctrl)
{
	if (!(ctrl & WINBIND_DEBUG_STATE)) {
		return false;
	}

	return _pam_log_is_debug_enabled(ctrl);
}