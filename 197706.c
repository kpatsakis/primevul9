int pdf_bitmap_char_update_bbox(gx_device_pdf * pdev,int x_offset, int y_offset, double x, double y)
{
    pdf_text_state_t *pts = pdev->text->text_state;
    gs_rect bbox;

    bbox.p.x = (pts->in.matrix.tx + x_offset) / (pdev->HWResolution[0] / 72);
    bbox.p.y = (pts->in.matrix.ty + y_offset) / (pdev->HWResolution[1] / 72);
    bbox.q.x = bbox.p.x + (x / (pdev->HWResolution[0] / 72));
    bbox.q.y = bbox.p.y + (y / (pdev->HWResolution[0] / 72));

    if (bbox.p.x < pdev->BBox.p.x)
        pdev->BBox.p.x = bbox.p.x;
    if (bbox.p.y < pdev->BBox.p.y)
        pdev->BBox.p.y = bbox.p.y;
    if (bbox.q.x > pdev->BBox.q.x)
        pdev->BBox.q.x = bbox.q.x;
    if (bbox.q.y > pdev->BBox.q.y)
        pdev->BBox.q.y = bbox.q.y;

    return 0;
}