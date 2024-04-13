grub_cmd_authenticate (struct grub_command *cmd __attribute__ ((unused)),
		       int argc, char **args)
{
  return grub_auth_check_authentication ((argc >= 1) ? args[0] : "");
}