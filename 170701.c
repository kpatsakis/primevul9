rescale_input_color(gs_range range, float input)
{
    return((input-range.rmin)/(range.rmax-range.rmin));
}