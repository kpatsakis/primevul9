vmxnet3_is_registered_vlan(VMXNET3State *s, const void *data)
{
    uint16_t vlan_tag = eth_get_pkt_tci(data) & VLAN_VID_MASK;
    if (IS_SPECIAL_VLAN_ID(vlan_tag)) {
        return true;
    }

    return VMXNET3_VFTABLE_ENTRY_IS_SET(s->vlan_table, vlan_tag);
}