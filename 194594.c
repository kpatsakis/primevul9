static bool qxl_rom_monitors_config_changed(QXLRom *rom,
        VDAgentMonitorsConfig *monitors_config,
        unsigned int max_outputs)
{
    int i;
    unsigned int monitors_count;

    monitors_count = MIN(monitors_config->num_of_monitors, max_outputs);

    if (rom->client_monitors_config.count != monitors_count) {
        return true;
    }

    for (i = 0 ; i < rom->client_monitors_config.count ; ++i) {
        VDAgentMonConfig *monitor = &monitors_config->monitors[i];
        QXLURect *rect = &rom->client_monitors_config.heads[i];
        /* monitor->depth ignored */
        if ((rect->left != monitor->x) ||
            (rect->top != monitor->y)  ||
            (rect->right != monitor->x + monitor->width) ||
            (rect->bottom != monitor->y + monitor->height)) {
            return true;
        }
    }

    return false;
}