RZ_API const char *rz_bin_dwarf_get_attr_name(ut64 attr_code) {
	if (attr_code < RZ_ARRAY_SIZE(dwarf_attr_encodings)) {
		return dwarf_attr_encodings[attr_code];
	}
	// the below codes are much sparser, so putting them in an array would require a lot of
	// unused memory
	switch (attr_code) {
	case DW_AT_lo_user:
		return "DW_AT_lo_user";
	case DW_AT_MIPS_linkage_name:
		return "DW_AT_MIPS_linkage_name";
	case DW_AT_GNU_call_site_value:
		return "DW_AT_GNU_call_site_value";
	case DW_AT_GNU_call_site_data_value:
		return "DW_AT_GNU_call_site_data_value";
	case DW_AT_GNU_call_site_target:
		return "DW_AT_GNU_call_site_target";
	case DW_AT_GNU_call_site_target_clobbered:
		return "DW_AT_GNU_call_site_target_clobbered";
	case DW_AT_GNU_tail_call:
		return "DW_AT_GNU_tail_call";
	case DW_AT_GNU_all_tail_call_sites:
		return "DW_AT_GNU_all_tail_call_sites";
	case DW_AT_GNU_all_call_sites:
		return "DW_AT_GNU_all_call_sites";
	case DW_AT_GNU_all_source_call_sites:
		return "DW_AT_GNU_all_source_call_sites";
	case DW_AT_GNU_macros:
		return "DW_AT_GNU_macros";
	case DW_AT_GNU_deleted:
		return "DW_AT_GNU_deleted";
	case DW_AT_GNU_dwo_name:
		return "DW_AT_GNU_dwo_name";
	case DW_AT_GNU_dwo_id:
		return "DW_AT_GNU_dwo_id";
	case DW_AT_GNU_ranges_base:
		return "DW_AT_GNU_ranges_base";
	case DW_AT_GNU_addr_base:
		return "DW_AT_GNU_addr_base";
	case DW_AT_GNU_pubnames:
		return "DW_AT_GNU_pubnames";
	case DW_AT_GNU_pubtypes:
		return "DW_AT_GNU_pubtypes";
	case DW_AT_hi_user:
		return "DW_AT_hi_user";
	default:
		return NULL;
	}
}