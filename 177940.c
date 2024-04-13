FloydSteinbergInitG(gx_device_printer * pdev)
{
    int i;
    gx_device_bjc_printer *dev = (gx_device_bjc_printer *)pdev;

    dev->FloydSteinbergErrorsG = (int *) gs_alloc_bytes(pdev->memory,
                                              sizeof(int)*(pdev->width+3),
                                              "bjc error buffer");
    if (dev->FloydSteinbergErrorsG == 0) /* can't allocate error buffer */
        return -1;
    dev->FloydSteinbergDirectionForward=true;

    for (i=0; i < pdev->width+3; i++) dev->FloydSteinbergErrorsG[i] = 0;
                                                              /* clear */
    bjc_rgb_to_gray(dev->paperColor.red,
                    dev->paperColor.green,
                    dev->paperColor.blue,
                    &dev->FloydSteinbergG);
    dev->FloydSteinbergG = (255 - dev->FloydSteinbergG) << 4;  /* Maybe */
    bjc_init_tresh(dev, dev->rnd);
    return 0;
}