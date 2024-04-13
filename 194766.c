hook_command_run_exec (struct t_gui_buffer *buffer, const char *command)
{
    struct t_hook *ptr_hook, *next_hook;
    int rc, hook_matching, length;
    char *command2;
    const char *ptr_command;

    ptr_command = command;
    command2 = NULL;

    if (command[0] != '/')
    {
        length = strlen (command) + 1;
        command2 = malloc (length);
        if (command2)
        {
            snprintf (command2, length, "/%s", command + 1);
            ptr_command = command2;
        }
    }

    ptr_hook = weechat_hooks[HOOK_TYPE_COMMAND_RUN];
    while (ptr_hook)
    {
        next_hook = ptr_hook->next_hook;

        if (!ptr_hook->deleted
            && !ptr_hook->running
            && HOOK_COMMAND_RUN(ptr_hook, command))
        {
            hook_matching = string_match (ptr_command,
                                          HOOK_COMMAND_RUN(ptr_hook, command),
                                          0);

            if (!hook_matching
                && !strchr (HOOK_COMMAND_RUN(ptr_hook, command), ' '))
            {
                hook_matching = (string_strncasecmp (ptr_command,
                                                     HOOK_COMMAND_RUN(ptr_hook, command),
                                                     utf8_strlen (HOOK_COMMAND_RUN(ptr_hook, command))) == 0);
            }

            if (hook_matching)
            {
                ptr_hook->running = 1;
                rc = (HOOK_COMMAND_RUN(ptr_hook, callback)) (ptr_hook->callback_data,
                                                             buffer,
                                                             ptr_command);
                ptr_hook->running = 0;
                if (rc == WEECHAT_RC_OK_EAT)
                {
                    if (command2)
                        free (command2);
                    return rc;
                }
            }
        }

        ptr_hook = next_hook;
    }

    if (command2)
        free (command2);

    return WEECHAT_RC_OK;
}