static void mark_curve_tr_app(cursor_tr *cr, fixed sx, fixed sy, fixed c1x, fixed c1y, fixed c2x, fixed c2y, fixed ex, fixed ey, int depth, int *id)
{
        int ax = (sx + c1x)>>1;
        int ay = (sy + c1y)>>1;
        int bx = (c1x + c2x)>>1;
        int by = (c1y + c2y)>>1;
        int cx = (c2x + ex)>>1;
        int cy = (c2y + ey)>>1;
        int dx = (ax + bx)>>1;
        int dy = (ay + by)>>1;
        int fx = (bx + cx)>>1;
        int fy = (by + cy)>>1;
        int gx = (dx + fx)>>1;
        int gy = (dy + fy)>>1;

        assert(depth >= 0);
        if (depth == 0) {
            *id += 1;
            mark_line_tr_app(cr, sx, sy, ex, ey, *id);
        } else {
            depth--;
            mark_curve_tr_app(cr, sx, sy, ax, ay, dx, dy, gx, gy, depth, id);
            mark_curve_tr_app(cr, gx, gy, fx, fy, cx, cy, ex, ey, depth, id);
        }
}