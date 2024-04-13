int usbredirparser_do_read(struct usbredirparser *parser_pub)
{
    struct usbredirparser_priv *parser =
        (struct usbredirparser_priv *)parser_pub;
    int r, header_len, type_header_len, data_len;
    bool data_ownership_transferred;
    uint8_t *dest;

    header_len = usbredirparser_get_header_len(parser_pub);

    /* Skip forward to next packet (only used in error conditions) */
    while (parser->to_skip > 0) {
        uint8_t buf[65536];
        r = (parser->to_skip > sizeof(buf)) ? sizeof(buf) : parser->to_skip;
        r = parser->callb.read_func(parser->callb.priv, buf, r);
        if (r <= 0)
            return r;
        parser->to_skip -= r;
    }

    /* Consume data until read would block or returns an error */
    while (1) {
        if (parser->header_read < header_len) {
            r = header_len - parser->header_read;
            dest = (uint8_t *)&parser->header + parser->header_read;
        } else if (parser->type_header_read < parser->type_header_len) {
            r = parser->type_header_len - parser->type_header_read;
            dest = parser->type_header + parser->type_header_read;
        } else {
            r = parser->data_len - parser->data_read;
            dest = parser->data + parser->data_read;
        }

        if (r > 0) {
            r = parser->callb.read_func(parser->callb.priv, dest, r);
            if (r <= 0) {
                return r;
            }
        }

        if (parser->header_read < header_len) {
            parser->header_read += r;
            if (parser->header_read == header_len) {
                type_header_len =
                    usbredirparser_get_type_header_len(parser_pub,
                                                       parser->header.type, 0);
                if (type_header_len < 0) {
                    ERROR("error invalid usb-redir packet type: %u",
                          parser->header.type);
                    parser->to_skip = parser->header.length;
                    parser->header_read = 0;
                    return usbredirparser_read_parse_error;
                }
                /* This should never happen */
                if (type_header_len > sizeof(parser->type_header)) {
                    ERROR("error type specific header buffer too small, please report!!");
                    parser->to_skip = parser->header.length;
                    parser->header_read = 0;
                    return usbredirparser_read_parse_error;
                }
                if (parser->header.length > MAX_PACKET_SIZE) {
                    ERROR("packet length of %d larger than permitted %d bytes",
                          parser->header.length, MAX_PACKET_SIZE);
                    parser->to_skip = parser->header.length;
                    parser->header_read = 0;
                    return usbredirparser_read_parse_error;
                }
                if ((int)parser->header.length < type_header_len ||
                    ((int)parser->header.length > type_header_len &&
                     !usbredirparser_expect_extra_data(parser))) {
                    ERROR("error invalid packet type %u length: %u",
                          parser->header.type, parser->header.length);
                    parser->to_skip = parser->header.length;
                    parser->header_read = 0;
                    return usbredirparser_read_parse_error;
                }
                data_len = parser->header.length - type_header_len;
                if (data_len) {
                    parser->data = malloc(data_len);
                    if (!parser->data) {
                        ERROR("Out of memory allocating data buffer");
                        parser->to_skip = parser->header.length;
                        parser->header_read = 0;
                        return usbredirparser_read_parse_error;
                    }
                }
                parser->type_header_len = type_header_len;
                parser->data_len = data_len;
            }
        } else if (parser->type_header_read < parser->type_header_len) {
            parser->type_header_read += r;
        } else {
            parser->data_read += r;
            if (parser->data_read == parser->data_len) {
                r = usbredirparser_verify_type_header(parser_pub,
                         parser->header.type, parser->type_header,
                         parser->data, parser->data_len, 0);
                data_ownership_transferred = false;
                if (r) {
                    usbredirparser_call_type_func(parser_pub,
                                                  &data_ownership_transferred);
                }
                if (!data_ownership_transferred) {
                    free(parser->data);
                }
                parser->header_read = 0;
                parser->type_header_len  = 0;
                parser->type_header_read = 0;
                parser->data_len  = 0;
                parser->data_read = 0;
                parser->data = NULL;
                if (!r)
                    return usbredirparser_read_parse_error;
                /* header len may change if this was an hello packet */
                header_len = usbredirparser_get_header_len(parser_pub);
            }
        }
    }
}