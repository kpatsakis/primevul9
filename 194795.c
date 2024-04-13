unhook (struct t_hook *hook)
{
    int i;

    /* invalid hook? */
    if (!hook_valid (hook))
        return;

    /* hook already deleted? */
    if (hook->deleted)
        return;

    if (weechat_debug_core >= 2)
    {
        gui_chat_printf (NULL,
                         "debug: removing hook: type=%d (%s), plugin=%lx (%s)",
                         hook->type, hook_type_string[hook->type],
                         hook->plugin, plugin_get_name (hook->plugin));
    }

    /* free data */
    if (hook->subplugin)
        free (hook->subplugin);
    if (hook->hook_data)
    {
        switch (hook->type)
        {
            case HOOK_TYPE_COMMAND:
                if (HOOK_COMMAND(hook, command))
                    free (HOOK_COMMAND(hook, command));
                if (HOOK_COMMAND(hook, description))
                    free (HOOK_COMMAND(hook, description));
                if (HOOK_COMMAND(hook, args))
                    free (HOOK_COMMAND(hook, args));
                if (HOOK_COMMAND(hook, args_description))
                    free (HOOK_COMMAND(hook, args_description));
                if (HOOK_COMMAND(hook, completion))
                    free (HOOK_COMMAND(hook, completion));
                if (HOOK_COMMAND(hook, cplt_templates))
                {
                    for (i = 0; i < HOOK_COMMAND(hook, cplt_num_templates); i++)
                    {
                        if (HOOK_COMMAND(hook, cplt_templates)[i])
                            free (HOOK_COMMAND(hook, cplt_templates)[i]);
                        if (HOOK_COMMAND(hook, cplt_templates_static)[i])
                            free (HOOK_COMMAND(hook, cplt_templates_static)[i]);
                        string_free_split (HOOK_COMMAND(hook, cplt_template_args)[i]);
                    }
                    free (HOOK_COMMAND(hook, cplt_templates));
                }
                if (HOOK_COMMAND(hook, cplt_templates_static))
                    free (HOOK_COMMAND(hook, cplt_templates_static));
                if (HOOK_COMMAND(hook, cplt_template_num_args))
                    free (HOOK_COMMAND(hook, cplt_template_num_args));
                if (HOOK_COMMAND(hook, cplt_template_args))
                    free (HOOK_COMMAND(hook, cplt_template_args));
                if (HOOK_COMMAND(hook, cplt_template_args_concat))
                {
                    for (i = 0;
                         i < HOOK_COMMAND(hook, cplt_template_num_args_concat);
                         i++)
                    {
                        free (HOOK_COMMAND(hook, cplt_template_args_concat[i]));
                    }
                    free (HOOK_COMMAND(hook, cplt_template_args_concat));
                }
                break;
            case HOOK_TYPE_COMMAND_RUN:
                if (HOOK_COMMAND_RUN(hook, command))
                    free (HOOK_COMMAND_RUN(hook, command));
                break;
            case HOOK_TYPE_TIMER:
                break;
            case HOOK_TYPE_FD:
                break;
            case HOOK_TYPE_PROCESS:
                if (HOOK_PROCESS(hook, command))
                    free (HOOK_PROCESS(hook, command));
                if (HOOK_PROCESS(hook, options))
                    hashtable_free (HOOK_PROCESS(hook, options));
                if (HOOK_PROCESS(hook, hook_fd[HOOK_PROCESS_STDOUT]))
                    unhook (HOOK_PROCESS(hook, hook_fd[HOOK_PROCESS_STDOUT]));
                if (HOOK_PROCESS(hook, hook_fd[HOOK_PROCESS_STDERR]))
                    unhook (HOOK_PROCESS(hook, hook_fd[HOOK_PROCESS_STDERR]));
                if (HOOK_PROCESS(hook, hook_timer))
                    unhook (HOOK_PROCESS(hook, hook_timer));
                if (HOOK_PROCESS(hook, child_pid) > 0)
                {
                    kill (HOOK_PROCESS(hook, child_pid), SIGKILL);
                    waitpid (HOOK_PROCESS(hook, child_pid), NULL, 0);
                }
                if (HOOK_PROCESS(hook, child_read[HOOK_PROCESS_STDOUT]) != -1)
                    close (HOOK_PROCESS(hook, child_read[HOOK_PROCESS_STDOUT]));
                if (HOOK_PROCESS(hook, child_write[HOOK_PROCESS_STDOUT]) != -1)
                    close (HOOK_PROCESS(hook, child_write[HOOK_PROCESS_STDOUT]));
                if (HOOK_PROCESS(hook, child_read[HOOK_PROCESS_STDERR]) != -1)
                    close (HOOK_PROCESS(hook, child_read[HOOK_PROCESS_STDERR]));
                if (HOOK_PROCESS(hook, child_write[HOOK_PROCESS_STDERR]) != -1)
                    close (HOOK_PROCESS(hook, child_write[HOOK_PROCESS_STDERR]));
                if (HOOK_PROCESS(hook, buffer[HOOK_PROCESS_STDOUT]))
                    free (HOOK_PROCESS(hook, buffer[HOOK_PROCESS_STDOUT]));
                if (HOOK_PROCESS(hook, buffer[HOOK_PROCESS_STDERR]))
                    free (HOOK_PROCESS(hook, buffer[HOOK_PROCESS_STDERR]));
                break;
            case HOOK_TYPE_CONNECT:
                if (HOOK_CONNECT(hook, proxy))
                    free (HOOK_CONNECT(hook, proxy));
                if (HOOK_CONNECT(hook, address))
                    free (HOOK_CONNECT(hook, address));
#ifdef HAVE_GNUTLS
                if (HOOK_CONNECT(hook, gnutls_priorities))
                    free (HOOK_CONNECT(hook, gnutls_priorities));
#endif
                if (HOOK_CONNECT(hook, local_hostname))
                    free (HOOK_CONNECT(hook, local_hostname));
                if (HOOK_CONNECT(hook, hook_child_timer))
                    unhook (HOOK_CONNECT(hook, hook_child_timer));
                if (HOOK_CONNECT(hook, hook_fd))
                    unhook (HOOK_CONNECT(hook, hook_fd));
                if (HOOK_CONNECT(hook, handshake_hook_fd))
                    unhook (HOOK_CONNECT(hook, handshake_hook_fd));
                if (HOOK_CONNECT(hook, handshake_hook_timer))
                    unhook (HOOK_CONNECT(hook, handshake_hook_timer));
                if (HOOK_CONNECT(hook, handshake_ip_address))
                    free (HOOK_CONNECT(hook, handshake_ip_address));
                if (HOOK_CONNECT(hook, child_pid) > 0)
                {
                    kill (HOOK_CONNECT(hook, child_pid), SIGKILL);
                    waitpid (HOOK_CONNECT(hook, child_pid), NULL, 0);
                }
                if (HOOK_CONNECT(hook, child_read) != -1)
                    close (HOOK_CONNECT(hook, child_read));
                if (HOOK_CONNECT(hook, child_write) != -1)
                    close (HOOK_CONNECT(hook, child_write));
                if (HOOK_CONNECT(hook, child_recv) != -1)
                    close (HOOK_CONNECT(hook, child_recv));
                if (HOOK_CONNECT(hook, child_send) != -1)
                    close (HOOK_CONNECT(hook, child_send));
#ifdef HOOK_CONNECT_MAX_SOCKETS
                for (i = 0; i < HOOK_CONNECT_MAX_SOCKETS; i++)
                {
                    if (HOOK_CONNECT(hook, sock_v4[i]) != -1)
                        close (HOOK_CONNECT(hook, sock_v4[i]));
                    if (HOOK_CONNECT(hook, sock_v6[i]) != -1)
                        close (HOOK_CONNECT(hook, sock_v6[i]));
                }
#endif
                break;
            case HOOK_TYPE_PRINT:
                if (HOOK_PRINT(hook, message))
                    free (HOOK_PRINT(hook, message));
                break;
            case HOOK_TYPE_SIGNAL:
                if (HOOK_SIGNAL(hook, signal))
                    free (HOOK_SIGNAL(hook, signal));
                break;
            case HOOK_TYPE_HSIGNAL:
                if (HOOK_HSIGNAL(hook, signal))
                    free (HOOK_HSIGNAL(hook, signal));
                break;
            case HOOK_TYPE_CONFIG:
                if (HOOK_CONFIG(hook, option))
                    free (HOOK_CONFIG(hook, option));
                break;
            case HOOK_TYPE_COMPLETION:
                if (HOOK_COMPLETION(hook, completion_item))
                    free (HOOK_COMPLETION(hook, completion_item));
                if (HOOK_COMPLETION(hook, description))
                    free (HOOK_COMPLETION(hook, description));
                break;
            case HOOK_TYPE_MODIFIER:
                if (HOOK_MODIFIER(hook, modifier))
                    free (HOOK_MODIFIER(hook, modifier));
                break;
            case HOOK_TYPE_INFO:
                if (HOOK_INFO(hook, info_name))
                    free (HOOK_INFO(hook, info_name));
                if (HOOK_INFO(hook, description))
                    free (HOOK_INFO(hook, description));
                if (HOOK_INFO(hook, args_description))
                    free (HOOK_INFO(hook, args_description));
                break;
            case HOOK_TYPE_INFO_HASHTABLE:
                if (HOOK_INFO_HASHTABLE(hook, info_name))
                    free (HOOK_INFO_HASHTABLE(hook, info_name));
                if (HOOK_INFO_HASHTABLE(hook, description))
                    free (HOOK_INFO_HASHTABLE(hook, description));
                if (HOOK_INFO_HASHTABLE(hook, args_description))
                    free (HOOK_INFO_HASHTABLE(hook, args_description));
                if (HOOK_INFO_HASHTABLE(hook, output_description))
                    free (HOOK_INFO_HASHTABLE(hook, output_description));
                break;
            case HOOK_TYPE_INFOLIST:
                if (HOOK_INFOLIST(hook, infolist_name))
                    free (HOOK_INFOLIST(hook, infolist_name));
                if (HOOK_INFOLIST(hook, description))
                    free (HOOK_INFOLIST(hook, description));
                if (HOOK_INFOLIST(hook, pointer_description))
                    free (HOOK_INFOLIST(hook, pointer_description));
                if (HOOK_INFOLIST(hook, args_description))
                    free (HOOK_INFOLIST(hook, args_description));
                break;
            case HOOK_TYPE_HDATA:
                if (HOOK_HDATA(hook, hdata_name))
                    free (HOOK_HDATA(hook, hdata_name));
                if (HOOK_HDATA(hook, description))
                    free (HOOK_HDATA(hook, description));
                break;
            case HOOK_TYPE_FOCUS:
                if (HOOK_FOCUS(hook, area))
                    free (HOOK_FOCUS(hook, area));
                break;
            case HOOK_NUM_TYPES:
                /*
                 * this constant is used to count types only,
                 * it is never used as type
                 */
                break;
        }
        free (hook->hook_data);
        hook->hook_data = NULL;
    }

    /* remove hook from list (if there's no hook exec pending) */
    if (hook_exec_recursion == 0)
    {
        hook_remove_from_list (hook);
    }
    else
    {
        /* there is one or more hook exec, then delete later */
        hook->deleted = 1;
        real_delete_pending = 1;
    }
}