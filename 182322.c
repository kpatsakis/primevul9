vmxnet3_is_allowed_mcast_group(VMXNET3State *s, const uint8_t *group_mac)
{
    int i;
    for (i = 0; i < s->mcast_list_len; i++) {
        if (!memcmp(group_mac, s->mcast_list[i].a, sizeof(s->mcast_list[i]))) {
            return true;
        }
    }
    return false;
}