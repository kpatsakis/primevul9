static void vmxnet3_rx_update_descr(struct NetRxPkt *pkt,
    struct Vmxnet3_RxCompDesc *rxcd)
{
    int csum_ok, is_gso;
    bool isip4, isip6, istcp, isudp;
    struct virtio_net_hdr *vhdr;
    uint8_t offload_type;

    if (net_rx_pkt_is_vlan_stripped(pkt)) {
        rxcd->ts = 1;
        rxcd->tci = net_rx_pkt_get_vlan_tag(pkt);
    }

    if (!net_rx_pkt_has_virt_hdr(pkt)) {
        goto nocsum;
    }

    vhdr = net_rx_pkt_get_vhdr(pkt);
    /*
     * Checksum is valid when lower level tell so or when lower level
     * requires checksum offload telling that packet produced/bridged
     * locally and did travel over network after last checksum calculation
     * or production
     */
    csum_ok = VMXNET_FLAG_IS_SET(vhdr->flags, VIRTIO_NET_HDR_F_DATA_VALID) ||
              VMXNET_FLAG_IS_SET(vhdr->flags, VIRTIO_NET_HDR_F_NEEDS_CSUM);

    offload_type = vhdr->gso_type & ~VIRTIO_NET_HDR_GSO_ECN;
    is_gso = (offload_type != VIRTIO_NET_HDR_GSO_NONE) ? 1 : 0;

    if (!csum_ok && !is_gso) {
        goto nocsum;
    }

    net_rx_pkt_get_protocols(pkt, &isip4, &isip6, &isudp, &istcp);
    if ((!istcp && !isudp) || (!isip4 && !isip6)) {
        goto nocsum;
    }

    rxcd->cnc = 0;
    rxcd->v4 = isip4 ? 1 : 0;
    rxcd->v6 = isip6 ? 1 : 0;
    rxcd->tcp = istcp ? 1 : 0;
    rxcd->udp = isudp ? 1 : 0;
    rxcd->fcs = rxcd->tuc = rxcd->ipc = 1;
    return;

nocsum:
    rxcd->cnc = 1;
    return;
}