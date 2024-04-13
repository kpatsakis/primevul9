crm_ipc_decompress(crm_ipc_t * client)
{
    struct crm_ipc_response_header *header = (struct crm_ipc_response_header *)client->buffer;

    if (header->flags & crm_ipc_compressed) {
        int rc = 0;
        unsigned int size_u = 1 + header->size_uncompressed;
        char *uncompressed = calloc(1, hdr_offset + size_u);

        crm_trace("Decompressing message data %d bytes into %d bytes",
                 header->size_compressed, size_u);

        rc = BZ2_bzBuffToBuffDecompress(uncompressed + hdr_offset, &size_u,
                                        client->buffer + hdr_offset, header->size_compressed, 1, 0);

        if (rc != BZ_OK) {
            crm_err("Decompression failed: %s (%d)", bz2_strerror(rc), rc);
            free(uncompressed);
            return -EILSEQ;
        }

        CRM_ASSERT((header->size_uncompressed + hdr_offset) >= ipc_buffer_max);
        CRM_ASSERT(size_u == header->size_uncompressed);

        memcpy(uncompressed, client->buffer, hdr_offset);       /* Preserve the header */
        header = (struct crm_ipc_response_header *)uncompressed;

        free(client->buffer);
        client->buf_size = hdr_offset + size_u;
        client->buffer = uncompressed;
    }

    CRM_ASSERT(client->buffer[hdr_offset + header->size_uncompressed - 1] == 0);
    return pcmk_ok;
}