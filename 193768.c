crm_ipc_buffer(crm_ipc_t * client)
{
    CRM_ASSERT(client != NULL);
    return client->buffer + sizeof(struct crm_ipc_response_header);
}