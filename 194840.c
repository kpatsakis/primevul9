hook_timer_time_to_next (struct timeval *tv_timeout)
{
    struct t_hook *ptr_hook;
    int found;
    struct timeval tv_now;
    long diff_usec;

    hook_timer_check_system_clock ();

    found = 0;
    tv_timeout->tv_sec = 0;
    tv_timeout->tv_usec = 0;

    for (ptr_hook = weechat_hooks[HOOK_TYPE_TIMER]; ptr_hook;
         ptr_hook = ptr_hook->next_hook)
    {
        if (!ptr_hook->deleted
            && (!found
                || (util_timeval_cmp (&HOOK_TIMER(ptr_hook, next_exec), tv_timeout) < 0)))
        {
            found = 1;
            tv_timeout->tv_sec = HOOK_TIMER(ptr_hook, next_exec).tv_sec;
            tv_timeout->tv_usec = HOOK_TIMER(ptr_hook, next_exec).tv_usec;
        }
    }

    /* no timeout found, return 2 seconds by default */
    if (!found)
    {
        tv_timeout->tv_sec = 2;
        tv_timeout->tv_usec = 0;
        return;
    }

    gettimeofday (&tv_now, NULL);

    /* next timeout is past date! */
    if (util_timeval_cmp (tv_timeout, &tv_now) < 0)
    {
        tv_timeout->tv_sec = 0;
        tv_timeout->tv_usec = 0;
        return;
    }

    tv_timeout->tv_sec = tv_timeout->tv_sec - tv_now.tv_sec;
    diff_usec = tv_timeout->tv_usec - tv_now.tv_usec;
    if (diff_usec >= 0)
        tv_timeout->tv_usec = diff_usec;
    else
    {
        tv_timeout->tv_sec--;
        tv_timeout->tv_usec = 1000000 + diff_usec;
    }

    /*
     * to detect clock skew, we ensure there's a call to timers every
     * 2 seconds max
     */
    if (tv_timeout->tv_sec > 2)
    {
        tv_timeout->tv_sec = 2;
        tv_timeout->tv_usec = 0;
    }
}