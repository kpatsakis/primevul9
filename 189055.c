pam_sm_setcred (pam_handle_t *pamh, int flags UNUSED,
		int argc, const char **argv)
{
  D(("Called"));
  return handle_env (pamh, argc, argv);
}