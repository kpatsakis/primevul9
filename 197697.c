pdf_from_stream_to_text(gx_device_pdf *pdev)
{
    pdf_text_state_t *pts = pdev->text->text_state;

    gs_make_identity(&pts->out.matrix);
    pts->line_start.x = pts->line_start.y = 0;
    pts->continue_line = false; /* Not sure, probably doesn't matter. */
    pts->buffer.count_chars = 0;
    pts->buffer.count_moves = 0;
    return 0;
}