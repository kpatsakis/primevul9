static int64_t guest_ip_prefix(IP_ADAPTER_UNICAST_ADDRESS *ip_addr)
{
    /* For Windows Vista/2008 and newer, use the OnLinkPrefixLength
     * field to obtain the prefix.
     */
    return ip_addr->OnLinkPrefixLength;
}