hook_command_exec (struct t_gui_buffer *buffer, int any_plugin,
                   struct t_weechat_plugin *plugin, const char *string)
{
    struct t_hook *ptr_hook, *next_hook;
    struct t_hook *hook_for_plugin, *hook_for_other_plugin;
    char **argv, **argv_eol, *ptr_command_name;
    int argc, rc, number_for_other_plugin;

    if (!buffer || !string || !string[0])
        return -1;

    rc = hook_command_run_exec (buffer, string);
    if (rc == WEECHAT_RC_OK_EAT)
        return 1;

    rc = -1;

    argv = string_split (string, " ", 0, 0, &argc);
    if (argc == 0)
    {
        string_free_split (argv);
        return -1;
    }
    argv_eol = string_split (string, " ", 1, 0, NULL);

    ptr_command_name = utf8_next_char (argv[0]);

    hook_exec_start ();

    hook_for_plugin = NULL;
    hook_for_other_plugin = NULL;
    number_for_other_plugin = 0;
    ptr_hook = weechat_hooks[HOOK_TYPE_COMMAND];
    while (ptr_hook)
    {
        next_hook = ptr_hook->next_hook;

        if (!ptr_hook->deleted
            && (string_strcasecmp (ptr_command_name,
                                   HOOK_COMMAND(ptr_hook, command)) == 0))
        {
            if (ptr_hook->plugin == plugin)
            {
                if (!hook_for_plugin)
                    hook_for_plugin = ptr_hook;
            }
            else
            {
                if (any_plugin)
                {
                    if (!hook_for_other_plugin)
                        hook_for_other_plugin = ptr_hook;
                    number_for_other_plugin++;
                }
            }
        }

        ptr_hook = next_hook;
    }

    if (!hook_for_plugin && !hook_for_other_plugin)
    {
        /* command not found */
        rc = -1;
    }
    else
    {
        if (!hook_for_plugin && (number_for_other_plugin > 1))
        {
            /*
             * ambiguous: no command for current plugin, but more than one
             * command was found for other plugins, we don't know which one to
             * run!
             */
            rc = -2;
        }
        else
        {
            ptr_hook = (hook_for_plugin) ?
                hook_for_plugin : hook_for_other_plugin;

            if (ptr_hook->running >= HOOK_COMMAND_MAX_CALLS)
                rc = -3;
            else
            {
                ptr_hook->running++;
                rc = (int) (HOOK_COMMAND(ptr_hook, callback))
                    (ptr_hook->callback_data, buffer, argc, argv, argv_eol);
                ptr_hook->running--;
                if (rc == WEECHAT_RC_ERROR)
                    rc = 0;
                else
                    rc = 1;
            }
        }
    }

    string_free_split (argv);
    string_free_split (argv_eol);

    hook_exec_end ();

    return rc;
}