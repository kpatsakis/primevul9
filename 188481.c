static IP_ADAPTER_INFO *guest_get_adapters_info(void)
{
    IP_ADAPTER_INFO *adptr_info = NULL;
    ULONG adptr_info_len = 0;
    DWORD ret;

    /* Call the first time to get the adptr_info_len. */
    GetAdaptersInfo(adptr_info, &adptr_info_len);

    adptr_info = g_malloc(adptr_info_len);
    ret = GetAdaptersInfo(adptr_info, &adptr_info_len);
    if (ret != ERROR_SUCCESS) {
        g_free(adptr_info);
        adptr_info = NULL;
    }
    return adptr_info;
}