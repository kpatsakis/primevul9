static void ga_get_win_version(RTL_OSVERSIONINFOEXW *info, Error **errp)
{
    typedef NTSTATUS(WINAPI * rtl_get_version_t)(
        RTL_OSVERSIONINFOEXW *os_version_info_ex);

    info->dwOSVersionInfoSize = sizeof(RTL_OSVERSIONINFOEXW);

    HMODULE module = GetModuleHandle("ntdll");
    PVOID fun = GetProcAddress(module, "RtlGetVersion");
    if (fun == NULL) {
        error_setg(errp, QERR_QGA_COMMAND_FAILED,
            "Failed to get address of RtlGetVersion");
        return;
    }

    rtl_get_version_t rtl_get_version = (rtl_get_version_t)fun;
    rtl_get_version(info);
    return;
}