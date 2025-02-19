static void i40e_get_oem_version(struct i40e_hw *hw)
{
	u16 block_offset = 0xffff;
	u16 block_length = 0;
	u16 capabilities = 0;
	u16 gen_snap = 0;
	u16 release = 0;

#define I40E_SR_NVM_OEM_VERSION_PTR		0x1B
#define I40E_NVM_OEM_LENGTH_OFFSET		0x00
#define I40E_NVM_OEM_CAPABILITIES_OFFSET	0x01
#define I40E_NVM_OEM_GEN_OFFSET			0x02
#define I40E_NVM_OEM_RELEASE_OFFSET		0x03
#define I40E_NVM_OEM_CAPABILITIES_MASK		0x000F
#define I40E_NVM_OEM_LENGTH			3

	/* Check if pointer to OEM version block is valid. */
	i40e_read_nvm_word(hw, I40E_SR_NVM_OEM_VERSION_PTR, &block_offset);
	if (block_offset == 0xffff)
		return;

	/* Check if OEM version block has correct length. */
	i40e_read_nvm_word(hw, block_offset + I40E_NVM_OEM_LENGTH_OFFSET,
			   &block_length);
	if (block_length < I40E_NVM_OEM_LENGTH)
		return;

	/* Check if OEM version format is as expected. */
	i40e_read_nvm_word(hw, block_offset + I40E_NVM_OEM_CAPABILITIES_OFFSET,
			   &capabilities);
	if ((capabilities & I40E_NVM_OEM_CAPABILITIES_MASK) != 0)
		return;

	i40e_read_nvm_word(hw, block_offset + I40E_NVM_OEM_GEN_OFFSET,
			   &gen_snap);
	i40e_read_nvm_word(hw, block_offset + I40E_NVM_OEM_RELEASE_OFFSET,
			   &release);
	hw->nvm.oem_ver = (gen_snap << I40E_OEM_SNAP_SHIFT) | release;
	hw->nvm.eetrack = I40E_OEM_EETRACK_ID;
}