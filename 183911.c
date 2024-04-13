static void send_color_map(VncState *vs)
{
    int i;

    vnc_write_u8(vs, VNC_MSG_SERVER_SET_COLOUR_MAP_ENTRIES);
    vnc_write_u8(vs,  0);    /* padding     */
    vnc_write_u16(vs, 0);    /* first color */
    vnc_write_u16(vs, 256);  /* # of colors */

    for (i = 0; i < 256; i++) {
        PixelFormat *pf = &vs->client_pf;

        vnc_write_u16(vs, (((i >> pf->rshift) & pf->rmax) << (16 - pf->rbits)));
        vnc_write_u16(vs, (((i >> pf->gshift) & pf->gmax) << (16 - pf->gbits)));
        vnc_write_u16(vs, (((i >> pf->bshift) & pf->bmax) << (16 - pf->bbits)));
    }
}