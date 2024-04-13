GuestUserList *qmp_guest_get_users(Error **err)
{
#if (_WIN32_WINNT >= 0x0600)
#define QGA_NANOSECONDS 10000000

    GHashTable *cache = NULL;
    GuestUserList *head = NULL, *cur_item = NULL;

    DWORD buffer_size = 0, count = 0, i = 0;
    GA_WTSINFOA *info = NULL;
    WTS_SESSION_INFOA *entries = NULL;
    GuestUserList *item = NULL;
    GuestUser *user = NULL;
    gpointer value = NULL;
    INT64 login = 0;
    double login_time = 0;

    cache = g_hash_table_new(g_str_hash, g_str_equal);

    if (WTSEnumerateSessionsA(NULL, 0, 1, &entries, &count)) {
        for (i = 0; i < count; ++i) {
            buffer_size = 0;
            info = NULL;
            if (WTSQuerySessionInformationA(
                NULL,
                entries[i].SessionId,
                WTSSessionInfo,
                (LPSTR *)&info,
                &buffer_size
            )) {

                if (strlen(info->UserName) == 0) {
                    WTSFreeMemory(info);
                    continue;
                }

                login = info->LogonTime.QuadPart;
                login -= W32_FT_OFFSET;
                login_time = ((double)login) / QGA_NANOSECONDS;

                if (g_hash_table_contains(cache, info->UserName)) {
                    value = g_hash_table_lookup(cache, info->UserName);
                    user = (GuestUser *)value;
                    if (user->login_time > login_time) {
                        user->login_time = login_time;
                    }
                } else {
                    item = g_new0(GuestUserList, 1);
                    item->value = g_new0(GuestUser, 1);

                    item->value->user = g_strdup(info->UserName);
                    item->value->domain = g_strdup(info->Domain);
                    item->value->has_domain = true;

                    item->value->login_time = login_time;

                    g_hash_table_add(cache, item->value->user);

                    if (!cur_item) {
                        head = cur_item = item;
                    } else {
                        cur_item->next = item;
                        cur_item = item;
                    }
                }
            }
            WTSFreeMemory(info);
        }
        WTSFreeMemory(entries);
    }
    g_hash_table_destroy(cache);
    return head;
#else
    error_setg(err, QERR_UNSUPPORTED);
    return NULL;
#endif
}