vmxnet3_io_bar0_write(void *opaque, hwaddr addr,
                      uint64_t val, unsigned size)
{
    VMXNET3State *s = opaque;

    if (!s->device_active) {
        return;
    }

    if (VMW_IS_MULTIREG_ADDR(addr, VMXNET3_REG_TXPROD,
                        VMXNET3_DEVICE_MAX_TX_QUEUES, VMXNET3_REG_ALIGN)) {
        int tx_queue_idx =
            VMW_MULTIREG_IDX_BY_ADDR(addr, VMXNET3_REG_TXPROD,
                                     VMXNET3_REG_ALIGN);
        if (tx_queue_idx <= s->txq_num) {
            vmxnet3_process_tx_queue(s, tx_queue_idx);
        } else {
            qemu_log_mask(LOG_GUEST_ERROR, "vmxnet3: Illegal TX queue %d/%d\n",
                          tx_queue_idx, s->txq_num);
        }
        return;
    }

    if (VMW_IS_MULTIREG_ADDR(addr, VMXNET3_REG_IMR,
                        VMXNET3_MAX_INTRS, VMXNET3_REG_ALIGN)) {
        int l = VMW_MULTIREG_IDX_BY_ADDR(addr, VMXNET3_REG_IMR,
                                         VMXNET3_REG_ALIGN);

        VMW_CBPRN("Interrupt mask for line %d written: 0x%" PRIx64, l, val);

        vmxnet3_on_interrupt_mask_changed(s, l, val);
        return;
    }

    if (VMW_IS_MULTIREG_ADDR(addr, VMXNET3_REG_RXPROD,
                        VMXNET3_DEVICE_MAX_RX_QUEUES, VMXNET3_REG_ALIGN) ||
       VMW_IS_MULTIREG_ADDR(addr, VMXNET3_REG_RXPROD2,
                        VMXNET3_DEVICE_MAX_RX_QUEUES, VMXNET3_REG_ALIGN)) {
        return;
    }

    VMW_WRPRN("BAR0 unknown write [%" PRIx64 "] = %" PRIx64 ", size %d",
              (uint64_t) addr, val, size);
}