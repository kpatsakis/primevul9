static IP_ADAPTER_ADDRESSES *guest_get_adapters_addresses(Error **errp)
{
    IP_ADAPTER_ADDRESSES *adptr_addrs = NULL;
    ULONG adptr_addrs_len = 0;
    DWORD ret;

    /* Call the first time to get the adptr_addrs_len. */
    GetAdaptersAddresses(AF_UNSPEC, GAA_FLAG_INCLUDE_PREFIX,
                         NULL, adptr_addrs, &adptr_addrs_len);

    adptr_addrs = g_malloc(adptr_addrs_len);
    ret = GetAdaptersAddresses(AF_UNSPEC, GAA_FLAG_INCLUDE_PREFIX,
                               NULL, adptr_addrs, &adptr_addrs_len);
    if (ret != ERROR_SUCCESS) {
        error_setg_win32(errp, ret, "failed to get adapters addresses");
        g_free(adptr_addrs);
        adptr_addrs = NULL;
    }
    return adptr_addrs;
}