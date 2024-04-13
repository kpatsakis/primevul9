pdf_set_text_matrix(gx_device_pdf * pdev)
{
    pdf_text_state_t *pts = pdev->text->text_state;
    stream *s = pdev->strm;

    pts->use_leading = false;
    if (matrix_is_compatible(&pts->out.matrix, &pts->in.matrix)) {
        gs_point dist;
        int code;

        code = set_text_distance(&dist, pts->start.x - pts->line_start.x,
                          pts->start.y - pts->line_start.y, &pts->in.matrix);
        if (code < 0)
            return code;
        if (dist.x == 0 && dist.y < 0) {
            /* Use TL, if needed, and T* or '. */
            float dist_y = (float)-dist.y;

            if (fabs(pts->leading - dist_y) > 0.0005) {
                pprintg1(s, "%g TL\n", dist_y);
                pts->leading = dist_y;
            }
            pts->use_leading = true;
        } else {
            /* Use Td. */
            pprintg2(s, "%g %g Td\n", dist.x, dist.y);
        }
    } else {			/* Use Tm. */
        /*
         * See stream_to_text in gdevpdfu.c for why we need the following
         * matrix adjustments.
         */
        double sx = 72.0 / pdev->HWResolution[0],
            sy = 72.0 / pdev->HWResolution[1], ax = sx, bx = sx, ay = sy, by = sy;

        /* We have a precision limit on decimal places with %g, make sure
         * we don't end up with values which will be truncated to 0
         */
        if (pts->in.matrix.xx != 0 && fabs(pts->in.matrix.xx) * ax < 0.00000001)
            ax = ceil(0.00000001 / pts->in.matrix.xx);
        if (pts->in.matrix.xy != 0 && fabs(pts->in.matrix.xy) * ay < 0.00000001)
            ay = ceil(0.00000001 / pts->in.matrix.xy);
        if (pts->in.matrix.yx != 0 && fabs(pts->in.matrix.yx) * bx < 0.00000001)
            bx = ceil(0.00000001 / pts->in.matrix.yx);
        if (pts->in.matrix.yy != 0 && fabs(pts->in.matrix.yy) * by < 0.00000001)
            by = ceil(0.00000001 / pts->in.matrix.yy);
        pprintg6(s, "%g %g %g %g %g %g Tm\n",
                 pts->in.matrix.xx * ax, pts->in.matrix.xy * ay,
                 pts->in.matrix.yx * bx, pts->in.matrix.yy * by,
                 pts->start.x * sx, pts->start.y * sy);
    }
    pts->line_start.x = pts->start.x;
    pts->line_start.y = pts->start.y;
    pts->out.matrix = pts->in.matrix;
    return 0;
}