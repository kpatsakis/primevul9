hook_command (struct t_weechat_plugin *plugin, const char *command,
              const char *description,
              const char *args, const char *args_description,
              const char *completion,
              t_hook_callback_command *callback, void *callback_data)
{
    struct t_hook *new_hook;
    struct t_hook_command *new_hook_command;
    int priority;
    const char *ptr_command;

    if (!callback)
        return NULL;

    if (hook_search_command (plugin, command))
    {
        gui_chat_printf (NULL,
                         _("%sError: another command \"%s\" already exists "
                           "for plugin \"%s\""),
                         gui_chat_prefix[GUI_CHAT_PREFIX_ERROR],
                         command,
                         plugin_get_name (plugin));
        return NULL;
    }

    new_hook = malloc (sizeof (*new_hook));
    if (!new_hook)
        return NULL;
    new_hook_command = malloc (sizeof (*new_hook_command));
    if (!new_hook_command)
    {
        free (new_hook);
        return NULL;
    }

    hook_get_priority_and_name (command, &priority, &ptr_command);
    hook_init_data (new_hook, plugin, HOOK_TYPE_COMMAND, priority,
                    callback_data);

    new_hook->hook_data = new_hook_command;
    new_hook_command->callback = callback;
    new_hook_command->command = strdup ((ptr_command) ? ptr_command :
                                        ((command) ? command : ""));
    new_hook_command->description = strdup ((description) ? description : "");
    new_hook_command->args = strdup ((args) ? args : "");
    new_hook_command->args_description = strdup ((args_description) ?
                                                 args_description : "");
    new_hook_command->completion = strdup ((completion) ? completion : "");

    /* build completion variables for command */
    new_hook_command->cplt_num_templates = 0;
    new_hook_command->cplt_templates = NULL;
    new_hook_command->cplt_templates_static = NULL;
    new_hook_command->cplt_template_num_args = NULL;
    new_hook_command->cplt_template_args = NULL;
    new_hook_command->cplt_template_num_args_concat = 0;
    new_hook_command->cplt_template_args_concat = NULL;
    hook_command_build_completion (new_hook_command);

    hook_add_to_list (new_hook);

    return new_hook;
}