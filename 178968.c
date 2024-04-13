static void ati_vga_reset(DeviceState *dev)
{
    ATIVGAState *s = ATI_VGA(dev);

    timer_del(&s->vblank_timer);
    ati_vga_update_irq(s);

    /* reset vga */
    vga_common_reset(&s->vga);
    s->mode = VGA_MODE;
}