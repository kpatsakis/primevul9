static bool _pam_log_is_debug_enabled(int ctrl)
{
	if (ctrl == -1) {
		return false;
	}

	if (_pam_log_is_silent(ctrl)) {
		return false;
	}

	if (!(ctrl & WINBIND_DEBUG_ARG)) {
		return false;
	}

	return true;
}