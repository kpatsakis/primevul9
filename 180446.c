static void mark_curve(fixed sx, fixed sy, fixed c1x, fixed c1y, fixed c2x, fixed c2y, fixed ex, fixed ey, fixed base_y, fixed height, int *table, int *index, int depth)
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
    if (depth == 0)
        mark_line(sx, sy, ex, ey, base_y, height, table, index);
    else {
        depth--;
        mark_curve(sx, sy, ax, ay, dx, dy, gx, gy, base_y, height, table, index, depth);
        mark_curve(gx, gy, fx, fy, cx, cy, ex, ey, base_y, height, table, index, depth);
    }
}