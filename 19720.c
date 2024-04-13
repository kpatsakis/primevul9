cdf_swap_header(cdf_header_t *h)
{
	size_t i;

	h->h_magic = CDF_TOLE8(h->h_magic);
	h->h_uuid[0] = CDF_TOLE8(h->h_uuid[0]);
	h->h_uuid[1] = CDF_TOLE8(h->h_uuid[1]);
	h->h_revision = CDF_TOLE2(h->h_revision);
	h->h_version = CDF_TOLE2(h->h_version);
	h->h_byte_order = CDF_TOLE2(h->h_byte_order);
	h->h_sec_size_p2 = CDF_TOLE2(h->h_sec_size_p2);
	h->h_short_sec_size_p2 = CDF_TOLE2(h->h_short_sec_size_p2);
	h->h_num_sectors_in_sat = CDF_TOLE4(h->h_num_sectors_in_sat);
	h->h_secid_first_directory = CDF_TOLE4(h->h_secid_first_directory);
	h->h_min_size_standard_stream =
	    CDF_TOLE4(h->h_min_size_standard_stream);
	h->h_secid_first_sector_in_short_sat =
	    CDF_TOLE4(CAST(uint32_t, h->h_secid_first_sector_in_short_sat));
	h->h_num_sectors_in_short_sat =
	    CDF_TOLE4(h->h_num_sectors_in_short_sat);
	h->h_secid_first_sector_in_master_sat =
	    CDF_TOLE4(CAST(uint32_t, h->h_secid_first_sector_in_master_sat));
	h->h_num_sectors_in_master_sat =
	    CDF_TOLE4(h->h_num_sectors_in_master_sat);
	for (i = 0; i < __arraycount(h->h_master_sat); i++) {
		h->h_master_sat[i] =
		    CDF_TOLE4(CAST(uint32_t, h->h_master_sat[i]));
	}
}