crm_ipcs_recv(crm_client_t * c, void *data, size_t size, uint32_t * id, uint32_t * flags)
{
    xmlNode *xml = NULL;
    char *uncompressed = NULL;
    char *text = ((char *)data) + sizeof(struct crm_ipc_response_header);
    struct crm_ipc_response_header *header = data;

    if (id) {
        *id = ((struct qb_ipc_request_header *)data)->id;
    }
    if (flags) {
        *flags = header->flags;
    }

    if (header->flags & crm_ipc_compressed) {
        int rc = 0;
        unsigned int size_u = 1 + header->size_uncompressed;
        uncompressed = calloc(1, hdr_offset + size_u);

        crm_trace("Decompressing message data %d bytes into %d bytes",
                  header->size_compressed, size_u);

        rc = BZ2_bzBuffToBuffDecompress(uncompressed, &size_u, text, header->size_compressed, 1, 0);
        text = uncompressed;

        if (rc != BZ_OK) {
            crm_err("Decompression failed: %s (%d)", bz2_strerror(rc), rc);
            free(uncompressed);
            return NULL;
        }
    }

    CRM_ASSERT(text[header->size_uncompressed - 1] == 0);

    crm_trace("Received %.200s", text);
    xml = string2xml(text);

    free(uncompressed);
    return xml;
}