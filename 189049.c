pam_sm_close_session (pam_handle_t *pamh UNUSED, int flags UNUSED,
		      int argc UNUSED, const char **argv UNUSED)
{
  D(("Called and Exit"));
  return PAM_SUCCESS;
}