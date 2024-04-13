hook_add_to_infolist_type (struct t_infolist *infolist, int type,
                           const char *arguments)
{
    struct t_hook *ptr_hook;
    struct t_infolist_item *ptr_item;
    char value[64];
    int match;

    for (ptr_hook = weechat_hooks[type]; ptr_hook;
         ptr_hook = ptr_hook->next_hook)
    {
        match = 1;
        if (arguments && !ptr_hook->deleted)
        {
            switch (ptr_hook->type)
            {
                case HOOK_TYPE_COMMAND:
                    match = string_match (HOOK_COMMAND(ptr_hook, command), arguments, 0);
                    break;
                default:
                    break;
            }
        }

        if (!match)
            continue;

        ptr_item = infolist_new_item (infolist);
        if (!ptr_item)
            return 0;

        if (!infolist_new_var_pointer (ptr_item, "pointer", ptr_hook))
            return 0;
        if (!infolist_new_var_pointer (ptr_item, "plugin", ptr_hook->plugin))
            return 0;
        if (!infolist_new_var_string (ptr_item, "plugin_name",
                                      (ptr_hook->plugin) ?
                                      ptr_hook->plugin->name : NULL))
            return 0;
        if (!infolist_new_var_string (ptr_item, "subplugin", ptr_hook->subplugin))
            return 0;
        if (!infolist_new_var_string (ptr_item, "type", hook_type_string[ptr_hook->type]))
            return 0;
        if (!infolist_new_var_integer (ptr_item, "deleted", ptr_hook->deleted))
            return 0;
        if (!infolist_new_var_integer (ptr_item, "running", ptr_hook->running))
            return 0;
        if (!infolist_new_var_integer (ptr_item, "priority", ptr_hook->priority))
            return 0;
        switch (ptr_hook->type)
        {
            case HOOK_TYPE_COMMAND:
                if (!ptr_hook->deleted)
                {
                    if (!infolist_new_var_pointer (ptr_item, "callback", HOOK_COMMAND(ptr_hook, callback)))
                        return 0;
                    if (!infolist_new_var_string (ptr_item, "command", HOOK_COMMAND(ptr_hook, command)))
                        return 0;
                    if (!infolist_new_var_string (ptr_item, "description",
                                                  HOOK_COMMAND(ptr_hook, description)))
                        return 0;
                    if (!infolist_new_var_string (ptr_item, "description_nls",
                                                  (HOOK_COMMAND(ptr_hook, description)
                                                   && HOOK_COMMAND(ptr_hook, description)[0]) ?
                                                  _(HOOK_COMMAND(ptr_hook, description)) : ""))
                        return 0;
                    if (!infolist_new_var_string (ptr_item, "args",
                                                  HOOK_COMMAND(ptr_hook, args)))
                        return 0;
                    if (!infolist_new_var_string (ptr_item, "args_nls",
                                                  (HOOK_COMMAND(ptr_hook, args)
                                                   && HOOK_COMMAND(ptr_hook, args)[0]) ?
                                                  _(HOOK_COMMAND(ptr_hook, args)) : ""))
                        return 0;
                    if (!infolist_new_var_string (ptr_item, "args_description",
                                                  HOOK_COMMAND(ptr_hook, args_description)))
                        return 0;
                    if (!infolist_new_var_string (ptr_item, "args_description_nls",
                                                  (HOOK_COMMAND(ptr_hook, args_description)
                                                   && HOOK_COMMAND(ptr_hook, args_description)[0]) ?
                                                  _(HOOK_COMMAND(ptr_hook, args_description)) : ""))
                        return 0;
                    if (!infolist_new_var_string (ptr_item, "completion", HOOK_COMMAND(ptr_hook, completion)))
                        return 0;
                }
                break;
            case HOOK_TYPE_COMMAND_RUN:
                if (!ptr_hook->deleted)
                {
                    if (!infolist_new_var_pointer (ptr_item, "callback", HOOK_COMMAND_RUN(ptr_hook, callback)))
                        return 0;
                    if (!infolist_new_var_string (ptr_item, "command", HOOK_COMMAND_RUN(ptr_hook, command)))
                        return 0;
                }
                break;
            case HOOK_TYPE_TIMER:
                if (!ptr_hook->deleted)
                {
                    if (!infolist_new_var_pointer (ptr_item, "callback", HOOK_TIMER(ptr_hook, callback)))
                        return 0;
                    snprintf (value, sizeof (value), "%ld", HOOK_TIMER(ptr_hook, interval));
                    if (!infolist_new_var_string (ptr_item, "interval", value))
                        return 0;
                    if (!infolist_new_var_integer (ptr_item, "align_second", HOOK_TIMER(ptr_hook, align_second)))
                        return 0;
                    if (!infolist_new_var_integer (ptr_item, "remaining_calls", HOOK_TIMER(ptr_hook, remaining_calls)))
                        return 0;
                    if (!infolist_new_var_buffer (ptr_item, "last_exec",
                                                  &(HOOK_TIMER(ptr_hook, last_exec)),
                                                  sizeof (HOOK_TIMER(ptr_hook, last_exec))))
                        return 0;
                    if (!infolist_new_var_buffer (ptr_item, "next_exec",
                                                  &(HOOK_TIMER(ptr_hook, next_exec)),
                                                  sizeof (HOOK_TIMER(ptr_hook, next_exec))))
                        return 0;
                }
                break;
            case HOOK_TYPE_FD:
                if (!ptr_hook->deleted)
                {
                    if (!infolist_new_var_pointer (ptr_item, "callback", HOOK_FD(ptr_hook, callback)))
                        return 0;
                    if (!infolist_new_var_integer (ptr_item, "fd", HOOK_FD(ptr_hook, fd)))
                        return 0;
                    if (!infolist_new_var_integer (ptr_item, "flags", HOOK_FD(ptr_hook, flags)))
                        return 0;
                    if (!infolist_new_var_integer (ptr_item, "error", HOOK_FD(ptr_hook, error)))
                        return 0;
                }
                break;
            case HOOK_TYPE_PROCESS:
                if (!ptr_hook->deleted)
                {
                    if (!infolist_new_var_pointer (ptr_item, "callback", HOOK_PROCESS(ptr_hook, callback)))
                        return 0;
                    if (!infolist_new_var_string (ptr_item, "command", HOOK_PROCESS(ptr_hook, command)))
                        return 0;
                    if (!infolist_new_var_string (ptr_item, "options", hashtable_get_string (HOOK_PROCESS(ptr_hook, options), "keys_values")))
                        return 0;
                    if (!infolist_new_var_integer (ptr_item, "timeout", HOOK_PROCESS(ptr_hook, timeout)))
                        return 0;
                    if (!infolist_new_var_integer (ptr_item, "child_read_stdout", HOOK_PROCESS(ptr_hook, child_read[HOOK_PROCESS_STDOUT])))
                        return 0;
                    if (!infolist_new_var_integer (ptr_item, "child_write_stdout", HOOK_PROCESS(ptr_hook, child_write[HOOK_PROCESS_STDOUT])))
                        return 0;
                    if (!infolist_new_var_integer (ptr_item, "child_read_stderr", HOOK_PROCESS(ptr_hook, child_read[HOOK_PROCESS_STDERR])))
                        return 0;
                    if (!infolist_new_var_integer (ptr_item, "child_write_stderr", HOOK_PROCESS(ptr_hook, child_write[HOOK_PROCESS_STDERR])))
                        return 0;
                    if (!infolist_new_var_integer (ptr_item, "child_pid", HOOK_PROCESS(ptr_hook, child_pid)))
                        return 0;
                    if (!infolist_new_var_pointer (ptr_item, "hook_fd_stdout", HOOK_PROCESS(ptr_hook, hook_fd[HOOK_PROCESS_STDOUT])))
                        return 0;
                    if (!infolist_new_var_pointer (ptr_item, "hook_fd_stderr", HOOK_PROCESS(ptr_hook, hook_fd[HOOK_PROCESS_STDERR])))
                        return 0;
                    if (!infolist_new_var_pointer (ptr_item, "hook_timer", HOOK_PROCESS(ptr_hook, hook_timer)))
                        return 0;
                }
                break;
            case HOOK_TYPE_CONNECT:
                if (!ptr_hook->deleted)
                {
                    if (!infolist_new_var_pointer (ptr_item, "callback", HOOK_CONNECT(ptr_hook, callback)))
                        return 0;
                    if (!infolist_new_var_string (ptr_item, "address", HOOK_CONNECT(ptr_hook, address)))
                        return 0;
                    if (!infolist_new_var_integer (ptr_item, "port", HOOK_CONNECT(ptr_hook, port)))
                        return 0;
                    if (!infolist_new_var_integer (ptr_item, "sock", HOOK_CONNECT(ptr_hook, sock)))
                        return 0;
                    if (!infolist_new_var_integer (ptr_item, "ipv6", HOOK_CONNECT(ptr_hook, ipv6)))
                        return 0;
                    if (!infolist_new_var_integer (ptr_item, "retry", HOOK_CONNECT(ptr_hook, retry)))
                        return 0;
#ifdef HAVE_GNUTLS
                    if (!infolist_new_var_pointer (ptr_item, "gnutls_sess", HOOK_CONNECT(ptr_hook, gnutls_sess)))
                        return 0;
                    if (!infolist_new_var_pointer (ptr_item, "gnutls_cb", HOOK_CONNECT(ptr_hook, gnutls_cb)))
                        return 0;
                    if (!infolist_new_var_integer (ptr_item, "gnutls_dhkey_size", HOOK_CONNECT(ptr_hook, gnutls_dhkey_size)))
                        return 0;
#endif
                    if (!infolist_new_var_string (ptr_item, "local_hostname", HOOK_CONNECT(ptr_hook, local_hostname)))
                        return 0;
                    if (!infolist_new_var_integer (ptr_item, "child_read", HOOK_CONNECT(ptr_hook, child_read)))
                        return 0;
                    if (!infolist_new_var_integer (ptr_item, "child_write", HOOK_CONNECT(ptr_hook, child_write)))
                        return 0;
                    if (!infolist_new_var_integer (ptr_item, "child_recv", HOOK_CONNECT(ptr_hook, child_recv)))
                        return 0;
                    if (!infolist_new_var_integer (ptr_item, "child_send", HOOK_CONNECT(ptr_hook, child_send)))
                        return 0;
                    if (!infolist_new_var_integer (ptr_item, "child_pid", HOOK_CONNECT(ptr_hook, child_pid)))
                        return 0;
                    if (!infolist_new_var_pointer (ptr_item, "hook_child_timer", HOOK_CONNECT(ptr_hook, hook_child_timer)))
                        return 0;
                    if (!infolist_new_var_pointer (ptr_item, "hook_fd", HOOK_CONNECT(ptr_hook, hook_fd)))
                        return 0;
                    if (!infolist_new_var_pointer (ptr_item, "handshake_hook_fd", HOOK_CONNECT(ptr_hook, handshake_hook_fd)))
                        return 0;
                    if (!infolist_new_var_pointer (ptr_item, "handshake_hook_timer", HOOK_CONNECT(ptr_hook, handshake_hook_timer)))
                        return 0;
                    if (!infolist_new_var_integer (ptr_item, "handshake_fd_flags", HOOK_CONNECT(ptr_hook, handshake_fd_flags)))
                        return 0;
                    if (!infolist_new_var_string (ptr_item, "handshake_ip_address", HOOK_CONNECT(ptr_hook, handshake_ip_address)))
                        return 0;
                }
                break;
            case HOOK_TYPE_PRINT:
                if (!ptr_hook->deleted)
                {
                    if (!infolist_new_var_pointer (ptr_item, "callback", HOOK_PRINT(ptr_hook, callback)))
                        return 0;
                    if (!infolist_new_var_pointer (ptr_item, "buffer", HOOK_PRINT(ptr_hook, buffer)))
                        return 0;
                    if (!infolist_new_var_integer (ptr_item, "tags_count", HOOK_PRINT(ptr_hook, tags_count)))
                        return 0;
                    if (!infolist_new_var_pointer (ptr_item, "tags_array", HOOK_PRINT(ptr_hook, tags_array)))
                        return 0;
                    if (!infolist_new_var_string (ptr_item, "message", HOOK_PRINT(ptr_hook, message)))
                        return 0;
                    if (!infolist_new_var_integer (ptr_item, "strip_colors", HOOK_PRINT(ptr_hook, strip_colors)))
                        return 0;
                }
                break;
            case HOOK_TYPE_SIGNAL:
                if (!ptr_hook->deleted)
                {
                    if (!infolist_new_var_pointer (ptr_item, "callback", HOOK_SIGNAL(ptr_hook, callback)))
                        return 0;
                    if (!infolist_new_var_string (ptr_item, "signal", HOOK_SIGNAL(ptr_hook, signal)))
                        return 0;
                }
                break;
            case HOOK_TYPE_HSIGNAL:
                if (!ptr_hook->deleted)
                {
                    if (!infolist_new_var_pointer (ptr_item, "callback", HOOK_HSIGNAL(ptr_hook, callback)))
                        return 0;
                    if (!infolist_new_var_string (ptr_item, "signal", HOOK_HSIGNAL(ptr_hook, signal)))
                        return 0;
                }
                break;
            case HOOK_TYPE_CONFIG:
                if (!ptr_hook->deleted)
                {
                    if (!infolist_new_var_pointer (ptr_item, "callback", HOOK_CONFIG(ptr_hook, callback)))
                        return 0;
                    if (!infolist_new_var_string (ptr_item, "option", HOOK_CONFIG(ptr_hook, option)))
                        return 0;
                }
                break;
            case HOOK_TYPE_COMPLETION:
                if (!ptr_hook->deleted)
                {
                    if (!infolist_new_var_pointer (ptr_item, "callback", HOOK_COMPLETION(ptr_hook, callback)))
                        return 0;
                    if (!infolist_new_var_string (ptr_item, "completion_item", HOOK_COMPLETION(ptr_hook, completion_item)))
                        return 0;
                    if (!infolist_new_var_string (ptr_item, "description", HOOK_COMPLETION(ptr_hook, description)))
                        return 0;
                    if (!infolist_new_var_string (ptr_item, "description_nls",
                                                  (HOOK_COMPLETION(ptr_hook, description)
                                                   && HOOK_COMPLETION(ptr_hook, description)[0]) ?
                                                  _(HOOK_COMPLETION(ptr_hook, description)) : ""))
                        return 0;
                }
                break;
            case HOOK_TYPE_MODIFIER:
                if (!ptr_hook->deleted)
                {
                    if (!infolist_new_var_pointer (ptr_item, "callback", HOOK_MODIFIER(ptr_hook, callback)))
                        return 0;
                    if (!infolist_new_var_string (ptr_item, "modifier", HOOK_MODIFIER(ptr_hook, modifier)))
                        return 0;
                }
                break;
            case HOOK_TYPE_INFO:
                if (!ptr_hook->deleted)
                {
                    if (!infolist_new_var_pointer (ptr_item, "callback", HOOK_INFO(ptr_hook, callback)))
                        return 0;
                    if (!infolist_new_var_string (ptr_item, "info_name", HOOK_INFO(ptr_hook, info_name)))
                        return 0;
                    if (!infolist_new_var_string (ptr_item, "description", HOOK_INFO(ptr_hook, description)))
                        return 0;
                    if (!infolist_new_var_string (ptr_item, "description_nls",
                                                  (HOOK_INFO(ptr_hook, description)
                                                   && HOOK_INFO(ptr_hook, description)[0]) ?
                                                  _(HOOK_INFO(ptr_hook, description)) : ""))
                        return 0;
                    if (!infolist_new_var_string (ptr_item, "args_description", HOOK_INFO(ptr_hook, args_description)))
                        return 0;
                    if (!infolist_new_var_string (ptr_item, "args_description_nls",
                                                  (HOOK_INFO(ptr_hook, args_description)
                                                   && HOOK_INFO(ptr_hook, args_description)[0]) ?
                                                  _(HOOK_INFO(ptr_hook, args_description)) : ""))
                        return 0;
                }
                break;
            case HOOK_TYPE_INFO_HASHTABLE:
                if (!ptr_hook->deleted)
                {
                    if (!infolist_new_var_pointer (ptr_item, "callback", HOOK_INFO_HASHTABLE(ptr_hook, callback)))
                        return 0;
                    if (!infolist_new_var_string (ptr_item, "info_name", HOOK_INFO_HASHTABLE(ptr_hook, info_name)))
                        return 0;
                    if (!infolist_new_var_string (ptr_item, "description", HOOK_INFO_HASHTABLE(ptr_hook, description)))
                        return 0;
                    if (!infolist_new_var_string (ptr_item, "description_nls",
                                                  (HOOK_INFO_HASHTABLE(ptr_hook, description)
                                                   && HOOK_INFO_HASHTABLE(ptr_hook, description)[0]) ?
                                                  _(HOOK_INFO_HASHTABLE(ptr_hook, description)) : ""))
                        return 0;
                    if (!infolist_new_var_string (ptr_item, "args_description", HOOK_INFO_HASHTABLE(ptr_hook, args_description)))
                        return 0;
                    if (!infolist_new_var_string (ptr_item, "args_description_nls",
                                                  (HOOK_INFO_HASHTABLE(ptr_hook, args_description)
                                                   && HOOK_INFO_HASHTABLE(ptr_hook, args_description)[0]) ?
                                                  _(HOOK_INFO_HASHTABLE(ptr_hook, args_description)) : ""))
                        return 0;
                    if (!infolist_new_var_string (ptr_item, "output_description", HOOK_INFO_HASHTABLE(ptr_hook, output_description)))
                        return 0;
                    if (!infolist_new_var_string (ptr_item, "output_description_nls",
                                                  (HOOK_INFO_HASHTABLE(ptr_hook, output_description)
                                                   && HOOK_INFO_HASHTABLE(ptr_hook, output_description)[0]) ?
                                                  _(HOOK_INFO_HASHTABLE(ptr_hook, output_description)) : ""))
                        return 0;
                }
                break;
            case HOOK_TYPE_INFOLIST:
                if (!ptr_hook->deleted)
                {
                    if (!infolist_new_var_pointer (ptr_item, "callback", HOOK_INFOLIST(ptr_hook, callback)))
                        return 0;
                    if (!infolist_new_var_string (ptr_item, "infolist_name", HOOK_INFOLIST(ptr_hook, infolist_name)))
                        return 0;
                    if (!infolist_new_var_string (ptr_item, "description", HOOK_INFOLIST(ptr_hook, description)))
                        return 0;
                    if (!infolist_new_var_string (ptr_item, "description_nls",
                                                  (HOOK_INFOLIST(ptr_hook, description)
                                                   && HOOK_INFOLIST(ptr_hook, description)[0]) ?
                                                  _(HOOK_INFOLIST(ptr_hook, description)) : ""))
                        return 0;
                    if (!infolist_new_var_string (ptr_item, "pointer_description", HOOK_INFOLIST(ptr_hook, pointer_description)))
                        return 0;
                    if (!infolist_new_var_string (ptr_item, "pointer_description_nls",
                                                  (HOOK_INFOLIST(ptr_hook, pointer_description)
                                                   && HOOK_INFOLIST(ptr_hook, pointer_description)[0]) ?
                                                  _(HOOK_INFOLIST(ptr_hook, pointer_description)) : ""))
                        return 0;
                    if (!infolist_new_var_string (ptr_item, "args_description", HOOK_INFOLIST(ptr_hook, args_description)))
                        return 0;
                    if (!infolist_new_var_string (ptr_item, "args_description_nls",
                                                  (HOOK_INFOLIST(ptr_hook, args_description)
                                                   && HOOK_INFOLIST(ptr_hook, args_description)[0]) ?
                                                  _(HOOK_INFOLIST(ptr_hook, args_description)) : ""))
                        return 0;
                }
                break;
            case HOOK_TYPE_HDATA:
                if (!ptr_hook->deleted)
                {
                    if (!infolist_new_var_pointer (ptr_item, "callback", HOOK_HDATA(ptr_hook, callback)))
                        return 0;
                    if (!infolist_new_var_string (ptr_item, "hdata_name", HOOK_HDATA(ptr_hook, hdata_name)))
                        return 0;
                    if (!infolist_new_var_string (ptr_item, "description", HOOK_HDATA(ptr_hook, description)))
                        return 0;
                    if (!infolist_new_var_string (ptr_item, "description_nls",
                                                  (HOOK_HDATA(ptr_hook, description)
                                                   && HOOK_HDATA(ptr_hook, description)[0]) ?
                                                  _(HOOK_HDATA(ptr_hook, description)) : ""))
                        return 0;
                }
                break;
            case HOOK_TYPE_FOCUS:
                if (!ptr_hook->deleted)
                {
                    if (!infolist_new_var_pointer (ptr_item, "callback", HOOK_FOCUS(ptr_hook, callback)))
                        return 0;
                    if (!infolist_new_var_string (ptr_item, "area", HOOK_FOCUS(ptr_hook, area)))
                        return 0;
                }
                break;
            case HOOK_NUM_TYPES:
                /*
                 * this constant is used to count types only,
                 * it is never used as type
                 */
                break;
        }
    }

    return 1;
}