static void vnc_set_share_mode(VncState *vs, VncShareMode mode)
{
#ifdef _VNC_DEBUG
    static const char *mn[] = {
        [0]                           = "undefined",
        [VNC_SHARE_MODE_CONNECTING]   = "connecting",
        [VNC_SHARE_MODE_SHARED]       = "shared",
        [VNC_SHARE_MODE_EXCLUSIVE]    = "exclusive",
        [VNC_SHARE_MODE_DISCONNECTED] = "disconnected",
    };
    fprintf(stderr, "%s/%p: %s -> %s\n", __func__,
            vs->ioc, mn[vs->share_mode], mn[mode]);
#endif

    switch (vs->share_mode) {
    case VNC_SHARE_MODE_CONNECTING:
        vs->vd->num_connecting--;
        break;
    case VNC_SHARE_MODE_SHARED:
        vs->vd->num_shared--;
        break;
    case VNC_SHARE_MODE_EXCLUSIVE:
        vs->vd->num_exclusive--;
        break;
    default:
        break;
    }

    vs->share_mode = mode;

    switch (vs->share_mode) {
    case VNC_SHARE_MODE_CONNECTING:
        vs->vd->num_connecting++;
        break;
    case VNC_SHARE_MODE_SHARED:
        vs->vd->num_shared++;
        break;
    case VNC_SHARE_MODE_EXCLUSIVE:
        vs->vd->num_exclusive++;
        break;
    default:
        break;
    }
}