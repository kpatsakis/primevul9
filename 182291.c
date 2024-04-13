static bool vmxnet3_peer_has_vnet_hdr(VMXNET3State *s)
{
    NetClientState *nc = qemu_get_queue(s->nic);

    if (qemu_has_vnet_hdr(nc->peer)) {
        return true;
    }

    return false;
}