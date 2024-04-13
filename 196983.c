static ENUM_PTRS_BEGIN(pattern2_instance_enum_ptrs) {
    if (index < st_pattern2_template_max_ptrs) {
        gs_ptr_type_t ptype =
            ENUM_SUPER_ELT(gs_pattern2_instance_t, st_pattern2_template,
                           templat, 0);

        if (ptype)
            return ptype;
        return ENUM_OBJ(NULL);  /* don't stop early */
    }
    ENUM_PREFIX(st_pattern_instance, st_pattern2_template_max_ptrs);
}