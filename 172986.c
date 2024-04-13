pdf14_copy_planes(gx_device * dev, const byte * data, int data_x, int raster,
                  gx_bitmap_id id, int x, int y, int w, int h, int plane_height)
{
    pdf14_device *pdev = (pdf14_device *)dev;
#if RAW_DUMP
    pdf14_ctx *ctx = pdev->ctx;
#endif
    pdf14_buf *buf = pdev->ctx->stack;
    uchar num_planes = dev->color_info.num_components;
    byte *dptr = (byte *)data + data_x;
    int yinc, xinc;
    uchar pi;
    gx_drawing_color dcolor;
    int code = 0;

    fit_fill_xywh(dev, x, y, w, h);
    if (w <= 0 || h <= 0)
        return 0;

    dcolor.type = gx_dc_type_devn;
    /* Because of the complexity of the blending and my desire to finish
       this planar sep device work, I am going to make this a series of
       rect fills.  ToDo: optimize this for more efficient planar operation.
       It would be interesting to use the put_image procedure. */
    for (yinc = 0; yinc < h; yinc++) {
        for (xinc = 0; xinc < w; xinc++) {
            for (pi = 0; pi < num_planes; pi++) {
                dcolor.colors.devn.values[pi] = *(dptr + plane_height * raster * pi) << 8;
            }
            if (buf->knockout)
                code =
                    pdf14_mark_fill_rectangle_ko_simple(dev, x + xinc,
                                                        y + yinc, 1, 1, 0,
                                                        &dcolor, true);
            else
                code =
                    pdf14_mark_fill_rectangle(dev, x + xinc, y + yinc, 1, 1, 0,
                                               &dcolor, true);
            if (code < 0)
                return code;
            dptr++;
        }
        dptr = (byte *)data + raster * yinc + data_x;
    }
    return code;
}