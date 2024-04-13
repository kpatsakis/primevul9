static void qxl_dirty_surfaces(PCIQXLDevice *qxl)
{
    int i;

    if (qxl->mode != QXL_MODE_NATIVE && qxl->mode != QXL_MODE_COMPAT) {
        return;
    }

    /* dirty the primary surface */
    qxl_dirty_one_surface(qxl, qxl->guest_primary.surface.mem,
                          qxl->guest_primary.surface.height,
                          qxl->guest_primary.surface.stride);

    /* dirty the off-screen surfaces */
    for (i = 0; i < qxl->ssd.num_surfaces; i++) {
        QXLSurfaceCmd *cmd;

        if (qxl->guest_surfaces.cmds[i] == 0) {
            continue;
        }

        cmd = qxl_phys2virt(qxl, qxl->guest_surfaces.cmds[i],
                            MEMSLOT_GROUP_GUEST);
        assert(cmd);
        assert(cmd->type == QXL_SURFACE_CMD_CREATE);
        qxl_dirty_one_surface(qxl, cmd->u.surface_create.data,
                              cmd->u.surface_create.height,
                              cmd->u.surface_create.stride);
    }
}