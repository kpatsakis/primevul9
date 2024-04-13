static void ohci_mem_write(void *opaque,
                           hwaddr addr,
                           uint64_t val,
                           unsigned size)
{
    OHCIState *ohci = opaque;

    /* Only aligned reads are allowed on OHCI */
    if (addr & 3) {
        trace_usb_ohci_mem_write_unaligned(addr);
        return;
    }

    if (addr >= 0x54 && addr < 0x54 + ohci->num_ports * 4) {
        /* HcRhPortStatus */
        ohci_port_set_status(ohci, (addr - 0x54) >> 2, val);
        return;
    }

    switch (addr >> 2) {
    case 1: /* HcControl */
        ohci_set_ctl(ohci, val);
        break;

    case 2: /* HcCommandStatus */
        /* SOC is read-only */
        val = (val & ~OHCI_STATUS_SOC);

        /* Bits written as '0' remain unchanged in the register */
        ohci->status |= val;

        if (ohci->status & OHCI_STATUS_HCR)
            ohci_soft_reset(ohci);
        break;

    case 3: /* HcInterruptStatus */
        ohci->intr_status &= ~val;
        ohci_intr_update(ohci);
        break;

    case 4: /* HcInterruptEnable */
        ohci->intr |= val;
        ohci_intr_update(ohci);
        break;

    case 5: /* HcInterruptDisable */
        ohci->intr &= ~val;
        ohci_intr_update(ohci);
        break;

    case 6: /* HcHCCA */
        ohci->hcca = val & OHCI_HCCA_MASK;
        break;

    case 7: /* HcPeriodCurrentED */
        /* Ignore writes to this read-only register, Linux does them */
        break;

    case 8: /* HcControlHeadED */
        ohci->ctrl_head = val & OHCI_EDPTR_MASK;
        break;

    case 9: /* HcControlCurrentED */
        ohci->ctrl_cur = val & OHCI_EDPTR_MASK;
        break;

    case 10: /* HcBulkHeadED */
        ohci->bulk_head = val & OHCI_EDPTR_MASK;
        break;

    case 11: /* HcBulkCurrentED */
        ohci->bulk_cur = val & OHCI_EDPTR_MASK;
        break;

    case 13: /* HcFmInterval */
        ohci->fsmps = (val & OHCI_FMI_FSMPS) >> 16;
        ohci->fit = (val & OHCI_FMI_FIT) >> 31;
        ohci_set_frame_interval(ohci, val);
        break;

    case 15: /* HcFmNumber */
        break;

    case 16: /* HcPeriodicStart */
        ohci->pstart = val & 0xffff;
        break;

    case 17: /* HcLSThreshold */
        ohci->lst = val & 0xffff;
        break;

    case 18: /* HcRhDescriptorA */
        ohci->rhdesc_a &= ~OHCI_RHA_RW_MASK;
        ohci->rhdesc_a |= val & OHCI_RHA_RW_MASK;
        break;

    case 19: /* HcRhDescriptorB */
        break;

    case 20: /* HcRhStatus */
        ohci_set_hub_status(ohci, val);
        break;

    /* PXA27x specific registers */
    case 24: /* HcStatus */
        ohci->hstatus &= ~(val & ohci->hmask);
        break;

    case 25: /* HcHReset */
        ohci->hreset = val & ~OHCI_HRESET_FSBIR;
        if (val & OHCI_HRESET_FSBIR)
            ohci_hard_reset(ohci);
        break;

    case 26: /* HcHInterruptEnable */
        ohci->hmask = val;
        break;

    case 27: /* HcHInterruptTest */
        ohci->htest = val;
        break;

    default:
        trace_usb_ohci_mem_write_bad_offset(addr);
        break;
    }
}