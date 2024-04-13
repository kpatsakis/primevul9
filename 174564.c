lips_media_selection(int width, int height)
{
    int landscape = 0;
    int tmp;
    paper_table *pt;

    if (width > height) {
        landscape = 1;
        tmp = width;
        width = height;
        height = tmp;
    }
    for (pt = lips_paper_table; pt->num_unit < 80; pt++)
        if (pt->width == width && pt->height == height)
            break;

    return pt->num_unit + landscape;
}