int qemu_can_receive_packet(NetClientState *nc)
{
    if (nc->receive_disabled) {
        return 0;
    } else if (nc->info->can_receive &&
               !nc->info->can_receive(nc)) {
        return 0;
    }
    return 1;
}