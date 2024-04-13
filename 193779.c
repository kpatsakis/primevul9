crm_ipc_new(const char *name, size_t max_size)
{
    crm_ipc_t *client = NULL;

    client = calloc(1, sizeof(crm_ipc_t));

    client->name = strdup(name);
    client->buf_size = pick_ipc_buffer(max_size);
    client->buffer = malloc(client->buf_size);

    client->pfd.fd = -1;
    client->pfd.events = POLLIN;
    client->pfd.revents = 0;

    return client;
}