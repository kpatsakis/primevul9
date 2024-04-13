int vnc_display_pw_expire(const char *id, time_t expires)
{
    VncDisplay *vd = vnc_display_find(id);

    if (!vd) {
        return -EINVAL;
    }

    vd->expires = expires;
    return 0;
}