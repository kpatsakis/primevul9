help(void)
{
    struct sudo_lbuf lbuf;
    const int indent = 32;
    const char *pname = getprogname();
    debug_decl(help, SUDO_DEBUG_ARGS);

    sudo_lbuf_init(&lbuf, usage_out, indent, NULL, user_details.ts_cols);
    if (strcmp(pname, "sudoedit") == 0)
	sudo_lbuf_append(&lbuf, _("%s - edit files as another user\n\n"), pname);
    else
	sudo_lbuf_append(&lbuf, _("%s - execute a command as another user\n\n"), pname);
    sudo_lbuf_print(&lbuf);

    display_usage(usage_out);

    sudo_lbuf_append(&lbuf, "%s", _("\nOptions:\n"));
    sudo_lbuf_append(&lbuf, "  -A, --askpass                 %s\n",
	_("use a helper program for password prompting"));
#ifdef HAVE_BSD_AUTH_H
    sudo_lbuf_append(&lbuf, "  -a, --auth-type=type          %s\n",
	_("use specified BSD authentication type"));
#endif
    sudo_lbuf_append(&lbuf, "  -b, --background              %s\n",
	_("run command in the background"));
    sudo_lbuf_append(&lbuf, "  -B, --bell                    %s\n",
	_("ring bell when prompting"));
    sudo_lbuf_append(&lbuf, "  -C, --close-from=num          %s\n",
	_("close all file descriptors >= num"));
#ifdef HAVE_LOGIN_CAP_H
    sudo_lbuf_append(&lbuf, "  -c, --login-class=class       %s\n",
	_("run command with the specified BSD login class"));
#endif
    sudo_lbuf_append(&lbuf, "  -D, --chdir=directory         %s\n",
	_("change the working directory before running command"));
    sudo_lbuf_append(&lbuf, "  -E, --preserve-env            %s\n",
	_("preserve user environment when running command"));
    sudo_lbuf_append(&lbuf, "      --preserve-env=list       %s\n",
	_("preserve specific environment variables"));
    sudo_lbuf_append(&lbuf, "  -e, --edit                    %s\n",
	_("edit files instead of running a command"));
    sudo_lbuf_append(&lbuf, "  -g, --group=group             %s\n",
	_("run command as the specified group name or ID"));
    sudo_lbuf_append(&lbuf, "  -H, --set-home                %s\n",
	_("set HOME variable to target user's home dir"));
    sudo_lbuf_append(&lbuf, "  -h, --help                    %s\n",
	_("display help message and exit"));
    sudo_lbuf_append(&lbuf, "  -h, --host=host               %s\n",
	_("run command on host (if supported by plugin)"));
    sudo_lbuf_append(&lbuf, "  -i, --login                   %s\n",
	_("run login shell as the target user; a command may also be specified"));
    sudo_lbuf_append(&lbuf, "  -K, --remove-timestamp        %s\n",
	_("remove timestamp file completely"));
    sudo_lbuf_append(&lbuf, "  -k, --reset-timestamp         %s\n",
	_("invalidate timestamp file"));
    sudo_lbuf_append(&lbuf, "  -l, --list                    %s\n",
	_("list user's privileges or check a specific command; use twice for longer format"));
    sudo_lbuf_append(&lbuf, "  -n, --non-interactive         %s\n",
	_("non-interactive mode, no prompts are used"));
    sudo_lbuf_append(&lbuf, "  -P, --preserve-groups         %s\n",
	_("preserve group vector instead of setting to target's"));
    sudo_lbuf_append(&lbuf, "  -p, --prompt=prompt           %s\n",
	_("use the specified password prompt"));
    sudo_lbuf_append(&lbuf, "  -R, --chroot=directory        %s\n",
	_("change the root directory before running command"));
#ifdef HAVE_SELINUX
    sudo_lbuf_append(&lbuf, "  -r, --role=role               %s\n",
	_("create SELinux security context with specified role"));
#endif
    sudo_lbuf_append(&lbuf, "  -S, --stdin                   %s\n",
	_("read password from standard input"));
    sudo_lbuf_append(&lbuf, "  -s, --shell                   %s\n",
	_("run shell as the target user; a command may also be specified"));
#ifdef HAVE_SELINUX
    sudo_lbuf_append(&lbuf, "  -t, --type=type               %s\n",
	_("create SELinux security context with specified type"));
#endif
    sudo_lbuf_append(&lbuf, "  -T, --command-timeout=timeout %s\n",
	_("terminate command after the specified time limit"));
    sudo_lbuf_append(&lbuf, "  -U, --other-user=user         %s\n",
	_("in list mode, display privileges for user"));
    sudo_lbuf_append(&lbuf, "  -u, --user=user               %s\n",
	_("run command (or edit file) as specified user name or ID"));
    sudo_lbuf_append(&lbuf, "  -V, --version                 %s\n",
	_("display version information and exit"));
    sudo_lbuf_append(&lbuf, "  -v, --validate                %s\n",
	_("update user's timestamp without running a command"));
    sudo_lbuf_append(&lbuf, "  --                            %s\n",
	_("stop processing command line arguments"));
    sudo_lbuf_print(&lbuf);
    sudo_lbuf_destroy(&lbuf);
    sudo_debug_exit_int(__func__, __FILE__, __LINE__, sudo_debug_subsys, 0);
    exit(EXIT_SUCCESS);
}