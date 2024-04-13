int vnc_init_func(void *opaque, QemuOpts *opts, Error **errp)
{
    Error *local_err = NULL;
    char *id = (char *)qemu_opts_id(opts);

    assert(id);
    vnc_display_init(id);
    vnc_display_open(id, &local_err);
    if (local_err != NULL) {
        error_reportf_err(local_err, "Failed to start VNC server: ");
        exit(1);
    }
    return 0;
}