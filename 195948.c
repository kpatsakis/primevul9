void net_socket_rs_init(SocketReadState *rs,
                        SocketReadStateFinalize *finalize,
                        bool vnet_hdr)
{
    rs->state = 0;
    rs->vnet_hdr = vnet_hdr;
    rs->index = 0;
    rs->packet_len = 0;
    rs->vnet_hdr_len = 0;
    memset(rs->buf, 0, sizeof(rs->buf));
    rs->finalize = finalize;
}