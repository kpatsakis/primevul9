pdf_get_text_state_values(gx_device_pdf *pdev, pdf_text_state_values_t *ptsv)
{
    *ptsv = pdev->text->text_state->in;
}