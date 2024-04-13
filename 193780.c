crm_ipc_prepare(uint32_t request, xmlNode * message, struct iovec ** result)
{
    static int biggest = 0;

    struct iovec *iov;
    unsigned int total = 0;
    char *compressed = NULL;
    char *buffer = dump_xml_unformatted(message);
    struct crm_ipc_response_header *header = calloc(1, sizeof(struct crm_ipc_response_header));

    CRM_ASSERT(result != NULL);

    iov = calloc(2, sizeof(struct iovec));

    crm_ipc_init();

    iov[0].iov_len = hdr_offset;
    iov[0].iov_base = header;

    header->size_uncompressed = 1 + strlen(buffer);
    total = hdr_offset + header->size_uncompressed;

    if (total < ipc_buffer_max) {
        iov[1].iov_base = buffer;
        iov[1].iov_len = header->size_uncompressed;

    } else {
        unsigned int new_size = 0;

        if (total > biggest) {
            biggest = 2 * QB_MAX(total, biggest);
            crm_notice("Message exceeds the configured ipc limit (%d bytes), "
                       "consider configuring PCMK_ipc_buffer to %d or higher "
                       "to avoid compression overheads", ipc_buffer_max, biggest);
        }

        if (crm_compress_string
            (buffer, header->size_uncompressed, ipc_buffer_max, &compressed, &new_size)) {

            header->flags |= crm_ipc_compressed;
            header->size_compressed = new_size;

            iov[1].iov_len = header->size_compressed;
            iov[1].iov_base = compressed;

            free(buffer);

        } else {
            ssize_t rc = -EMSGSIZE;

            crm_log_xml_trace(message, "EMSGSIZE");

            crm_err
                ("Could not compress the message into less than the configured ipc limit (%d bytes)."
                 "Set PCMK_ipc_buffer to a higher value (%d bytes suggested)", ipc_buffer_max,
                 biggest);

            free(compressed);
            free(buffer);
            free(header);
            free(iov);

            return rc;
        }
    }

    header->qb.size = iov[0].iov_len + iov[1].iov_len;
    header->qb.id = request;    /* Replying to a specific request */

    *result = iov;
    return header->qb.size;
}