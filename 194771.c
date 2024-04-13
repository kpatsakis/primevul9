hook_focus_hashtable_map2_cb (void *data, struct t_hashtable *hashtable,
                              const void *key, const void *value)
{
    struct t_hashtable *hashtable1;
    int length;
    char *key2;

    /* make C compiler happy */
    (void) hashtable;

    hashtable1 = (struct t_hashtable *)data;

    length = strlen ((const char *)key) + 1 + 1;
    key2 = malloc (length);
    if (key2)
    {
        snprintf (key2, length, "%s2", (const char *)key);
        if (hashtable1 && key && value)
            hashtable_set (hashtable1, key2, (const char *)value);
        free (key2);
    }
}