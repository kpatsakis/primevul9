static u32 hclge_tm_get_shapping_para(u8 ir_b, u8 ir_u, u8 ir_s,
				      u8 bs_b, u8 bs_s)
{
	u32 shapping_para = 0;

	hclge_tm_set_field(shapping_para, IR_B, ir_b);
	hclge_tm_set_field(shapping_para, IR_U, ir_u);
	hclge_tm_set_field(shapping_para, IR_S, ir_s);
	hclge_tm_set_field(shapping_para, BS_B, bs_b);
	hclge_tm_set_field(shapping_para, BS_S, bs_s);

	return shapping_para;
}