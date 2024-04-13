static void vmxnet3_trigger_interrupt(VMXNET3State *s, int lidx)
{
    PCIDevice *d = PCI_DEVICE(s);
    s->interrupt_states[lidx].is_pending = true;
    vmxnet3_update_interrupt_line_state(s, lidx);

    if (s->msix_used && msix_enabled(d) && s->auto_int_masking) {
        goto do_automask;
    }

    if (msi_enabled(d) && s->auto_int_masking) {
        goto do_automask;
    }

    return;

do_automask:
    s->interrupt_states[lidx].is_masked = true;
    vmxnet3_update_interrupt_line_state(s, lidx);
}