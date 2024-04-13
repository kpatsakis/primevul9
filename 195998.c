void print_net_client(Monitor *mon, NetClientState *nc)
{
    NetFilterState *nf;

    monitor_printf(mon, "%s: index=%d,type=%s,%s\n", nc->name,
                   nc->queue_index,
                   NetClientDriver_str(nc->info->type),
                   nc->info_str);
    if (!QTAILQ_EMPTY(&nc->filters)) {
        monitor_printf(mon, "filters:\n");
    }
    QTAILQ_FOREACH(nf, &nc->filters, next) {
        monitor_printf(mon, "  - %s: type=%s",
                       object_get_canonical_path_component(OBJECT(nf)),
                       object_get_typename(OBJECT(nf)));
        netfilter_print_info(mon, nf);
    }
}