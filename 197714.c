pdf_text_distance_transform(double wx, double wy, const pdf_text_state_t *pts,
                            gs_point *ppt)
{
    return gs_distance_transform(wx, wy, &pts->in.matrix, ppt);
}