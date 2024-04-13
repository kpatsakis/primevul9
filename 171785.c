static bool _pam_log_is_silent(int ctrl)
{
	return on(ctrl, WINBIND_SILENT);
}