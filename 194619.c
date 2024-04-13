static int qxl_track_command(PCIQXLDevice *qxl, struct QXLCommandExt *ext)
{
    switch (le32_to_cpu(ext->cmd.type)) {
    case QXL_CMD_SURFACE:
    {
        QXLSurfaceCmd *cmd = qxl_phys2virt(qxl, ext->cmd.data, ext->group_id);

        if (!cmd) {
            return 1;
        }
        uint32_t id = le32_to_cpu(cmd->surface_id);

        if (id >= qxl->ssd.num_surfaces) {
            qxl_set_guest_bug(qxl, "QXL_CMD_SURFACE id %d >= %d", id,
                              qxl->ssd.num_surfaces);
            return 1;
        }
        if (cmd->type == QXL_SURFACE_CMD_CREATE &&
            (cmd->u.surface_create.stride & 0x03) != 0) {
            qxl_set_guest_bug(qxl, "QXL_CMD_SURFACE stride = %d %% 4 != 0\n",
                              cmd->u.surface_create.stride);
            return 1;
        }
        qemu_mutex_lock(&qxl->track_lock);
        if (cmd->type == QXL_SURFACE_CMD_CREATE) {
            qxl->guest_surfaces.cmds[id] = ext->cmd.data;
            qxl->guest_surfaces.count++;
            if (qxl->guest_surfaces.max < qxl->guest_surfaces.count)
                qxl->guest_surfaces.max = qxl->guest_surfaces.count;
        }
        if (cmd->type == QXL_SURFACE_CMD_DESTROY) {
            qxl->guest_surfaces.cmds[id] = 0;
            qxl->guest_surfaces.count--;
        }
        qemu_mutex_unlock(&qxl->track_lock);
        break;
    }
    case QXL_CMD_CURSOR:
    {
        QXLCursorCmd *cmd = qxl_phys2virt(qxl, ext->cmd.data, ext->group_id);

        if (!cmd) {
            return 1;
        }
        if (cmd->type == QXL_CURSOR_SET) {
            qemu_mutex_lock(&qxl->track_lock);
            qxl->guest_cursor = ext->cmd.data;
            qemu_mutex_unlock(&qxl->track_lock);
        }
        if (cmd->type == QXL_CURSOR_HIDE) {
            qemu_mutex_lock(&qxl->track_lock);
            qxl->guest_cursor = 0;
            qemu_mutex_unlock(&qxl->track_lock);
        }
        break;
    }
    }
    return 0;
}