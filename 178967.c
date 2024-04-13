static void ati_cursor_draw_line(VGACommonState *vga, uint8_t *d, int scr_y)
{
    ATIVGAState *s = container_of(vga, ATIVGAState, vga);
    uint32_t srcoff;
    uint32_t *dp = (uint32_t *)d;
    int i, j, h;

    if (!(s->regs.crtc_gen_cntl & CRTC2_CUR_EN) ||
        scr_y < vga->hw_cursor_y || scr_y >= vga->hw_cursor_y + 64 ||
        scr_y > s->regs.crtc_v_total_disp >> 16) {
        return;
    }
    /* FIXME handle cur_hv_offs correctly */
    srcoff = s->cursor_offset + (scr_y - vga->hw_cursor_y) * 16;
    dp = &dp[vga->hw_cursor_x];
    h = ((s->regs.crtc_h_total_disp >> 16) + 1) * 8;
    for (i = 0; i < 8; i++) {
        uint32_t color;
        uint8_t abits = vga_read_byte(vga, srcoff + i);
        uint8_t xbits = vga_read_byte(vga, srcoff + i + 8);
        for (j = 0; j < 8; j++, abits <<= 1, xbits <<= 1) {
            if (abits & BIT(7)) {
                if (xbits & BIT(7)) {
                    color = dp[i * 8 + j] ^ 0xffffffff; /* complement */
                } else {
                    continue; /* transparent, no change */
                }
            } else {
                color = (xbits & BIT(7) ? s->regs.cur_color1 :
                                          s->regs.cur_color0) | 0xff000000;
            }
            if (vga->hw_cursor_x + i * 8 + j >= h) {
                return; /* end of screen, don't span to next line */
            }
            dp[i * 8 + j] = color;
        }
    }
}