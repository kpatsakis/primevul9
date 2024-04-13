GF_Err gf_odf_dovi_cfg_write_bs(GF_DOVIDecoderConfigurationRecord *cfg, GF_BitStream *bs)
{
	gf_bs_write_u8(bs,  cfg->dv_version_major);
	gf_bs_write_u8(bs,  cfg->dv_version_minor);
	gf_bs_write_int(bs, cfg->dv_profile, 7);
	gf_bs_write_int(bs, cfg->dv_level, 6);
	gf_bs_write_int(bs, cfg->rpu_present_flag, 1);
	gf_bs_write_int(bs, cfg->el_present_flag, 1);
	gf_bs_write_int(bs, cfg->bl_present_flag, 1);
    gf_bs_write_u32(bs, 0);
    gf_bs_write_u32(bs, 0);
    gf_bs_write_u32(bs, 0);
    gf_bs_write_u32(bs, 0);
    gf_bs_write_u32(bs, 0);
	return GF_OK;
}