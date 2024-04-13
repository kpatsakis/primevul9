crm_ipc_init(void)
{
    if (hdr_offset == 0) {
        hdr_offset = sizeof(struct crm_ipc_response_header);
    }
    if (ipc_buffer_max == 0) {
        ipc_buffer_max = pick_ipc_buffer(0);
    }
}