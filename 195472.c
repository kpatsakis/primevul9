static uint64_t ohci_mem_read(void *opaque,
                              hwaddr addr,
                              unsigned size)
{
    OHCIState *ohci = opaque;
    uint32_t retval;

    /* Only aligned reads are allowed on OHCI */
    if (addr & 3) {
        trace_usb_ohci_mem_read_unaligned(addr);
        return 0xffffffff;
    } else if (addr >= 0x54 && addr < 0x54 + ohci->num_ports * 4) {
        /* HcRhPortStatus */
        retval = ohci->rhport[(addr - 0x54) >> 2].ctrl | OHCI_PORT_PPS;
    } else {
        switch (addr >> 2) {
        case 0: /* HcRevision */
            retval = 0x10;
            break;

        case 1: /* HcControl */
            retval = ohci->ctl;
            break;

        case 2: /* HcCommandStatus */
            retval = ohci->status;
            break;

        case 3: /* HcInterruptStatus */
            retval = ohci->intr_status;
            break;

        case 4: /* HcInterruptEnable */
        case 5: /* HcInterruptDisable */
            retval = ohci->intr;
            break;

        case 6: /* HcHCCA */
            retval = ohci->hcca;
            break;

        case 7: /* HcPeriodCurrentED */
            retval = ohci->per_cur;
            break;

        case 8: /* HcControlHeadED */
            retval = ohci->ctrl_head;
            break;

        case 9: /* HcControlCurrentED */
            retval = ohci->ctrl_cur;
            break;

        case 10: /* HcBulkHeadED */
            retval = ohci->bulk_head;
            break;

        case 11: /* HcBulkCurrentED */
            retval = ohci->bulk_cur;
            break;

        case 12: /* HcDoneHead */
            retval = ohci->done;
            break;

        case 13: /* HcFmInterretval */
            retval = (ohci->fit << 31) | (ohci->fsmps << 16) | (ohci->fi);
            break;

        case 14: /* HcFmRemaining */
            retval = ohci_get_frame_remaining(ohci);
            break;

        case 15: /* HcFmNumber */
            retval = ohci->frame_number;
            break;

        case 16: /* HcPeriodicStart */
            retval = ohci->pstart;
            break;

        case 17: /* HcLSThreshold */
            retval = ohci->lst;
            break;

        case 18: /* HcRhDescriptorA */
            retval = ohci->rhdesc_a;
            break;

        case 19: /* HcRhDescriptorB */
            retval = ohci->rhdesc_b;
            break;

        case 20: /* HcRhStatus */
            retval = ohci->rhstatus;
            break;

        /* PXA27x specific registers */
        case 24: /* HcStatus */
            retval = ohci->hstatus & ohci->hmask;
            break;

        case 25: /* HcHReset */
            retval = ohci->hreset;
            break;

        case 26: /* HcHInterruptEnable */
            retval = ohci->hmask;
            break;

        case 27: /* HcHInterruptTest */
            retval = ohci->htest;
            break;

        default:
            trace_usb_ohci_mem_read_bad_offset(addr);
            retval = 0xffffffff;
        }
    }

    return retval;
}