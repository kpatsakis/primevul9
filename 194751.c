hook_focus_hashtable_map_cb (void *data, struct t_hashtable *hashtable,
                             const void *key, const void *value)
{
    struct t_hashtable *hashtable1;

    /* make C compiler happy */
    (void) hashtable;

    hashtable1 = (struct t_hashtable *)data;

    if (hashtable1 && key && value)
        hashtable_set (hashtable1, (const char *)key, (const char *)value);
}