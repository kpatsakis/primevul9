static MemTxResult gic_hyp_read(void *opaque, int cpu, hwaddr addr,
                                uint64_t *data, MemTxAttrs attrs)
{
    GICState *s = ARM_GIC(opaque);
    int vcpu = cpu + GIC_NCPU;

    switch (addr) {
    case A_GICH_HCR: /* Hypervisor Control */
        *data = s->h_hcr[cpu];
        break;

    case A_GICH_VTR: /* VGIC Type */
        *data = FIELD_DP32(0, GICH_VTR, ListRegs, s->num_lrs - 1);
        *data = FIELD_DP32(*data, GICH_VTR, PREbits,
                           GIC_VIRT_MAX_GROUP_PRIO_BITS - 1);
        *data = FIELD_DP32(*data, GICH_VTR, PRIbits,
                           (7 - GIC_VIRT_MIN_BPR) - 1);
        break;

    case A_GICH_VMCR: /* Virtual Machine Control */
        *data = FIELD_DP32(0, GICH_VMCR, VMCCtlr,
                           extract32(s->cpu_ctlr[vcpu], 0, 10));
        *data = FIELD_DP32(*data, GICH_VMCR, VMABP, s->abpr[vcpu]);
        *data = FIELD_DP32(*data, GICH_VMCR, VMBP, s->bpr[vcpu]);
        *data = FIELD_DP32(*data, GICH_VMCR, VMPriMask,
                           extract32(s->priority_mask[vcpu], 3, 5));
        break;

    case A_GICH_MISR: /* Maintenance Interrupt Status */
        *data = s->h_misr[cpu];
        break;

    case A_GICH_EISR0: /* End of Interrupt Status 0 and 1 */
    case A_GICH_EISR1:
        *data = gic_compute_eisr(s, cpu, (addr - A_GICH_EISR0) * 8);
        break;

    case A_GICH_ELRSR0: /* Empty List Status 0 and 1 */
    case A_GICH_ELRSR1:
        *data = gic_compute_elrsr(s, cpu, (addr - A_GICH_ELRSR0) * 8);
        break;

    case A_GICH_APR: /* Active Priorities */
        *data = s->h_apr[cpu];
        break;

    case A_GICH_LR0 ... A_GICH_LR63: /* List Registers */
    {
        int lr_idx = (addr - A_GICH_LR0) / 4;

        if (lr_idx > s->num_lrs) {
            *data = 0;
        } else {
            *data = s->h_lr[lr_idx][cpu];
        }
        break;
    }

    default:
        qemu_log_mask(LOG_GUEST_ERROR,
                      "gic_hyp_read: Bad offset %" HWADDR_PRIx "\n", addr);
        return MEMTX_OK;
    }

    trace_gic_hyp_read(addr, *data);
    return MEMTX_OK;
}