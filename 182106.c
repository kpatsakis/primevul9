static void fill_ssh_args(struct child_process *conn, const char *ssh_host,
			  const char *port, enum protocol_version version,
			  int flags)
{
	const char *ssh;
	enum ssh_variant variant;

	if (looks_like_command_line_option(ssh_host))
		die(_("strange hostname '%s' blocked"), ssh_host);

	ssh = get_ssh_command();
	if (ssh) {
		variant = determine_ssh_variant(ssh, 1);
	} else {
		/*
		 * GIT_SSH is the no-shell version of
		 * GIT_SSH_COMMAND (and must remain so for
		 * historical compatibility).
		 */
		conn->use_shell = 0;

		ssh = getenv("GIT_SSH");
		if (!ssh)
			ssh = "ssh";
		variant = determine_ssh_variant(ssh, 0);
	}

	if (variant == VARIANT_AUTO) {
		struct child_process detect = CHILD_PROCESS_INIT;

		detect.use_shell = conn->use_shell;
		detect.no_stdin = detect.no_stdout = detect.no_stderr = 1;

		argv_array_push(&detect.args, ssh);
		argv_array_push(&detect.args, "-G");
		push_ssh_options(&detect.args, &detect.env_array,
				 VARIANT_SSH, port, version, flags);
		argv_array_push(&detect.args, ssh_host);

		variant = run_command(&detect) ? VARIANT_SIMPLE : VARIANT_SSH;
	}

	argv_array_push(&conn->args, ssh);
	push_ssh_options(&conn->args, &conn->env_array, variant, port, version, flags);
	argv_array_push(&conn->args, ssh_host);
}