static char *assign_name(NetClientState *nc1, const char *model)
{
    NetClientState *nc;
    int id = 0;

    QTAILQ_FOREACH(nc, &net_clients, next) {
        if (nc == nc1) {
            continue;
        }
        if (strcmp(nc->model, model) == 0) {
            id++;
        }
    }

    return g_strdup_printf("%s.%d", model, id);
}