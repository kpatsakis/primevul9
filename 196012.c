void colo_notify_filters_event(int event, Error **errp)
{
    NetClientState *nc;
    NetFilterState *nf;
    NetFilterClass *nfc = NULL;
    Error *local_err = NULL;

    QTAILQ_FOREACH(nc, &net_clients, next) {
        QTAILQ_FOREACH(nf, &nc->filters, next) {
            nfc = NETFILTER_GET_CLASS(OBJECT(nf));
            nfc->handle_event(nf, event, &local_err);
            if (local_err) {
                error_propagate(errp, local_err);
                return;
            }
        }
    }
}