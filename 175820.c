gx_upright_get_initial_matrix(gx_device * dev, register gs_matrix * pmat)
{
    pmat->xx = dev->HWResolution[0] / 72.0;	/* x_pixels_per_inch */
    pmat->xy = 0;
    pmat->yx = 0;
    pmat->yy = dev->HWResolution[1] / 72.0;	/* y_pixels_per_inch */
    /****** tx/y is WRONG for devices with ******/
    /****** arbitrary initial matrix ******/
    pmat->tx = 0;
    pmat->ty = 0;
}