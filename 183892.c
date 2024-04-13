int vnc_display_password(const char *id, const char *password)
{
    VncDisplay *vd = vnc_display_find(id);

    if (!vd) {
        return -EINVAL;
    }
    if (vd->auth == VNC_AUTH_NONE) {
        error_printf_unless_qmp("If you want use passwords please enable "
                                "password auth using '-vnc ${dpy},password'.\n");
        return -EINVAL;
    }

    g_free(vd->password);
    vd->password = g_strdup(password);

    return 0;
}