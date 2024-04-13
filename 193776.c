crm_ipc_destroy(crm_ipc_t * client)
{
    if (client) {
        if (client->ipc && qb_ipcc_is_connected(client->ipc)) {
            crm_notice("Destroying an active IPC connection to %s", client->name);
            /* The next line is basically unsafe
             *
             * If this connection was attached to mainloop and mainloop is active,
             *   the 'disconnected' callback will end up back here and we'll end
             *   up free'ing the memory twice - something that can still happen
             *   even without this if we destroy a connection and it closes before
             *   we call exit
             */
            /* crm_ipc_close(client); */
        }
        crm_trace("Destroying IPC connection to %s: %p", client->name, client);
        free(client->buffer);
        free(client->name);
        free(client);
    }
}