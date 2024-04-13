static MemTxResult gic_hyp_write(void *opaque, int cpu, hwaddr addr,
                                 uint64_t value, MemTxAttrs attrs)
{
    GICState *s = ARM_GIC(opaque);
    int vcpu = cpu + GIC_NCPU;

    trace_gic_hyp_write(addr, value);

    switch (addr) {
    case A_GICH_HCR: /* Hypervisor Control */
        s->h_hcr[cpu] = value & GICH_HCR_MASK;
        break;

    case A_GICH_VMCR: /* Virtual Machine Control */
        gic_vmcr_write(s, value, attrs);
        break;

    case A_GICH_APR: /* Active Priorities */
        s->h_apr[cpu] = value;
        s->running_priority[vcpu] = gic_get_prio_from_apr_bits(s, vcpu);
        break;

    case A_GICH_LR0 ... A_GICH_LR63: /* List Registers */
    {
        int lr_idx = (addr - A_GICH_LR0) / 4;

        if (lr_idx > s->num_lrs) {
            return MEMTX_OK;
        }

        s->h_lr[lr_idx][cpu] = value & GICH_LR_MASK;
        trace_gic_lr_entry(cpu, lr_idx, s->h_lr[lr_idx][cpu]);
        break;
    }

    default:
        qemu_log_mask(LOG_GUEST_ERROR,
                      "gic_hyp_write: Bad offset %" HWADDR_PRIx "\n", addr);
        return MEMTX_OK;
    }

    gic_update_virt(s);
    return MEMTX_OK;
}