hook_command_run (struct t_weechat_plugin *plugin, const char *command,
                  t_hook_callback_command_run *callback, void *callback_data)
{
    struct t_hook *new_hook;
    struct t_hook_command_run *new_hook_command_run;
    int priority;
    const char *ptr_command;

    if (!callback)
        return NULL;

    new_hook = malloc (sizeof (*new_hook));
    if (!new_hook)
        return NULL;
    new_hook_command_run = malloc (sizeof (*new_hook_command_run));
    if (!new_hook_command_run)
    {
        free (new_hook);
        return NULL;
    }

    hook_get_priority_and_name (command, &priority, &ptr_command);
    hook_init_data (new_hook, plugin, HOOK_TYPE_COMMAND_RUN, priority,
                    callback_data);

    new_hook->hook_data = new_hook_command_run;
    new_hook_command_run->callback = callback;
    new_hook_command_run->command = strdup ((ptr_command) ? ptr_command :
                                            ((command) ? command : ""));

    hook_add_to_list (new_hook);

    return new_hook;
}