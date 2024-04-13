grub_normal_auth_init (void)
{
  cmd = grub_register_command ("authenticate",
			       grub_cmd_authenticate,
			       N_("[USERLIST]"),
			       N_("Check whether user is in USERLIST."));

}