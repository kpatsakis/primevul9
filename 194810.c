hook_process (struct t_weechat_plugin *plugin,
              const char *command, int timeout,
              t_hook_callback_process *callback, void *callback_data)
{
    return hook_process_hashtable (plugin, command, NULL, timeout,
                                   callback, callback_data);
}