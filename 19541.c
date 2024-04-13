static int iwl_pcie_load_section(struct iwl_trans *trans, u8 section_num,
			    const struct fw_desc *section)
{
	u8 *v_addr;
	dma_addr_t p_addr;
	u32 offset, chunk_sz = min_t(u32, FH_MEM_TB_MAX_LENGTH, section->len);
	int ret = 0;

	IWL_DEBUG_FW(trans, "[%d] uCode section being loaded...\n",
		     section_num);

	v_addr = dma_alloc_coherent(trans->dev, chunk_sz, &p_addr,
				    GFP_KERNEL | __GFP_NOWARN);
	if (!v_addr) {
		IWL_DEBUG_INFO(trans, "Falling back to small chunks of DMA\n");
		chunk_sz = PAGE_SIZE;
		v_addr = dma_alloc_coherent(trans->dev, chunk_sz,
					    &p_addr, GFP_KERNEL);
		if (!v_addr)
			return -ENOMEM;
	}

	for (offset = 0; offset < section->len; offset += chunk_sz) {
		u32 copy_size, dst_addr;
		bool extended_addr = false;

		copy_size = min_t(u32, chunk_sz, section->len - offset);
		dst_addr = section->offset + offset;

		if (dst_addr >= IWL_FW_MEM_EXTENDED_START &&
		    dst_addr <= IWL_FW_MEM_EXTENDED_END)
			extended_addr = true;

		if (extended_addr)
			iwl_set_bits_prph(trans, LMPM_CHICK,
					  LMPM_CHICK_EXTENDED_ADDR_SPACE);

		memcpy(v_addr, (u8 *)section->data + offset, copy_size);
		ret = iwl_pcie_load_firmware_chunk(trans, dst_addr, p_addr,
						   copy_size);

		if (extended_addr)
			iwl_clear_bits_prph(trans, LMPM_CHICK,
					    LMPM_CHICK_EXTENDED_ADDR_SPACE);

		if (ret) {
			IWL_ERR(trans,
				"Could not load the [%d] uCode section\n",
				section_num);
			break;
		}
	}

	dma_free_coherent(trans->dev, chunk_sz, v_addr, p_addr);
	return ret;
}