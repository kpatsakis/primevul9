int pdf_modify_text_render_mode(pdf_text_state_t *pts, int render_mode)
{
    switch (pts->in.render_mode) {
        case 0:
            if (render_mode == 1) {
                pts->in.render_mode = 2;
                return(1);
            }
            break;
        case 1:
            if (render_mode == 1)
                return(1);
            break;
        case 2:
            if (render_mode == 1)
                return(1);
            break;
        case 3:
            if (render_mode == 1) {
                pts->in.render_mode = 1;
                return(1);
            }
            break;
        case 4:
            if (render_mode == 1) {
                pts->in.render_mode = 6;
                return(1);
            }
            break;
        case 5:
            if (render_mode == 1)
                return(1);
            break;
        case 6:
            if (render_mode == 1)
                return(1);
            break;
        case 7:
            if (render_mode == 1) {
                pts->in.render_mode = 5;
                return(1);
            }
            break;
        default:
            break;
    }
    return(0);
}