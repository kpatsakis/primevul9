static int get_pw_from_env(struct parsed_mount_info *parsed_info, const char *program)
{
	int rc = 0;

	if (getenv("PASSWD"))
		rc = set_password(parsed_info, getenv("PASSWD"));
	else if (getenv("PASSWD_FD"))
		rc = get_password_from_file(atoi(getenv("PASSWD_FD")), NULL,
					    parsed_info, program);
	else if (getenv("PASSWD_FILE"))
		rc = get_password_from_file(0, getenv("PASSWD_FILE"),
					    parsed_info, program);

	return rc;
}