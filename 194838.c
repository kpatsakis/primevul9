hook_timer_exec ()
{
    struct timeval tv_time;
    struct t_hook *ptr_hook, *next_hook;

    hook_timer_check_system_clock ();

    gettimeofday (&tv_time, NULL);

    hook_exec_start ();

    ptr_hook = weechat_hooks[HOOK_TYPE_TIMER];
    while (ptr_hook)
    {
        next_hook = ptr_hook->next_hook;

        if (!ptr_hook->deleted
            && !ptr_hook->running
            && (util_timeval_cmp (&HOOK_TIMER(ptr_hook, next_exec),
                                  &tv_time) <= 0))
        {
            ptr_hook->running = 1;
            (void) (HOOK_TIMER(ptr_hook, callback))
                (ptr_hook->callback_data,
                 (HOOK_TIMER(ptr_hook, remaining_calls) > 0) ?
                  HOOK_TIMER(ptr_hook, remaining_calls) - 1 : -1);
            ptr_hook->running = 0;
            if (!ptr_hook->deleted)
            {
                HOOK_TIMER(ptr_hook, last_exec).tv_sec = tv_time.tv_sec;
                HOOK_TIMER(ptr_hook, last_exec).tv_usec = tv_time.tv_usec;

                util_timeval_add (&HOOK_TIMER(ptr_hook, next_exec),
                                  HOOK_TIMER(ptr_hook, interval));

                if (HOOK_TIMER(ptr_hook, remaining_calls) > 0)
                {
                    HOOK_TIMER(ptr_hook, remaining_calls)--;
                    if (HOOK_TIMER(ptr_hook, remaining_calls) == 0)
                        unhook (ptr_hook);
                }
            }
        }

        ptr_hook = next_hook;
    }

    hook_exec_end ();
}