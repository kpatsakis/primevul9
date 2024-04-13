GF_Err gf_odf_ac3_config_parse_bs(GF_BitStream *bs, Bool is_ec3, GF_AC3Config *cfg)
{
	if (!cfg || !bs) return GF_BAD_PARAM;
	memset(cfg, 0, sizeof(GF_AC3Config));
	cfg->is_ec3 = is_ec3;
	if (is_ec3) {
		u32 j;
		cfg->is_ec3 = 1;
		cfg->brcode = gf_bs_read_int(bs, 13);
		cfg->nb_streams = 1 + gf_bs_read_int(bs, 3);
		for (j=0; j<cfg->nb_streams; j++) {
			cfg->streams[j].fscod = gf_bs_read_int(bs, 2);
			cfg->streams[j].bsid = gf_bs_read_int(bs, 5);
			gf_bs_read_int(bs, 1);
			cfg->streams[j].asvc = gf_bs_read_int(bs, 1);
			cfg->streams[j].bsmod = gf_bs_read_int(bs, 3);
			cfg->streams[j].acmod = gf_bs_read_int(bs, 3);
			cfg->streams[j].lfon = gf_bs_read_int(bs, 1);
			gf_bs_read_int(bs, 3);
			cfg->streams[j].nb_dep_sub = gf_bs_read_int(bs, 4);
			if (cfg->streams[j].nb_dep_sub) {
				cfg->streams[j].chan_loc = gf_bs_read_int(bs, 9);
			} else {
				gf_bs_read_int(bs, 1);
			}
		}
	} else {
		cfg->nb_streams = 1;
		cfg->streams[0].fscod = gf_bs_read_int(bs, 2);
		cfg->streams[0].bsid = gf_bs_read_int(bs, 5);
		cfg->streams[0].bsmod = gf_bs_read_int(bs, 3);
		cfg->streams[0].acmod = gf_bs_read_int(bs, 3);
		cfg->streams[0].lfon = gf_bs_read_int(bs, 1);
		cfg->brcode = gf_bs_read_int(bs, 5);
		gf_bs_read_int(bs, 5);
	}
	return GF_OK;
}