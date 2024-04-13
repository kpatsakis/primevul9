GuestOSInfo *qmp_guest_get_osinfo(Error **errp)
{
    Error *local_err = NULL;
    OSVERSIONINFOEXW os_version = {0};
    bool server;
    char *product_name;
    GuestOSInfo *info;

    ga_get_win_version(&os_version, &local_err);
    if (local_err) {
        error_propagate(errp, local_err);
        return NULL;
    }

    server = os_version.wProductType != VER_NT_WORKSTATION;
    product_name = ga_get_win_product_name(&local_err);
    if (product_name == NULL) {
        error_propagate(errp, local_err);
        return NULL;
    }

    info = g_new0(GuestOSInfo, 1);

    info->has_kernel_version = true;
    info->kernel_version = g_strdup_printf("%lu.%lu",
        os_version.dwMajorVersion,
        os_version.dwMinorVersion);
    info->has_kernel_release = true;
    info->kernel_release = g_strdup_printf("%lu",
        os_version.dwBuildNumber);
    info->has_machine = true;
    info->machine = ga_get_current_arch();

    info->has_id = true;
    info->id = g_strdup("mswindows");
    info->has_name = true;
    info->name = g_strdup("Microsoft Windows");
    info->has_pretty_name = true;
    info->pretty_name = product_name;
    info->has_version = true;
    info->version = ga_get_win_name(&os_version, false);
    info->has_version_id = true;
    info->version_id = ga_get_win_name(&os_version, true);
    info->has_variant = true;
    info->variant = g_strdup(server ? "server" : "client");
    info->has_variant_id = true;
    info->variant_id = g_strdup(server ? "server" : "client");

    return info;
}