GuestUserList *qmp_guest_get_users(Error **err)
{
    GHashTable *cache = NULL;
    GuestUserList *head = NULL, *cur_item = NULL;
    struct utmpx *user_info = NULL;
    gpointer value = NULL;
    GuestUser *user = NULL;
    GuestUserList *item = NULL;
    double login_time = 0;

    cache = g_hash_table_new(g_str_hash, g_str_equal);
    setutxent();

    for (;;) {
        user_info = getutxent();
        if (user_info == NULL) {
            break;
        } else if (user_info->ut_type != USER_PROCESS) {
            continue;
        } else if (g_hash_table_contains(cache, user_info->ut_user)) {
            value = g_hash_table_lookup(cache, user_info->ut_user);
            user = (GuestUser *)value;
            login_time = ga_get_login_time(user_info);
            /* We're ensuring the earliest login time to be sent */
            if (login_time < user->login_time) {
                user->login_time = login_time;
            }
            continue;
        }

        item = g_new0(GuestUserList, 1);
        item->value = g_new0(GuestUser, 1);
        item->value->user = g_strdup(user_info->ut_user);
        item->value->login_time = ga_get_login_time(user_info);

        g_hash_table_insert(cache, item->value->user, item->value);

        if (!cur_item) {
            head = cur_item = item;
        } else {
            cur_item->next = item;
            cur_item = item;
        }
    }
    endutxent();
    g_hash_table_destroy(cache);
    return head;
}