static int make_bbox(gx_path       * path,
               const gs_fixed_rect * clip,
                     gs_fixed_rect * ibox,
                     fixed           adjust)
{
    gs_fixed_rect bbox;
    int           code;

    /* Find the bbox - fixed */
    code = gx_path_bbox(path, &bbox);
    if (code < 0)
        return code;

    if (clip) {
        if (bbox.p.y < clip->p.y)
            bbox.p.y = clip->p.y;
        if (bbox.q.y > clip->q.y)
            bbox.q.y = clip->q.y;
    }

    /* Convert to bbox - int */
    ibox->p.x = fixed2int(bbox.p.x-adjust);
    ibox->p.y = fixed2int(bbox.p.y-adjust);
    ibox->q.x = fixed2int(bbox.q.x-adjust+fixed_1);
    ibox->q.y = fixed2int(bbox.q.y-adjust+fixed_1);

    return 0;
}