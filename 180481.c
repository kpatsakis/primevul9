static void mark_curve_app(cursor *cr, fixed sx, fixed sy, fixed c1x, fixed c1y, fixed c2x, fixed c2y, fixed ex, fixed ey, int depth)
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
        if (depth == 0)
            mark_line_app(cr, sx, sy, ex, ey);
        else {
            depth--;
            mark_curve_app(cr, sx, sy, ax, ay, dx, dy, gx, gy, depth);
            mark_curve_app(cr, gx, gy, fx, fy, cx, cy, ex, ey, depth);
        }
}