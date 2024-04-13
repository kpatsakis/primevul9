RxFilterInfoList *qmp_query_rx_filter(bool has_name, const char *name,
                                      Error **errp)
{
    NetClientState *nc;
    RxFilterInfoList *filter_list = NULL, **tail = &filter_list;

    QTAILQ_FOREACH(nc, &net_clients, next) {
        RxFilterInfo *info;

        if (has_name && strcmp(nc->name, name) != 0) {
            continue;
        }

        /* only query rx-filter information of NIC */
        if (nc->info->type != NET_CLIENT_DRIVER_NIC) {
            if (has_name) {
                error_setg(errp, "net client(%s) isn't a NIC", name);
                assert(!filter_list);
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
            QAPI_LIST_APPEND(tail, info);
        } else if (has_name) {
            error_setg(errp, "net client(%s) doesn't support"
                       " rx-filter querying", name);
            assert(!filter_list);
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