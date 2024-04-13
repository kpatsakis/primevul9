static void mark_curve_tr(fixed sx, fixed sy, fixed c1x, fixed c1y, fixed c2x, fixed c2y, fixed ex, fixed ey, fixed base_y, fixed height, int *table, int *index, int *id, int depth)
{
    fixed ax = (sx + c1x)>>1;
    fixed ay = (sy + c1y)>>1;
    fixed bx = (c1x + c2x)>>1;
    fixed by = (c1y + c2y)>>1;
    fixed cx = (c2x + ex)>>1;
    fixed cy = (c2y + ey)>>1;
    fixed dx = (ax + bx)>>1;
    fixed dy = (ay + by)>>1;
    fixed fx = (bx + cx)>>1;
    fixed fy = (by + cy)>>1;
    fixed gx = (dx + fx)>>1;
    fixed gy = (dy + fy)>>1;

    assert(depth >= 0);
    if (depth == 0) {
        *id += 1;
        mark_line_tr(sx, sy, ex, ey, base_y, height, table, index, *id);
    } else {
        depth--;
        mark_curve_tr(sx, sy, ax, ay, dx, dy, gx, gy, base_y, height, table, index, id, depth);
        mark_curve_tr(gx, gy, fx, fy, cx, cy, ex, ey, base_y, height, table, index, id, depth);
    }
}