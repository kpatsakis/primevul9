hook_hdata_get (struct t_weechat_plugin *plugin, const char *hdata_name)
{
    struct t_hook *ptr_hook, *next_hook;
    struct t_hdata *value;

    /* make C compiler happy */
    (void) plugin;

    if (!hdata_name || !hdata_name[0])
        return NULL;

    if (weechat_hdata)
    {
        value = hashtable_get (weechat_hdata, hdata_name);
        if (value)
            return value;
    }

    hook_exec_start ();

    ptr_hook = weechat_hooks[HOOK_TYPE_HDATA];
    while (ptr_hook)
    {
        next_hook = ptr_hook->next_hook;

        if (!ptr_hook->deleted
            && !ptr_hook->running
            && (strcmp (HOOK_HDATA(ptr_hook, hdata_name), hdata_name) == 0))
        {
            ptr_hook->running = 1;
            value = (HOOK_HDATA(ptr_hook, callback))
                (ptr_hook->callback_data,
                 HOOK_HDATA(ptr_hook, hdata_name));
            ptr_hook->running = 0;

            hook_exec_end ();
            return value;
        }

        ptr_hook = next_hook;
    }

    hook_exec_end ();

    /* hdata not found */
    return NULL;
}