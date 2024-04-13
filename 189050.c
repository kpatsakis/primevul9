pam_sm_chauthtok (pam_handle_t *pamh UNUSED, int flags UNUSED,
		  int argc UNUSED, const char **argv UNUSED)
{
  pam_syslog (pamh, LOG_NOTICE, "pam_sm_chauthtok called inappropriately");
  return PAM_SERVICE_ERR;
}