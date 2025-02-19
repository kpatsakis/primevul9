RxFilterInfoList *qmp_query_rx_filter(bool has_name, const char *name,
                                      Error **errp)
{
    NetClientState *nc;
    RxFilterInfoList *filter_list = NULL, *last_entry = NULL;

    QTAILQ_FOREACH(nc, &net_clients, next) {
        RxFilterInfoList *entry;
        RxFilterInfo *info;

        if (has_name && strcmp(nc->name, name) != 0) {
            continue;
        }

        /* only query rx-filter information of NIC */
        if (nc->info->type != NET_CLIENT_DRIVER_NIC) {
            if (has_name) {
                error_setg(errp, "net client(%s) isn't a NIC", name);
                return NULL;
            }
            continue;
        }

        /* only query information on queue 0 since the info is per nic,
         * not per queue
         */
        if (nc->queue_index != 0)
            continue;

        if (nc->info->query_rx_filter) {
            info = nc->info->query_rx_filter(nc);
            entry = g_malloc0(sizeof(*entry));
            entry->value = info;

            if (!filter_list) {
                filter_list = entry;
            } else {
                last_entry->next = entry;
            }
            last_entry = entry;
        } else if (has_name) {
            error_setg(errp, "net client(%s) doesn't support"
                       " rx-filter querying", name);
            return NULL;
        }

        if (has_name) {
            break;
        }
    }

    if (filter_list == NULL && has_name) {
        error_setg(errp, "invalid net client name: %s", name);
    }

    return filter_list;
}