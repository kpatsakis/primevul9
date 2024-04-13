pdf_text_position(const gx_device_pdf *pdev, gs_point *ppt)
{
    pdf_text_state_t *pts = pdev->text->text_state;

    ppt->x = pts->in.matrix.tx;
    ppt->y = pts->in.matrix.ty;
}