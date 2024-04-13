pam_sm_authenticate (pam_handle_t *pamh UNUSED, int flags UNUSED,
		     int argc UNUSED, const char **argv UNUSED)
{
  return PAM_IGNORE;
}