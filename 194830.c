hook_init_data (struct t_hook *hook, struct t_weechat_plugin *plugin,
                int type, int priority, void *callback_data)
{
    hook->plugin = plugin;
    hook->subplugin = NULL;
    hook->type = type;
    hook->deleted = 0;
    hook->running = 0;
    hook->priority = priority;
    hook->callback_data = callback_data;
    hook->hook_data = NULL;

    if (weechat_debug_core >= 2)
    {
        gui_chat_printf (NULL,
                         "debug: adding hook: type=%d (%s), plugin=%lx (%s), "
                         "priority=%d",
                         hook->type, hook_type_string[hook->type],
                         hook->plugin, plugin_get_name (hook->plugin),
                         hook->priority);
    }
}