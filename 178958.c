static void ati_vga_switch_mode(ATIVGAState *s)
{
    DPRINTF("%d -> %d\n",
            s->mode, !!(s->regs.crtc_gen_cntl & CRTC2_EXT_DISP_EN));
    if (s->regs.crtc_gen_cntl & CRTC2_EXT_DISP_EN) {
        /* Extended mode enabled */
        s->mode = EXT_MODE;
        if (s->regs.crtc_gen_cntl & CRTC2_EN) {
            /* CRT controller enabled, use CRTC values */
            /* FIXME Should these be the same as VGA CRTC regs? */
            uint32_t offs = s->regs.crtc_offset & 0x07ffffff;
            int stride = (s->regs.crtc_pitch & 0x7ff) * 8;
            int bpp = 0;
            int h, v;

            if (s->regs.crtc_h_total_disp == 0) {
                s->regs.crtc_h_total_disp = ((640 / 8) - 1) << 16;
            }
            if (s->regs.crtc_v_total_disp == 0) {
                s->regs.crtc_v_total_disp = (480 - 1) << 16;
            }
            h = ((s->regs.crtc_h_total_disp >> 16) + 1) * 8;
            v = (s->regs.crtc_v_total_disp >> 16) + 1;
            switch (s->regs.crtc_gen_cntl & CRTC_PIX_WIDTH_MASK) {
            case CRTC_PIX_WIDTH_4BPP:
                bpp = 4;
                break;
            case CRTC_PIX_WIDTH_8BPP:
                bpp = 8;
                break;
            case CRTC_PIX_WIDTH_15BPP:
                bpp = 15;
                break;
            case CRTC_PIX_WIDTH_16BPP:
                bpp = 16;
                break;
            case CRTC_PIX_WIDTH_24BPP:
                bpp = 24;
                break;
            case CRTC_PIX_WIDTH_32BPP:
                bpp = 32;
                break;
            default:
                qemu_log_mask(LOG_UNIMP, "Unsupported bpp value\n");
            }
            assert(bpp != 0);
            DPRINTF("Switching to %dx%d %d %d @ %x\n", h, v, stride, bpp, offs);
            vbe_ioport_write_index(&s->vga, 0, VBE_DISPI_INDEX_ENABLE);
            vbe_ioport_write_data(&s->vga, 0, VBE_DISPI_DISABLED);
            s->vga.big_endian_fb = (s->regs.config_cntl & APER_0_ENDIAN ||
                                    s->regs.config_cntl & APER_1_ENDIAN ?
                                    true : false);
            /* reset VBE regs then set up mode */
            s->vga.vbe_regs[VBE_DISPI_INDEX_XRES] = h;
            s->vga.vbe_regs[VBE_DISPI_INDEX_YRES] = v;
            s->vga.vbe_regs[VBE_DISPI_INDEX_BPP] = bpp;
            /* enable mode via ioport so it updates vga regs */
            vbe_ioport_write_index(&s->vga, 0, VBE_DISPI_INDEX_ENABLE);
            vbe_ioport_write_data(&s->vga, 0, VBE_DISPI_ENABLED |
                VBE_DISPI_LFB_ENABLED | VBE_DISPI_NOCLEARMEM |
                (s->regs.dac_cntl & DAC_8BIT_EN ? VBE_DISPI_8BIT_DAC : 0));
            /* now set offset and stride after enable as that resets these */
            if (stride) {
                int bypp = DIV_ROUND_UP(bpp, BITS_PER_BYTE);

                vbe_ioport_write_index(&s->vga, 0, VBE_DISPI_INDEX_VIRT_WIDTH);
                vbe_ioport_write_data(&s->vga, 0, stride);
                stride *= bypp;
                if (offs % stride) {
                    DPRINTF("CRTC offset is not multiple of pitch\n");
                    vbe_ioport_write_index(&s->vga, 0,
                                           VBE_DISPI_INDEX_X_OFFSET);
                    vbe_ioport_write_data(&s->vga, 0, offs % stride / bypp);
                }
                vbe_ioport_write_index(&s->vga, 0, VBE_DISPI_INDEX_Y_OFFSET);
                vbe_ioport_write_data(&s->vga, 0, offs / stride);
                DPRINTF("VBE offset (%d,%d), vbe_start_addr=%x\n",
                        s->vga.vbe_regs[VBE_DISPI_INDEX_X_OFFSET],
                        s->vga.vbe_regs[VBE_DISPI_INDEX_Y_OFFSET],
                        s->vga.vbe_start_addr);
            }
        }
    } else {
        /* VGA mode enabled */
        s->mode = VGA_MODE;
        vbe_ioport_write_index(&s->vga, 0, VBE_DISPI_INDEX_ENABLE);
        vbe_ioport_write_data(&s->vga, 0, VBE_DISPI_DISABLED);
    }
}