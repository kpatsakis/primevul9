hook_timer_check_system_clock ()
{
    time_t now;
    long diff_time;
    struct t_hook *ptr_hook;

    now = time (NULL);

    /*
     * check if difference with previous time is more than 10 seconds:
     * if it is, then consider it's clock skew and reinitialize all timers
     */
    diff_time = now - hook_last_system_time;
    if ((diff_time <= -10) || (diff_time >= 10))
    {
        if (weechat_debug_core >= 1)
        {
            gui_chat_printf (NULL,
                             _("System clock skew detected (%+ld seconds), "
                               "reinitializing all timers"),
                             diff_time);
        }

        /* reinitialize all timers */
        for (ptr_hook = weechat_hooks[HOOK_TYPE_TIMER]; ptr_hook;
             ptr_hook = ptr_hook->next_hook)
        {
            if (!ptr_hook->deleted)
                hook_timer_init (ptr_hook);
        }
    }

    hook_last_system_time = now;
}