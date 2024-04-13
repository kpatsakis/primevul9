hook_print_log ()
{
    int type, i, j;
    struct t_hook *ptr_hook;
    struct tm *local_time;
    char text_time[1024];

    for (type = 0; type < HOOK_NUM_TYPES; type++)
    {
        for (ptr_hook = weechat_hooks[type]; ptr_hook;
             ptr_hook = ptr_hook->next_hook)
        {
            log_printf ("");
            log_printf ("[hook (addr:0x%lx)]", ptr_hook);
            log_printf ("  plugin. . . . . . . . . : 0x%lx ('%s')",
                        ptr_hook->plugin, plugin_get_name (ptr_hook->plugin));
            log_printf ("  subplugin . . . . . . . : '%s'",  ptr_hook->subplugin);
            log_printf ("  type. . . . . . . . . . : %d (%s)",
                        ptr_hook->type, hook_type_string[ptr_hook->type]);
            log_printf ("  deleted . . . . . . . . : %d",    ptr_hook->deleted);
            log_printf ("  running . . . . . . . . : %d",    ptr_hook->running);
            log_printf ("  priority. . . . . . . . : %d",    ptr_hook->priority);
            log_printf ("  callback_data . . . . . : 0x%lx", ptr_hook->callback_data);
            switch (ptr_hook->type)
            {
                case HOOK_TYPE_COMMAND:
                    if (!ptr_hook->deleted)
                    {
                        log_printf ("  command data:");
                        log_printf ("    callback. . . . . . . : 0x%lx", HOOK_COMMAND(ptr_hook, callback));
                        log_printf ("    command . . . . . . . : '%s'",  HOOK_COMMAND(ptr_hook, command));
                        log_printf ("    description . . . . . : '%s'",  HOOK_COMMAND(ptr_hook, description));
                        log_printf ("    args. . . . . . . . . : '%s'",  HOOK_COMMAND(ptr_hook, args));
                        log_printf ("    args_description. . . : '%s'",  HOOK_COMMAND(ptr_hook, args_description));
                        log_printf ("    completion. . . . . . : '%s'",  HOOK_COMMAND(ptr_hook, completion));
                        log_printf ("    cplt_num_templates. . : %d",    HOOK_COMMAND(ptr_hook, cplt_num_templates));
                        for (i = 0; i < HOOK_COMMAND(ptr_hook, cplt_num_templates); i++)
                        {
                            log_printf ("    cplt_templates[%04d] . . . : '%s'",
                                        i, HOOK_COMMAND(ptr_hook, cplt_templates)[i]);
                            log_printf ("    cplt_templates_static[%04d]: '%s'",
                                        i, HOOK_COMMAND(ptr_hook, cplt_templates_static)[i]);
                            log_printf ("      num_args. . . . . . : %d",
                                        HOOK_COMMAND(ptr_hook, cplt_template_num_args)[i]);
                            for (j = 0; j < HOOK_COMMAND(ptr_hook, cplt_template_num_args)[i]; j++)
                            {
                                log_printf ("      args[%04d]. . . . . : '%s'",
                                            j, HOOK_COMMAND(ptr_hook, cplt_template_args)[i][j]);
                            }
                        }
                        log_printf ("    num_args_concat . . . : %d", HOOK_COMMAND(ptr_hook, cplt_template_num_args_concat));
                        for (i = 0; i < HOOK_COMMAND(ptr_hook, cplt_template_num_args_concat); i++)
                        {
                            log_printf ("    args_concat[%04d] . . : '%s'",
                                        i, HOOK_COMMAND(ptr_hook, cplt_template_args_concat)[i]);
                        }
                    }
                    break;
                case HOOK_TYPE_COMMAND_RUN:
                    if (!ptr_hook->deleted)
                    {
                        log_printf ("  command_run data:");
                        log_printf ("    callback. . . . . . . : 0x%lx", HOOK_COMMAND_RUN(ptr_hook, callback));
                        log_printf ("    command . . . . . . . : '%s'",  HOOK_COMMAND_RUN(ptr_hook, command));
                    }
                    break;
                case HOOK_TYPE_TIMER:
                    if (!ptr_hook->deleted)
                    {
                        log_printf ("  timer data:");
                        log_printf ("    callback. . . . . . . : 0x%lx", HOOK_TIMER(ptr_hook, callback));
                        log_printf ("    interval. . . . . . . : %ld",   HOOK_TIMER(ptr_hook, interval));
                        log_printf ("    align_second. . . . . : %d",    HOOK_TIMER(ptr_hook, align_second));
                        log_printf ("    remaining_calls . . . : %d",    HOOK_TIMER(ptr_hook, remaining_calls));
                        text_time[0] = '\0';
                        local_time = localtime (&HOOK_TIMER(ptr_hook, last_exec).tv_sec);
                        if (local_time)
                        {
                            strftime (text_time, sizeof (text_time),
                                      "%d/%m/%Y %H:%M:%S", local_time);
                        }
                        log_printf ("    last_exec.tv_sec. . . : %ld (%s)",
                                    HOOK_TIMER(ptr_hook, last_exec.tv_sec),
                                    text_time);
                        log_printf ("    last_exec.tv_usec . . : %ld",   HOOK_TIMER(ptr_hook, last_exec.tv_usec));
                        text_time[0] = '\0';
                        local_time = localtime (&HOOK_TIMER(ptr_hook, next_exec).tv_sec);
                        if (local_time)
                        {
                            strftime (text_time, sizeof (text_time),
                                      "%d/%m/%Y %H:%M:%S", local_time);
                        }
                        log_printf ("    next_exec.tv_sec. . . : %ld (%s)",
                                    HOOK_TIMER(ptr_hook, next_exec.tv_sec),
                                    text_time);
                        log_printf ("    next_exec.tv_usec . . : %ld",   HOOK_TIMER(ptr_hook, next_exec.tv_usec));
                    }
                    break;
                case HOOK_TYPE_FD:
                    if (!ptr_hook->deleted)
                    {
                        log_printf ("  fd data:");
                        log_printf ("    callback. . . . . . . : 0x%lx", HOOK_FD(ptr_hook, callback));
                        log_printf ("    fd. . . . . . . . . . : %d",    HOOK_FD(ptr_hook, fd));
                        log_printf ("    flags . . . . . . . . : %d",    HOOK_FD(ptr_hook, flags));
                        log_printf ("    error . . . . . . . . : %d",    HOOK_FD(ptr_hook, error));
                    }
                    break;
                case HOOK_TYPE_PROCESS:
                    if (!ptr_hook->deleted)
                    {
                        log_printf ("  process data:");
                        log_printf ("    callback. . . . . . . : 0x%lx", HOOK_PROCESS(ptr_hook, callback));
                        log_printf ("    command . . . . . . . : '%s'",  HOOK_PROCESS(ptr_hook, command));
                        log_printf ("    options . . . . . . . : 0x%lx (hashtable: '%s')",
                                    HOOK_PROCESS(ptr_hook, options),
                                    hashtable_get_string (HOOK_PROCESS(ptr_hook, options),
                                                          "keys_values"));
                        log_printf ("    timeout . . . . . . . : %d",    HOOK_PROCESS(ptr_hook, timeout));
                        log_printf ("    child_read[stdout]. . : %d",    HOOK_PROCESS(ptr_hook, child_read[HOOK_PROCESS_STDOUT]));
                        log_printf ("    child_write[stdout] . : %d",    HOOK_PROCESS(ptr_hook, child_write[HOOK_PROCESS_STDOUT]));
                        log_printf ("    child_read[stderr]. . : %d",    HOOK_PROCESS(ptr_hook, child_read[HOOK_PROCESS_STDERR]));
                        log_printf ("    child_write[stderr] . : %d",    HOOK_PROCESS(ptr_hook, child_write[HOOK_PROCESS_STDERR]));
                        log_printf ("    child_pid . . . . . . : %d",    HOOK_PROCESS(ptr_hook, child_pid));
                        log_printf ("    hook_fd[stdout] . . . : 0x%lx", HOOK_PROCESS(ptr_hook, hook_fd[HOOK_PROCESS_STDOUT]));
                        log_printf ("    hook_fd[stderr] . . . : 0x%lx", HOOK_PROCESS(ptr_hook, hook_fd[HOOK_PROCESS_STDERR]));
                        log_printf ("    hook_timer. . . . . . : 0x%lx", HOOK_PROCESS(ptr_hook, hook_timer));
                    }
                    break;
                case HOOK_TYPE_CONNECT:
                    if (!ptr_hook->deleted)
                    {
                        log_printf ("  connect data:");
                        log_printf ("    callback. . . . . . . : 0x%lx", HOOK_CONNECT(ptr_hook, callback));
                        log_printf ("    address . . . . . . . : '%s'",  HOOK_CONNECT(ptr_hook, address));
                        log_printf ("    port. . . . . . . . . : %d",    HOOK_CONNECT(ptr_hook, port));
                        log_printf ("    sock. . . . . . . . . : %d",    HOOK_CONNECT(ptr_hook, sock));
                        log_printf ("    ipv6. . . . . . . . . : %d",    HOOK_CONNECT(ptr_hook, ipv6));
                        log_printf ("    retry . . . . . . . . : %d",    HOOK_CONNECT(ptr_hook, retry));
#ifdef HAVE_GNUTLS
                        log_printf ("    gnutls_sess . . . . . : 0x%lx", HOOK_CONNECT(ptr_hook, gnutls_sess));
                        log_printf ("    gnutls_cb . . . . . . : 0x%lx", HOOK_CONNECT(ptr_hook, gnutls_cb));
                        log_printf ("    gnutls_dhkey_size . . : %d",    HOOK_CONNECT(ptr_hook, gnutls_dhkey_size));
                        log_printf ("    gnutls_priorities . . : '%s'",  HOOK_CONNECT(ptr_hook, gnutls_priorities));
#endif
                        log_printf ("    local_hostname. . . . : '%s'",  HOOK_CONNECT(ptr_hook, local_hostname));
                        log_printf ("    child_read. . . . . . : %d",    HOOK_CONNECT(ptr_hook, child_read));
                        log_printf ("    child_write . . . . . : %d",    HOOK_CONNECT(ptr_hook, child_write));
                        log_printf ("    child_recv. . . . . . : %d",    HOOK_CONNECT(ptr_hook, child_recv));
                        log_printf ("    child_send. . . . . . : %d",    HOOK_CONNECT(ptr_hook, child_send));
                        log_printf ("    child_pid . . . . . . : %d",    HOOK_CONNECT(ptr_hook, child_pid));
                        log_printf ("    hook_child_timer. . . : 0x%lx", HOOK_CONNECT(ptr_hook, hook_child_timer));
                        log_printf ("    hook_fd . . . . . . . : 0x%lx", HOOK_CONNECT(ptr_hook, hook_fd));
                        log_printf ("    handshake_hook_fd . . : 0x%lx", HOOK_CONNECT(ptr_hook, handshake_hook_fd));
                        log_printf ("    handshake_hook_timer. : 0x%lx", HOOK_CONNECT(ptr_hook, handshake_hook_timer));
                        log_printf ("    handshake_fd_flags. . : %d",    HOOK_CONNECT(ptr_hook, handshake_fd_flags));
                        log_printf ("    handshake_ip_address. : '%s'",  HOOK_CONNECT(ptr_hook, handshake_ip_address));
#ifdef HOOK_CONNECT_MAX_SOCKETS
                        for (i = 0; i < HOOK_CONNECT_MAX_SOCKETS; i++)
                        {
                            log_printf ("    sock_v4[%d]. . . . . . : '%d'", HOOK_CONNECT(ptr_hook, sock_v4[i]));
                            log_printf ("    sock_v6[%d]. . . . . . : '%d'", HOOK_CONNECT(ptr_hook, sock_v6[i]));
                        }
#endif
                    }
                    break;
                case HOOK_TYPE_PRINT:
                    if (!ptr_hook->deleted)
                    {
                        log_printf ("  print data:");
                        log_printf ("    callback. . . . . . . : 0x%lx", HOOK_PRINT(ptr_hook, callback));
                        log_printf ("    buffer. . . . . . . . : 0x%lx", HOOK_PRINT(ptr_hook, buffer));
                        log_printf ("    tags_count. . . . . . : %d",    HOOK_PRINT(ptr_hook, tags_count));
                        log_printf ("    tags_array. . . . . . : 0x%lx", HOOK_PRINT(ptr_hook, tags_array));
                        log_printf ("    message . . . . . . . : '%s'",  HOOK_PRINT(ptr_hook, message));
                        log_printf ("    strip_colors. . . . . : %d",    HOOK_PRINT(ptr_hook, strip_colors));
                    }
                    break;
                case HOOK_TYPE_SIGNAL:
                    if (!ptr_hook->deleted)
                    {
                        log_printf ("  signal data:");
                        log_printf ("    callback. . . . . . . : 0x%lx", HOOK_SIGNAL(ptr_hook, callback));
                        log_printf ("    signal. . . . . . . . : '%s'",  HOOK_SIGNAL(ptr_hook, signal));
                    }
                    break;
                case HOOK_TYPE_HSIGNAL:
                    if (!ptr_hook->deleted)
                    {
                        log_printf ("  signal data:");
                        log_printf ("    callback. . . . . . . : 0x%lx", HOOK_HSIGNAL(ptr_hook, callback));
                        log_printf ("    signal. . . . . . . . : '%s'",  HOOK_HSIGNAL(ptr_hook, signal));
                    }
                    break;
                case HOOK_TYPE_CONFIG:
                    if (!ptr_hook->deleted)
                    {
                        log_printf ("  config data:");
                        log_printf ("    callback. . . . . . . : 0x%lx", HOOK_CONFIG(ptr_hook, callback));
                        log_printf ("    option. . . . . . . . : '%s'",  HOOK_CONFIG(ptr_hook, option));
                    }
                    break;
                case HOOK_TYPE_COMPLETION:
                    if (!ptr_hook->deleted)
                    {
                        log_printf ("  completion data:");
                        log_printf ("    callback. . . . . . . : 0x%lx", HOOK_COMPLETION(ptr_hook, callback));
                        log_printf ("    completion_item . . . : '%s'",  HOOK_COMPLETION(ptr_hook, completion_item));
                        log_printf ("    description . . . . . : '%s'",  HOOK_COMPLETION(ptr_hook, description));
                    }
                    break;
                case HOOK_TYPE_MODIFIER:
                    if (!ptr_hook->deleted)
                    {
                        log_printf ("  modifier data:");
                        log_printf ("    callback. . . . . . . : 0x%lx", HOOK_MODIFIER(ptr_hook, callback));
                        log_printf ("    modifier. . . . . . . : '%s'",  HOOK_MODIFIER(ptr_hook, modifier));
                    }
                    break;
                case HOOK_TYPE_INFO:
                    if (!ptr_hook->deleted)
                    {
                        log_printf ("  info data:");
                        log_printf ("    callback. . . . . . . : 0x%lx", HOOK_INFO(ptr_hook, callback));
                        log_printf ("    info_name . . . . . . : '%s'",  HOOK_INFO(ptr_hook, info_name));
                        log_printf ("    description . . . . . : '%s'",  HOOK_INFO(ptr_hook, description));
                        log_printf ("    args_description. . . : '%s'",  HOOK_INFO(ptr_hook, args_description));
                    }
                    break;
                case HOOK_TYPE_INFO_HASHTABLE:
                    if (!ptr_hook->deleted)
                    {
                        log_printf ("  info_hashtable data:");
                        log_printf ("    callback. . . . . . . : 0x%lx", HOOK_INFO_HASHTABLE(ptr_hook, callback));
                        log_printf ("    info_name . . . . . . : '%s'",  HOOK_INFO_HASHTABLE(ptr_hook, info_name));
                        log_printf ("    description . . . . . : '%s'",  HOOK_INFO_HASHTABLE(ptr_hook, description));
                        log_printf ("    args_description. . . : '%s'",  HOOK_INFO_HASHTABLE(ptr_hook, args_description));
                        log_printf ("    output_description. . : '%s'",  HOOK_INFO_HASHTABLE(ptr_hook, output_description));
                    }
                    break;
                case HOOK_TYPE_INFOLIST:
                    if (!ptr_hook->deleted)
                    {
                        log_printf ("  infolist data:");
                        log_printf ("    callback. . . . . . . : 0x%lx", HOOK_INFOLIST(ptr_hook, callback));
                        log_printf ("    infolist_name . . . . : '%s'",  HOOK_INFOLIST(ptr_hook, infolist_name));
                        log_printf ("    description . . . . . : '%s'",  HOOK_INFOLIST(ptr_hook, description));
                        log_printf ("    pointer_description . : '%s'",  HOOK_INFOLIST(ptr_hook, pointer_description));
                        log_printf ("    args_description. . . : '%s'",  HOOK_INFOLIST(ptr_hook, args_description));
                    }
                    break;
                case HOOK_TYPE_HDATA:
                    if (!ptr_hook->deleted)
                    {
                        log_printf ("  hdata data:");
                        log_printf ("    callback. . . . . . . : 0x%lx", HOOK_HDATA(ptr_hook, callback));
                        log_printf ("    hdata_name. . . . . . : '%s'",  HOOK_HDATA(ptr_hook, hdata_name));
                        log_printf ("    description . . . . . : '%s'",  HOOK_HDATA(ptr_hook, description));
                    }
                    break;
                case HOOK_TYPE_FOCUS:
                    if (!ptr_hook->deleted)
                    {
                        log_printf ("  focus data:");
                        log_printf ("    callback. . . . . . . : 0x%lx", HOOK_FOCUS(ptr_hook, callback));
                        log_printf ("    area. . . . . . . . . : '%s'",  HOOK_FOCUS(ptr_hook, area));
                    }
                    break;
                case HOOK_NUM_TYPES:
                    /*
                     * this constant is used to count types only,
                     * it is never used as type
                     */
                    break;
            }
            log_printf ("  prev_hook . . . . . . . : 0x%lx", ptr_hook->prev_hook);
            log_printf ("  next_hook . . . . . . . : 0x%lx", ptr_hook->next_hook);
        }
    }
}