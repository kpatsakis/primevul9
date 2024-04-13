vmxnet3_on_tx_done_update_stats(VMXNET3State *s, int qidx,
    Vmxnet3PktStatus status)
{
    size_t tot_len = net_tx_pkt_get_total_len(s->tx_pkt);
    struct UPT1_TxStats *stats = &s->txq_descr[qidx].txq_stats;

    switch (status) {
    case VMXNET3_PKT_STATUS_OK:
        switch (net_tx_pkt_get_packet_type(s->tx_pkt)) {
        case ETH_PKT_BCAST:
            stats->bcastPktsTxOK++;
            stats->bcastBytesTxOK += tot_len;
            break;
        case ETH_PKT_MCAST:
            stats->mcastPktsTxOK++;
            stats->mcastBytesTxOK += tot_len;
            break;
        case ETH_PKT_UCAST:
            stats->ucastPktsTxOK++;
            stats->ucastBytesTxOK += tot_len;
            break;
        default:
            g_assert_not_reached();
        }

        if (s->offload_mode == VMXNET3_OM_TSO) {
            /*
             * According to VMWARE headers this statistic is a number
             * of packets after segmentation but since we don't have
             * this information in QEMU model, the best we can do is to
             * provide number of non-segmented packets
             */
            stats->TSOPktsTxOK++;
            stats->TSOBytesTxOK += tot_len;
        }
        break;

    case VMXNET3_PKT_STATUS_DISCARD:
        stats->pktsTxDiscard++;
        break;

    case VMXNET3_PKT_STATUS_ERROR:
        stats->pktsTxError++;
        break;

    default:
        g_assert_not_reached();
    }
}