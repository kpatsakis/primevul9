hook_focus_get_data (struct t_hashtable *hashtable_focus1,
                     struct t_hashtable *hashtable_focus2)
{
    struct t_hook *ptr_hook, *next_hook;
    struct t_hashtable *hashtable1, *hashtable2, *hashtable_ret;
    const char *focus1_chat, *focus1_bar_item_name, *keys;
    char **list_keys, *new_key;
    int num_keys, i, length, focus1_is_chat;

    if (!hashtable_focus1)
        return NULL;

    focus1_chat = hashtable_get (hashtable_focus1, "_chat");
    focus1_is_chat = (focus1_chat && (strcmp (focus1_chat, "1") == 0));
    focus1_bar_item_name = hashtable_get (hashtable_focus1, "_bar_item_name");

    hashtable1 = hashtable_dup (hashtable_focus1);
    if (!hashtable1)
        return NULL;
    hashtable2 = (hashtable_focus2) ? hashtable_dup (hashtable_focus2) : NULL;

    hook_exec_start ();

    ptr_hook = weechat_hooks[HOOK_TYPE_FOCUS];
    while (ptr_hook)
    {
        next_hook = ptr_hook->next_hook;

        if (!ptr_hook->deleted
            && !ptr_hook->running
            && ((focus1_is_chat
                 && (strcmp (HOOK_FOCUS(ptr_hook, area), "chat") == 0))
                || (focus1_bar_item_name && focus1_bar_item_name[0]
                    && (strcmp (HOOK_FOCUS(ptr_hook, area), focus1_bar_item_name) == 0))))
        {
            /* run callback for focus #1 */
            ptr_hook->running = 1;
            hashtable_ret = (HOOK_FOCUS(ptr_hook, callback))
                (ptr_hook->callback_data, hashtable1);
            ptr_hook->running = 0;
            if (hashtable_ret)
            {
                if (hashtable_ret != hashtable1)
                {
                    /*
                     * add keys of hashtable_ret into hashtable1
                     * and destroy it
                     */
                    hashtable_map (hashtable_ret,
                                   &hook_focus_hashtable_map_cb,
                                   hashtable1);
                    hashtable_free (hashtable_ret);
                }
            }

            /* run callback for focus #2 */
            if (hashtable2)
            {
                ptr_hook->running = 1;
                hashtable_ret = (HOOK_FOCUS(ptr_hook, callback))
                    (ptr_hook->callback_data, hashtable2);
                ptr_hook->running = 0;
                if (hashtable_ret)
                {
                    if (hashtable_ret != hashtable2)
                    {
                        /*
                         * add keys of hashtable_ret into hashtable2
                         * and destroy it
                         */
                        hashtable_map (hashtable_ret,
                                       &hook_focus_hashtable_map_cb,
                                       hashtable2);
                        hashtable_free (hashtable_ret);
                    }
                }
            }
        }

        ptr_hook = next_hook;
    }

    if (hashtable2)
    {
        hashtable_map (hashtable2, &hook_focus_hashtable_map2_cb, hashtable1);
        hashtable_free (hashtable2);
    }
    else
    {
        keys = hashtable_get_string (hashtable1, "keys");
        if (keys)
        {
            list_keys = string_split (keys, ",", 0, 0, &num_keys);
            if (list_keys)
            {
                for (i = 0; i < num_keys; i++)
                {
                    length = strlen (list_keys[i]) + 1 + 1;
                    new_key = malloc (length);
                    if (new_key)
                    {
                        snprintf (new_key, length, "%s2", list_keys[i]);
                        hashtable_set (hashtable1, new_key,
                                       hashtable_get (hashtable1,
                                                      list_keys[i]));
                        free (new_key);
                    }
                }
                string_free_split (list_keys);
            }
        }
    }

    hook_exec_end ();

    return hashtable1;
}