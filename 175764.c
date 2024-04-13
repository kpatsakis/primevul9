gx_default_get_initial_matrix(gx_device * dev, register gs_matrix * pmat)
{
    /* NB this device has no paper margins */
    double fs_res = dev->HWResolution[0] / 72.0;
    double ss_res = dev->HWResolution[1] / 72.0;

    switch(dev->LeadingEdge & LEADINGEDGE_MASK) {
    case 1: /* 90 degrees */
        pmat->xx = 0;
        pmat->xy = -ss_res;
        pmat->yx = -fs_res;
        pmat->yy = 0;
        pmat->tx = (float)dev->width;
        pmat->ty = (float)dev->height;
        break;
    case 2: /* 180 degrees */
        pmat->xx = -fs_res;
        pmat->xy = 0;
        pmat->yx = 0;
        pmat->yy = ss_res;
        pmat->tx = (float)dev->width;
        pmat->ty = 0;
        break;
    case 3: /* 270 degrees */
        pmat->xx = 0;
        pmat->xy = ss_res;
        pmat->yx = fs_res;
        pmat->yy = 0;
        pmat->tx = 0;
        pmat->ty = 0;
        break;
    default:
    case 0:
        pmat->xx = fs_res;
        pmat->xy = 0;
        pmat->yx = 0;
        pmat->yy = -ss_res;
        pmat->tx = 0;
        pmat->ty = (float)dev->height;
        /****** tx/y is WRONG for devices with ******/
        /****** arbitrary initial matrix ******/
        break;
    }
}