vmxnet3_io_bar1_write(void *opaque,
                      hwaddr addr,
                      uint64_t val,
                      unsigned size)
{
    VMXNET3State *s = opaque;

    switch (addr) {
    /* Vmxnet3 Revision Report Selection */
    case VMXNET3_REG_VRRS:
        VMW_CBPRN("Write BAR1 [VMXNET3_REG_VRRS] = %" PRIx64 ", size %d",
                  val, size);
        break;

    /* UPT Version Report Selection */
    case VMXNET3_REG_UVRS:
        VMW_CBPRN("Write BAR1 [VMXNET3_REG_UVRS] = %" PRIx64 ", size %d",
                  val, size);
        break;

    /* Driver Shared Address Low */
    case VMXNET3_REG_DSAL:
        VMW_CBPRN("Write BAR1 [VMXNET3_REG_DSAL] = %" PRIx64 ", size %d",
                  val, size);
        /*
         * Guest driver will first write the low part of the shared
         * memory address. We save it to temp variable and set the
         * shared address only after we get the high part
         */
        if (val == 0) {
            vmxnet3_deactivate_device(s);
        }
        s->temp_shared_guest_driver_memory = val;
        s->drv_shmem = 0;
        break;

    /* Driver Shared Address High */
    case VMXNET3_REG_DSAH:
        VMW_CBPRN("Write BAR1 [VMXNET3_REG_DSAH] = %" PRIx64 ", size %d",
                  val, size);
        /*
         * Set the shared memory between guest driver and device.
         * We already should have low address part.
         */
        s->drv_shmem = s->temp_shared_guest_driver_memory | (val << 32);
        break;

    /* Command */
    case VMXNET3_REG_CMD:
        VMW_CBPRN("Write BAR1 [VMXNET3_REG_CMD] = %" PRIx64 ", size %d",
                  val, size);
        vmxnet3_handle_command(s, val);
        break;

    /* MAC Address Low */
    case VMXNET3_REG_MACL:
        VMW_CBPRN("Write BAR1 [VMXNET3_REG_MACL] = %" PRIx64 ", size %d",
                  val, size);
        s->temp_mac = val;
        break;

    /* MAC Address High */
    case VMXNET3_REG_MACH:
        VMW_CBPRN("Write BAR1 [VMXNET3_REG_MACH] = %" PRIx64 ", size %d",
                  val, size);
        vmxnet3_set_variable_mac(s, val, s->temp_mac);
        break;

    /* Interrupt Cause Register */
    case VMXNET3_REG_ICR:
        VMW_CBPRN("Write BAR1 [VMXNET3_REG_ICR] = %" PRIx64 ", size %d",
                  val, size);
        g_assert_not_reached();
        break;

    /* Event Cause Register */
    case VMXNET3_REG_ECR:
        VMW_CBPRN("Write BAR1 [VMXNET3_REG_ECR] = %" PRIx64 ", size %d",
                  val, size);
        vmxnet3_ack_events(s, val);
        break;

    default:
        VMW_CBPRN("Unknown Write to BAR1 [%" PRIx64 "] = %" PRIx64 ", size %d",
                  addr, val, size);
        break;
    }
}