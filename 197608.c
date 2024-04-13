static MemTxResult gic_cpu_write(GICState *s, int cpu, int offset,
                                 uint32_t value, MemTxAttrs attrs)
{
    trace_gic_cpu_write(gic_is_vcpu(cpu) ? "vcpu" : "cpu",
                        gic_get_vcpu_real_id(cpu), offset, value);

    switch (offset) {
    case 0x00: /* Control */
        gic_set_cpu_control(s, cpu, value, attrs);
        break;
    case 0x04: /* Priority mask */
        gic_set_priority_mask(s, cpu, value, attrs);
        break;
    case 0x08: /* Binary Point */
        if (gic_cpu_ns_access(s, cpu, attrs)) {
            if (s->cpu_ctlr[cpu] & GICC_CTLR_CBPR) {
                /* WI when CBPR is 1 */
                return MEMTX_OK;
            } else {
                s->abpr[cpu] = MAX(value & 0x7, GIC_MIN_ABPR);
            }
        } else {
            int min_bpr = gic_is_vcpu(cpu) ? GIC_VIRT_MIN_BPR : GIC_MIN_BPR;
            s->bpr[cpu] = MAX(value & 0x7, min_bpr);
        }
        break;
    case 0x10: /* End Of Interrupt */
        gic_complete_irq(s, cpu, value & 0x3ff, attrs);
        return MEMTX_OK;
    case 0x1c: /* Aliased Binary Point */
        if (!gic_has_groups(s) || (gic_cpu_ns_access(s, cpu, attrs))) {
            /* unimplemented, or NS access: RAZ/WI */
            return MEMTX_OK;
        } else {
            s->abpr[cpu] = MAX(value & 0x7, GIC_MIN_ABPR);
        }
        break;
    case 0xd0: case 0xd4: case 0xd8: case 0xdc:
    {
        int regno = (offset - 0xd0) / 4;
        int nr_aprs = gic_is_vcpu(cpu) ? GIC_VIRT_NR_APRS : GIC_NR_APRS;

        if (regno >= nr_aprs || s->revision != 2) {
            return MEMTX_OK;
        }
        if (gic_is_vcpu(cpu)) {
            s->h_apr[gic_get_vcpu_real_id(cpu)] = value;
        } else if (gic_cpu_ns_access(s, cpu, attrs)) {
            /* NS view of GICC_APR<n> is the top half of GIC_NSAPR<n> */
            gic_apr_write_ns_view(s, regno, cpu, value);
        } else {
            s->apr[regno][cpu] = value;
        }
        break;
    }
    case 0xe0: case 0xe4: case 0xe8: case 0xec:
    {
        int regno = (offset - 0xe0) / 4;

        if (regno >= GIC_NR_APRS || s->revision != 2) {
            return MEMTX_OK;
        }
        if (gic_is_vcpu(cpu)) {
            return MEMTX_OK;
        }
        if (!gic_has_groups(s) || (gic_cpu_ns_access(s, cpu, attrs))) {
            return MEMTX_OK;
        }
        s->nsapr[regno][cpu] = value;
        break;
    }
    case 0x1000:
        /* GICC_DIR */
        gic_deactivate_irq(s, cpu, value & 0x3ff, attrs);
        break;
    default:
        qemu_log_mask(LOG_GUEST_ERROR,
                      "gic_cpu_write: Bad offset %x\n", (int)offset);
        return MEMTX_OK;
    }

    if (gic_is_vcpu(cpu)) {
        gic_update_virt(s);
    } else {
        gic_update(s);
    }

    return MEMTX_OK;
}