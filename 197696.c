flush_text_buffer(gx_device_pdf *pdev)
{
    pdf_text_state_t *pts = pdev->text->text_state;
    stream *s = pdev->strm;

    if (pts->buffer.count_chars != 0) {
        pdf_font_resource_t *pdfont = pts->in.pdfont;
        int code = pdf_assign_font_object_id(pdev, pdfont);

        if (code < 0)
            return code;
        code = pdf_add_resource(pdev, pdev->substream_Resources, "/Font", (pdf_resource_t *)pdfont);
        if (code < 0)
            return code;
    }
    if (pts->buffer.count_moves > 0) {
        int i, cur = 0;

        if (pts->use_leading)
            stream_puts(s, "T*");
        stream_puts(s, "[");
        for (i = 0; i < pts->buffer.count_moves; ++i) {
            int next = pts->buffer.moves[i].index;

            pdf_put_string(pdev, pts->buffer.chars + cur, next - cur);
            pprintg1(s, "%g", pts->buffer.moves[i].amount);
            cur = next;
        }
        if (pts->buffer.count_chars > cur)
            pdf_put_string(pdev, pts->buffer.chars + cur,
                           pts->buffer.count_chars - cur);
        stream_puts(s, "]TJ\n");
    } else {
        pdf_put_string(pdev, pts->buffer.chars, pts->buffer.count_chars);
        stream_puts(s, (pts->use_leading ? "'\n" : "Tj\n"));
    }
    pts->buffer.count_chars = 0;
    pts->buffer.count_moves = 0;
    pts->use_leading = false;
    return 0;
}