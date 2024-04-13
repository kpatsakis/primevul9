int net_fill_rstate(SocketReadState *rs, const uint8_t *buf, int size)
{
    unsigned int l;

    while (size > 0) {
        /* Reassemble a packet from the network.
         * 0 = getting length.
         * 1 = getting vnet header length.
         * 2 = getting data.
         */
        switch (rs->state) {
        case 0:
            l = 4 - rs->index;
            if (l > size) {
                l = size;
            }
            memcpy(rs->buf + rs->index, buf, l);
            buf += l;
            size -= l;
            rs->index += l;
            if (rs->index == 4) {
                /* got length */
                rs->packet_len = ntohl(*(uint32_t *)rs->buf);
                rs->index = 0;
                if (rs->vnet_hdr) {
                    rs->state = 1;
                } else {
                    rs->state = 2;
                    rs->vnet_hdr_len = 0;
                }
            }
            break;
        case 1:
            l = 4 - rs->index;
            if (l > size) {
                l = size;
            }
            memcpy(rs->buf + rs->index, buf, l);
            buf += l;
            size -= l;
            rs->index += l;
            if (rs->index == 4) {
                /* got vnet header length */
                rs->vnet_hdr_len = ntohl(*(uint32_t *)rs->buf);
                rs->index = 0;
                rs->state = 2;
            }
            break;
        case 2:
            l = rs->packet_len - rs->index;
            if (l > size) {
                l = size;
            }
            if (rs->index + l <= sizeof(rs->buf)) {
                memcpy(rs->buf + rs->index, buf, l);
            } else {
                fprintf(stderr, "serious error: oversized packet received,"
                    "connection terminated.\n");
                rs->index = rs->state = 0;
                return -1;
            }

            rs->index += l;
            buf += l;
            size -= l;
            if (rs->index >= rs->packet_len) {
                rs->index = 0;
                rs->state = 0;
                assert(rs->finalize);
                rs->finalize(rs);
            }
            break;
        }
    }

    assert(size == 0);
    return 0;
}