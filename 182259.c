vmxnet3_indicate_packet(VMXNET3State *s)
{
    struct Vmxnet3_RxDesc rxd;
    PCIDevice *d = PCI_DEVICE(s);
    bool is_head = true;
    uint32_t rxd_idx;
    uint32_t rx_ridx = 0;

    struct Vmxnet3_RxCompDesc rxcd;
    uint32_t new_rxcd_gen = VMXNET3_INIT_GEN;
    hwaddr new_rxcd_pa = 0;
    hwaddr ready_rxcd_pa = 0;
    struct iovec *data = net_rx_pkt_get_iovec(s->rx_pkt);
    size_t bytes_copied = 0;
    size_t bytes_left = net_rx_pkt_get_total_len(s->rx_pkt);
    uint16_t num_frags = 0;
    size_t chunk_size;

    net_rx_pkt_dump(s->rx_pkt);

    while (bytes_left > 0) {

        /* cannot add more frags to packet */
        if (num_frags == s->max_rx_frags) {
            break;
        }

        new_rxcd_pa = vmxnet3_pop_rxc_descr(s, RXQ_IDX, &new_rxcd_gen);
        if (!new_rxcd_pa) {
            break;
        }

        if (!vmxnet3_get_next_rx_descr(s, is_head, &rxd, &rxd_idx, &rx_ridx)) {
            break;
        }

        chunk_size = MIN(bytes_left, rxd.len);
        vmxnet3_pci_dma_writev(d, data, bytes_copied, rxd.addr, chunk_size);
        bytes_copied += chunk_size;
        bytes_left -= chunk_size;

        vmxnet3_dump_rx_descr(&rxd);

        if (ready_rxcd_pa != 0) {
            vmxnet3_pci_dma_write_rxcd(d, ready_rxcd_pa, &rxcd);
        }

        memset(&rxcd, 0, sizeof(struct Vmxnet3_RxCompDesc));
        rxcd.rxdIdx = rxd_idx;
        rxcd.len = chunk_size;
        rxcd.sop = is_head;
        rxcd.gen = new_rxcd_gen;
        rxcd.rqID = RXQ_IDX + rx_ridx * s->rxq_num;

        if (bytes_left == 0) {
            vmxnet3_rx_update_descr(s->rx_pkt, &rxcd);
        }

        VMW_RIPRN("RX Completion descriptor: rxRing: %lu rxIdx %lu len %lu "
                  "sop %d csum_correct %lu",
                  (unsigned long) rx_ridx,
                  (unsigned long) rxcd.rxdIdx,
                  (unsigned long) rxcd.len,
                  (int) rxcd.sop,
                  (unsigned long) rxcd.tuc);

        is_head = false;
        ready_rxcd_pa = new_rxcd_pa;
        new_rxcd_pa = 0;
        num_frags++;
    }

    if (ready_rxcd_pa != 0) {
        rxcd.eop = 1;
        rxcd.err = (bytes_left != 0);

        vmxnet3_pci_dma_write_rxcd(d, ready_rxcd_pa, &rxcd);

        /* Flush RX descriptor changes */
        smp_wmb();
    }

    if (new_rxcd_pa != 0) {
        vmxnet3_revert_rxc_descr(s, RXQ_IDX);
    }

    vmxnet3_trigger_interrupt(s, s->rxq_descr[RXQ_IDX].intr_idx);

    if (bytes_left == 0) {
        vmxnet3_on_rx_done_update_stats(s, RXQ_IDX, VMXNET3_PKT_STATUS_OK);
        return true;
    } else if (num_frags == s->max_rx_frags) {
        vmxnet3_on_rx_done_update_stats(s, RXQ_IDX, VMXNET3_PKT_STATUS_ERROR);
        return false;
    } else {
        vmxnet3_on_rx_done_update_stats(s, RXQ_IDX,
                                        VMXNET3_PKT_STATUS_OUT_OF_BUF);
        return false;
    }
}