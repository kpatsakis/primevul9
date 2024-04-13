static void i40e_stat_update48(struct i40e_hw *hw, u32 hireg, u32 loreg,
			       bool offset_loaded, u64 *offset, u64 *stat)
{
	u64 new_data;

	if (hw->device_id == I40E_DEV_ID_QEMU) {
		new_data = rd32(hw, loreg);
		new_data |= ((u64)(rd32(hw, hireg) & 0xFFFF)) << 32;
	} else {
		new_data = rd64(hw, loreg);
	}
	if (!offset_loaded)
		*offset = new_data;
	if (likely(new_data >= *offset))
		*stat = new_data - *offset;
	else
		*stat = (new_data + BIT_ULL(48)) - *offset;
	*stat &= 0xFFFFFFFFFFFFULL;
}