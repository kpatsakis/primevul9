static void rtl8139_RxBufPtr_write(RTL8139State *s, uint32_t val)
{
    DPRINTF("RxBufPtr write val=0x%04x\n", val);

    /* this value is off by 16 */
    s->RxBufPtr = MOD2(val + 0x10, s->RxBufferSize);

    /* more buffer space may be available so try to receive */
    qemu_flush_queued_packets(qemu_get_queue(s->nic));

    DPRINTF(" CAPR write: rx buffer length %d head 0x%04x read 0x%04x\n",
        s->RxBufferSize, s->RxBufAddr, s->RxBufPtr);
}