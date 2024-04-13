static void gfs2_rindex_print(const struct gfs2_rgrpd *rgd)
{
	pr_info("ri_addr = %llu\n", (unsigned long long)rgd->rd_addr);
	pr_info("ri_length = %u\n", rgd->rd_length);
	pr_info("ri_data0 = %llu\n", (unsigned long long)rgd->rd_data0);
	pr_info("ri_data = %u\n", rgd->rd_data);
	pr_info("ri_bitbytes = %u\n", rgd->rd_bitbytes);
}