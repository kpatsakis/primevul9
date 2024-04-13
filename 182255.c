vmxnet3_rx_filter_may_indicate(VMXNET3State *s, const void *data,
    size_t size)
{
    struct eth_header *ehdr = PKT_GET_ETH_HDR(data);

    if (VMXNET_FLAG_IS_SET(s->rx_mode, VMXNET3_RXM_PROMISC)) {
        return true;
    }

    if (!vmxnet3_is_registered_vlan(s, data)) {
        return false;
    }

    switch (net_rx_pkt_get_packet_type(s->rx_pkt)) {
    case ETH_PKT_UCAST:
        if (!VMXNET_FLAG_IS_SET(s->rx_mode, VMXNET3_RXM_UCAST)) {
            return false;
        }
        if (memcmp(s->conf.macaddr.a, ehdr->h_dest, ETH_ALEN)) {
            return false;
        }
        break;

    case ETH_PKT_BCAST:
        if (!VMXNET_FLAG_IS_SET(s->rx_mode, VMXNET3_RXM_BCAST)) {
            return false;
        }
        break;

    case ETH_PKT_MCAST:
        if (VMXNET_FLAG_IS_SET(s->rx_mode, VMXNET3_RXM_ALL_MULTI)) {
            return true;
        }
        if (!VMXNET_FLAG_IS_SET(s->rx_mode, VMXNET3_RXM_MCAST)) {
            return false;
        }
        if (!vmxnet3_is_allowed_mcast_group(s, ehdr->h_dest)) {
            return false;
        }
        break;

    default:
        g_assert_not_reached();
    }

    return true;
}