pdf_set_text_wmode(gx_device_pdf *pdev, int wmode)
{
    pdf_text_state_t *pts = pdev->text->text_state;

    pts->wmode = wmode;
}