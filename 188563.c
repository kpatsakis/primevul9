GuestOSInfo *qmp_guest_get_osinfo(Error **errp)
{
    GuestOSInfo *info = NULL;
    struct utsname kinfo;
    GKeyFile *osrelease = NULL;
    const char *qga_os_release = g_getenv("QGA_OS_RELEASE");

    info = g_new0(GuestOSInfo, 1);

    if (uname(&kinfo) != 0) {
        error_setg_errno(errp, errno, "uname failed");
    } else {
        info->has_kernel_version = true;
        info->kernel_version = g_strdup(kinfo.version);
        info->has_kernel_release = true;
        info->kernel_release = g_strdup(kinfo.release);
        info->has_machine = true;
        info->machine = g_strdup(kinfo.machine);
    }

    if (qga_os_release != NULL) {
        osrelease = ga_parse_osrelease(qga_os_release);
    } else {
        osrelease = ga_parse_osrelease("/etc/os-release");
        if (osrelease == NULL) {
            osrelease = ga_parse_osrelease("/usr/lib/os-release");
        }
    }

    if (osrelease != NULL) {
        char *value;

#define GET_FIELD(field, osfield) do { \
    value = g_key_file_get_value(osrelease, "os-release", osfield, NULL); \
    if (value != NULL) { \
        ga_osrelease_replace_special(value); \
        info->has_ ## field = true; \
        info->field = value; \
    } \
} while (0)
        GET_FIELD(id, "ID");
        GET_FIELD(name, "NAME");
        GET_FIELD(pretty_name, "PRETTY_NAME");
        GET_FIELD(version, "VERSION");
        GET_FIELD(version_id, "VERSION_ID");
        GET_FIELD(variant, "VARIANT");
        GET_FIELD(variant_id, "VARIANT_ID");
#undef GET_FIELD

        g_key_file_free(osrelease);
    }

    return info;
}