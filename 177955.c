FloydSteinbergInitC(gx_device_printer * pdev)
{
    int i;
    gx_device_bjc_printer *dev = (gx_device_bjc_printer *)pdev;

    dev-> FloydSteinbergErrorsC = (int *) gs_alloc_bytes(pdev->memory,
                                            3*sizeof(int)*(pdev->width+3),
                                              "bjc CMY error buffer");
    if (dev->FloydSteinbergErrorsC == 0 ) /* can't allocate error buffer */
        return -1;

    for (i=0; i < 3 * (pdev->width+3); i++) dev->FloydSteinbergErrorsC[i] = 0;

    dev->FloydSteinbergDirectionForward=true;
    bjc_rgb_to_cmy(dev->paperColor.red,
                   dev->paperColor.green,
                   dev->paperColor.blue,
                   &dev->FloydSteinbergC,
                   &dev->FloydSteinbergM,
                   &dev->FloydSteinbergY);

    dev->FloydSteinbergC <<= 4;
    dev->FloydSteinbergM <<= 4;
    dev->FloydSteinbergY <<= 4;
    bjc_init_tresh(dev, dev->rnd);
    return 0;
}