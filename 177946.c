void FloydSteinbergCloseG(gx_device_printer *pdev)
{
    gx_device_bjc_printer *dev = (gx_device_bjc_printer *)pdev;
    gs_free_object(pdev->memory, dev->FloydSteinbergErrorsG, "bjc error buffer");
}