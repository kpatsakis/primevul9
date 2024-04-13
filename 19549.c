static void iwl_trans_pcie_write8(struct iwl_trans *trans, u32 ofs, u8 val)
{
	writeb(val, IWL_TRANS_GET_PCIE_TRANS(trans)->hw_base + ofs);
}