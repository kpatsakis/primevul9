compute_dir(const fill_options *fo, fixed y0, fixed y1)
{
    if (max(y0, y1) < fo->ymin)
        return DIR_OUT_OF_Y_RANGE;
    if (min(y0, y1) > fo->ymax)
        return DIR_OUT_OF_Y_RANGE;
    return (y0 < y1 ? DIR_UP :
            y0 > y1 ? DIR_DOWN : DIR_HORIZONTAL);
}