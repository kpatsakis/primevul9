void FloydSteinbergCloseC(gx_device_printer *pdev)
{
    gx_device_bjc_printer *dev = (gx_device_bjc_printer *)pdev;
    gs_free_object(pdev->memory, dev->FloydSteinbergErrorsC,
                   "bjc CMY error buffer");
}