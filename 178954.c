static void ati_vga_vblank_irq(void *opaque)
{
    ATIVGAState *s = opaque;

    timer_mod(&s->vblank_timer, qemu_clock_get_ns(QEMU_CLOCK_VIRTUAL) +
              NANOSECONDS_PER_SECOND / 60);
    s->regs.gen_int_status |= CRTC_VBLANK_INT;
    ati_vga_update_irq(s);
}