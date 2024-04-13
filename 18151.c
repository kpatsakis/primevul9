update_curbuf(int type)
{
    redraw_curbuf_later(type);
    update_screen(type);
}