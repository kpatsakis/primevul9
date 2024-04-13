static void i40e_stat_update_and_clear32(struct i40e_hw *hw, u32 reg, u64 *stat)
{
	u32 new_data = rd32(hw, reg);

	wr32(hw, reg, 1); /* must write a nonzero value to clear register */
	*stat += new_data;
}