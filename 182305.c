vmxnet3_on_rx_done_update_stats(VMXNET3State *s,
                                int qidx,
                                Vmxnet3PktStatus status)
{
    struct UPT1_RxStats *stats = &s->rxq_descr[qidx].rxq_stats;
    size_t tot_len = net_rx_pkt_get_total_len(s->rx_pkt);

    switch (status) {
    case VMXNET3_PKT_STATUS_OUT_OF_BUF:
        stats->pktsRxOutOfBuf++;
        break;

    case VMXNET3_PKT_STATUS_ERROR:
        stats->pktsRxError++;
        break;
    case VMXNET3_PKT_STATUS_OK:
        switch (net_rx_pkt_get_packet_type(s->rx_pkt)) {
        case ETH_PKT_BCAST:
            stats->bcastPktsRxOK++;
            stats->bcastBytesRxOK += tot_len;
            break;
        case ETH_PKT_MCAST:
            stats->mcastPktsRxOK++;
            stats->mcastBytesRxOK += tot_len;
            break;
        case ETH_PKT_UCAST:
            stats->ucastPktsRxOK++;
            stats->ucastBytesRxOK += tot_len;
            break;
        default:
            g_assert_not_reached();
        }

        if (tot_len > s->mtu) {
            stats->LROPktsRxOK++;
            stats->LROBytesRxOK += tot_len;
        }
        break;
    default:
        g_assert_not_reached();
    }
}