GF_Err gf_odf_ac3_cfg_write_bs(GF_AC3Config *cfg, GF_BitStream *bs)
{
	if (!cfg || !bs) return GF_BAD_PARAM;

	if (cfg->is_ec3) {
		u32 i;
		gf_bs_write_int(bs, cfg->brcode, 13);
		gf_bs_write_int(bs, cfg->nb_streams - 1, 3);
		for (i=0; i<cfg->nb_streams; i++) {
			gf_bs_write_int(bs, cfg->streams[i].fscod, 2);
			gf_bs_write_int(bs, cfg->streams[i].bsid, 5);
			gf_bs_write_int(bs, cfg->streams[i].bsmod, 5);
			gf_bs_write_int(bs, cfg->streams[i].acmod, 3);
			gf_bs_write_int(bs, cfg->streams[i].lfon, 1);
			gf_bs_write_int(bs, 0, 3);
			gf_bs_write_int(bs, cfg->streams[i].nb_dep_sub, 4);
			if (cfg->streams[i].nb_dep_sub) {
				gf_bs_write_int(bs, cfg->streams[i].chan_loc, 9);
			} else {
				gf_bs_write_int(bs, 0, 1);
			}
		}
	} else {
		gf_bs_write_int(bs, cfg->streams[0].fscod, 2);
		gf_bs_write_int(bs, cfg->streams[0].bsid, 5);
		gf_bs_write_int(bs, cfg->streams[0].bsmod, 3);
		gf_bs_write_int(bs, cfg->streams[0].acmod, 3);
		gf_bs_write_int(bs, cfg->streams[0].lfon, 1);
		gf_bs_write_int(bs, cfg->brcode, 5);
		gf_bs_write_int(bs, 0, 5);
	}
	return GF_OK;
}