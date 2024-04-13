activate_globus_gsi( void )
{
#if !defined(HAVE_EXT_GLOBUS)
	set_error_string( NOT_SUPPORTED_MSG );
	return -1;
#else
	static bool globus_gsi_activated = false;
	static bool activation_failed = false;

	if ( globus_gsi_activated ) {
		return 0;
	}
	if ( activation_failed ) {
		return -1;
	}

	if ( Condor_Auth_SSL::Initialize() == false ) {
		// Error in the dlopen/sym calls for libssl, return failure.
		std::string buf;
		formatstr( buf, "Failed to open SSL library" );
		set_error_string( buf.c_str() );
		activation_failed = true;
		return -1;
	}

#if defined(DLOPEN_GSI_LIBS)
	void *dl_hdl;

	if ( (dl_hdl = dlopen(LIBLTDL_SO, RTLD_LAZY)) == NULL ||
		 (dl_hdl = dlopen(LIBGLOBUS_COMMON_SO, RTLD_LAZY)) == NULL ||
		 !(globus_module_activate_ptr = (int (*)(globus_module_descriptor_t*))dlsym(dl_hdl, "globus_module_activate")) ||
		 !(globus_thread_set_model_ptr = (int (*)(const char*))dlsym(dl_hdl, "globus_thread_set_model")) ||
		 (dl_hdl = dlopen(LIBGLOBUS_CALLOUT_SO, RTLD_LAZY)) == NULL ||
		 (dl_hdl = dlopen(LIBGLOBUS_PROXY_SSL_SO, RTLD_LAZY)) == NULL ||
		 (dl_hdl = dlopen(LIBGLOBUS_OPENSSL_ERROR_SO, RTLD_LAZY)) == NULL ||
		 (dl_hdl = dlopen(LIBGLOBUS_OPENSSL_SO, RTLD_LAZY)) == NULL ||
		 (dl_hdl = dlopen(LIBGLOBUS_GSI_CERT_UTILS_SO, RTLD_LAZY)) == NULL ||
		 (dl_hdl = dlopen(LIBGLOBUS_GSI_SYSCONFIG_SO, RTLD_LAZY)) == NULL ||
		 !(globus_gsi_sysconfig_get_proxy_filename_unix_ptr = (globus_result_t (*)(char**, globus_gsi_proxy_file_type_t))dlsym(dl_hdl, "globus_gsi_sysconfig_get_proxy_filename_unix")) ||
		 (dl_hdl = dlopen(LIBGLOBUS_OLDGAA_SO, RTLD_LAZY)) == NULL ||
		 (dl_hdl = dlopen(LIBGLOBUS_GSI_CALLBACK_SO, RTLD_LAZY)) == NULL ||
		 (dl_hdl = dlopen(LIBGLOBUS_GSI_CREDENTIAL_SO, RTLD_LAZY))== NULL ||
		 !(globus_gsi_cred_get_cert_ptr = (globus_result_t (*)(globus_l_gsi_cred_handle_s*, X509**))dlsym(dl_hdl, "globus_gsi_cred_get_cert")) ||
		 !(globus_gsi_cred_get_cert_chain_ptr = (globus_result_t (*)(globus_gsi_cred_handle_t, STACK_OF(X509)**))dlsym(dl_hdl, "globus_gsi_cred_get_cert_chain")) ||
		 !(globus_gsi_cred_get_cert_type_ptr = (globus_result_t (*)(globus_l_gsi_cred_handle_s*, globus_gsi_cert_utils_cert_type_t*))dlsym(dl_hdl, "globus_gsi_cred_get_cert_type")) ||
		 !(globus_gsi_cred_get_identity_name_ptr = (globus_result_t (*)(globus_l_gsi_cred_handle_s*, char**))dlsym(dl_hdl, "globus_gsi_cred_get_identity_name")) ||
		 !(globus_gsi_cred_get_lifetime_ptr = (globus_result_t (*)(globus_l_gsi_cred_handle_s*, time_t*))dlsym(dl_hdl, "globus_gsi_cred_get_lifetime")) ||
		 !(globus_gsi_cred_get_subject_name_ptr = (globus_result_t (*)(globus_l_gsi_cred_handle_s*, char**))dlsym(dl_hdl, "globus_gsi_cred_get_subject_name")) ||
		 !(globus_gsi_cred_handle_attrs_destroy_ptr = (globus_result_t (*)(globus_l_gsi_cred_handle_attrs_s*))dlsym(dl_hdl, "globus_gsi_cred_handle_attrs_destroy")) ||
		 !(globus_gsi_cred_handle_attrs_init_ptr = (globus_result_t (*)(globus_l_gsi_cred_handle_attrs_s**))dlsym(dl_hdl, "globus_gsi_cred_handle_attrs_init")) ||
		 !(globus_gsi_cred_handle_destroy_ptr = (globus_result_t (*)(globus_l_gsi_cred_handle_s*))dlsym(dl_hdl, "globus_gsi_cred_handle_destroy")) ||
		 !(globus_gsi_cred_handle_init_ptr = (globus_result_t (*)(globus_l_gsi_cred_handle_s**, globus_l_gsi_cred_handle_attrs_s*))dlsym(dl_hdl, "globus_gsi_cred_handle_init")) ||
		 !(globus_gsi_cred_read_proxy_ptr = (globus_result_t (*)(globus_l_gsi_cred_handle_s*, const char*))dlsym(dl_hdl, "globus_gsi_cred_read_proxy")) ||
		 !(globus_gsi_cred_write_proxy_ptr = (globus_result_t (*)(globus_l_gsi_cred_handle_s*, char*))dlsym(dl_hdl, "globus_gsi_cred_write_proxy")) ||
		 (dl_hdl = dlopen(LIBGLOBUS_GSI_PROXY_CORE_SO, RTLD_LAZY)) == NULL ||
		 !(globus_gsi_proxy_assemble_cred_ptr = (globus_result_t (*)(globus_l_gsi_proxy_handle_s*, globus_l_gsi_cred_handle_s**, BIO*))dlsym(dl_hdl, "globus_gsi_proxy_assemble_cred")) ||
		 !(globus_gsi_proxy_create_req_ptr = (globus_result_t (*)(globus_l_gsi_proxy_handle_s*, BIO*))dlsym(dl_hdl, "globus_gsi_proxy_create_req")) ||
		 !(globus_gsi_proxy_handle_attrs_destroy_ptr = (globus_result_t (*)(globus_l_gsi_proxy_handle_attrs_s*))dlsym(dl_hdl, "globus_gsi_proxy_handle_attrs_destroy")) ||
		 !(globus_gsi_proxy_handle_attrs_get_keybits_ptr = (globus_result_t (*)(globus_l_gsi_proxy_handle_attrs_s*, int*))dlsym(dl_hdl, "globus_gsi_proxy_handle_attrs_get_keybits")) ||
		 !(globus_gsi_proxy_handle_attrs_init_ptr = (globus_result_t (*)(globus_l_gsi_proxy_handle_attrs_s**))dlsym(dl_hdl, "globus_gsi_proxy_handle_attrs_init")) ||
		 !(globus_gsi_proxy_handle_attrs_set_clock_skew_allowable_ptr = (globus_result_t (*)(globus_l_gsi_proxy_handle_attrs_s*, int))dlsym(dl_hdl, "globus_gsi_proxy_handle_attrs_set_clock_skew_allowable")) ||
		 !(globus_gsi_proxy_handle_attrs_set_keybits_ptr = (globus_result_t (*)(globus_l_gsi_proxy_handle_attrs_s*, int))dlsym(dl_hdl, "globus_gsi_proxy_handle_attrs_set_keybits")) ||
		 !(globus_gsi_proxy_handle_destroy_ptr = (globus_result_t (*)(globus_l_gsi_proxy_handle_s*))dlsym(dl_hdl, "globus_gsi_proxy_handle_destroy")) ||
		 !(globus_gsi_proxy_handle_init_ptr = (globus_result_t (*)(globus_l_gsi_proxy_handle_s**, globus_l_gsi_proxy_handle_attrs_s*))dlsym(dl_hdl, "globus_gsi_proxy_handle_init")) ||
		 !(globus_gsi_proxy_handle_set_is_limited_ptr = (globus_result_t (*)(globus_l_gsi_proxy_handle_s*, globus_bool_t))dlsym(dl_hdl, "globus_gsi_proxy_handle_set_is_limited")) ||
		 !(globus_gsi_proxy_handle_set_time_valid_ptr = (globus_result_t (*)(globus_l_gsi_proxy_handle_s*, int))dlsym(dl_hdl, "globus_gsi_proxy_handle_set_time_valid")) ||
		 !(globus_gsi_proxy_handle_set_type_ptr = (globus_result_t (*)(globus_l_gsi_proxy_handle_s*, globus_gsi_cert_utils_cert_type_t))dlsym(dl_hdl, "globus_gsi_proxy_handle_set_type")) ||
		 !(globus_gsi_proxy_inquire_req_ptr = (globus_result_t (*)(globus_l_gsi_proxy_handle_s*, BIO*))dlsym(dl_hdl, "globus_gsi_proxy_inquire_req")) ||
		 !(globus_gsi_proxy_sign_req_ptr = (globus_result_t (*)(globus_l_gsi_proxy_handle_s*, globus_l_gsi_cred_handle_s*, BIO*))dlsym(dl_hdl, "globus_gsi_proxy_sign_req")) ||
		 (dl_hdl = dlopen(LIBGLOBUS_GSSAPI_GSI_SO, RTLD_LAZY)) == NULL ||
		 !(gss_accept_sec_context_ptr = (OM_uint32 (*)(OM_uint32 *, gss_ctx_id_t *, const gss_cred_id_t, const gss_buffer_t, const gss_channel_bindings_t, gss_name_t *, gss_OID *, gss_buffer_t, OM_uint32 *, OM_uint32 *, gss_cred_id_t *))dlsym(dl_hdl, "gss_accept_sec_context")) ||
		 !(gss_compare_name_ptr = (OM_uint32 (*)(OM_uint32*, const gss_name_t, const gss_name_t, int*))dlsym(dl_hdl, "gss_compare_name")) ||
		 !(gss_context_time_ptr = (OM_uint32 (*)(OM_uint32*, const gss_ctx_id_t, OM_uint32*))dlsym(dl_hdl, "gss_context_time")) ||
		 !(gss_delete_sec_context_ptr = (OM_uint32 (*)(OM_uint32*, gss_ctx_id_t*, gss_buffer_t))dlsym(dl_hdl, "gss_delete_sec_context")) ||
		 !(gss_display_name_ptr = (OM_uint32 (*)( OM_uint32*, const gss_name_t, gss_buffer_t, gss_OID*))dlsym(dl_hdl, "gss_display_name")) ||
		 !(gss_import_cred_ptr = (OM_uint32 (*)(OM_uint32*, gss_cred_id_desc_struct**, gss_OID_desc_struct*, OM_uint32, gss_buffer_desc_struct*, OM_uint32, OM_uint32*))dlsym(dl_hdl, "gss_import_cred")) ||
		 !(gss_import_name_ptr = (OM_uint32 (*)(OM_uint32*, const gss_buffer_t, const gss_OID, gss_name_t*))dlsym(dl_hdl, "gss_import_name")) ||
		 !(gss_inquire_context_ptr = (OM_uint32 (*)(OM_uint32*, const gss_ctx_id_t, gss_name_t*, gss_name_t*, OM_uint32*, gss_OID*, OM_uint32*, int*, int*))dlsym(dl_hdl, "gss_inquire_context")) ||
		 !(gss_release_buffer_ptr = (OM_uint32 (*)(OM_uint32*, gss_buffer_t))dlsym(dl_hdl, "gss_release_buffer")) ||
		 !(gss_release_cred_ptr = (OM_uint32 (*)(OM_uint32*, gss_cred_id_desc_struct**))dlsym(dl_hdl, "gss_release_cred")) ||
		 !(gss_release_name_ptr = (OM_uint32 (*)(OM_uint32*, gss_name_t*))dlsym(dl_hdl, "gss_release_name")) ||
		 !(gss_unwrap_ptr = (OM_uint32 (*)(OM_uint32*, const gss_ctx_id_t, const gss_buffer_t, gss_buffer_t, int*, gss_qop_t*))dlsym(dl_hdl, "gss_unwrap")) ||
		 !(gss_wrap_ptr = (OM_uint32 (*)(OM_uint32*, const gss_ctx_id_t, int, gss_qop_t, const gss_buffer_t, int*, gss_buffer_t))dlsym(dl_hdl, "gss_wrap")) ||
		 !(gss_nt_host_ip_ptr = (gss_OID_desc **)dlsym(dl_hdl, "gss_nt_host_ip")) ||
		 (dl_hdl = dlopen(LIBGLOBUS_GSS_ASSIST_SO, RTLD_LAZY)) == NULL ||
		 !(globus_gss_assist_display_status_str_ptr = (OM_uint32 (*)(char**, char*, OM_uint32, OM_uint32, int))dlsym(dl_hdl, "globus_gss_assist_display_status_str")) ||
		 !(globus_gss_assist_map_and_authorize_ptr = (globus_result_t (*)(gss_ctx_id_t, char*, char*, char*, unsigned int))dlsym(dl_hdl, "globus_gss_assist_map_and_authorize")) ||
		 !(globus_gss_assist_acquire_cred_ptr = (OM_uint32 (*)(OM_uint32*, gss_cred_usage_t, gss_cred_id_t*))dlsym(dl_hdl, "globus_gss_assist_acquire_cred")) ||
		 !(globus_gss_assist_init_sec_context_ptr = (OM_uint32 (*)(OM_uint32*, const gss_cred_id_t, gss_ctx_id_t*, char*, OM_uint32, OM_uint32*, int*, int (*)(void*, void**, size_t*), void*, int (*)(void*, void*, size_t), void*))dlsym(dl_hdl, "globus_gss_assist_init_sec_context")) ||
		 !(globus_i_gsi_gss_assist_module_ptr = (globus_module_descriptor_t*)dlsym(dl_hdl, "globus_i_gsi_gss_assist_module")) ||
#if defined(HAVE_EXT_VOMS)
		 (dl_hdl = dlopen(LIBVOMSAPI_SO, RTLD_LAZY)) == NULL ||
		 !(VOMS_Destroy_ptr = (void (*)(vomsdata*))dlsym(dl_hdl, "VOMS_Destroy")) ||
		 !(VOMS_ErrorMessage_ptr = (char* (*)(vomsdata*, int, char*, int))dlsym(dl_hdl, "VOMS_ErrorMessage")) ||
		 !(VOMS_Init_ptr = (vomsdata* (*)(char*, char*))dlsym(dl_hdl, "VOMS_Init")) ||
		 !(VOMS_Retrieve_ptr = (int (*)(X509*, STACK_OF(X509)*, int, struct vomsdata*, int*))dlsym(dl_hdl, "VOMS_Retrieve")) ||
		 !(VOMS_SetVerificationType_ptr = (int (*)(int, vomsdata*, int*))dlsym(dl_hdl, "VOMS_SetVerificationType"))
#else
		 false
#endif
		 ) {
			 // Error in the dlopen/sym calls, return failure.
		const char *err = dlerror();
		std::string buf;
		formatstr( buf, "Failed to open GSI libraries: %s", err ? err : "Unknown error" );
		set_error_string( buf.c_str() );
		activation_failed = true;
		return -1;
	}
#else
	globus_module_activate_ptr = globus_module_activate;
	globus_thread_set_model_ptr = globus_thread_set_model;
	globus_gsi_sysconfig_get_proxy_filename_unix_ptr = globus_gsi_sysconfig_get_proxy_filename_unix;
	globus_gsi_cred_get_cert_ptr = globus_gsi_cred_get_cert;
	globus_gsi_cred_get_cert_chain_ptr = globus_gsi_cred_get_cert_chain;
	globus_gsi_cred_get_cert_type_ptr = globus_gsi_cred_get_cert_type;
	globus_gsi_cred_get_identity_name_ptr = globus_gsi_cred_get_identity_name;
	globus_gsi_cred_get_lifetime_ptr = globus_gsi_cred_get_lifetime;
	globus_gsi_cred_get_subject_name_ptr = globus_gsi_cred_get_subject_name;
	globus_gsi_cred_handle_attrs_destroy_ptr = globus_gsi_cred_handle_attrs_destroy;
	globus_gsi_cred_handle_attrs_init_ptr = globus_gsi_cred_handle_attrs_init;
	globus_gsi_cred_handle_destroy_ptr = globus_gsi_cred_handle_destroy;
	globus_gsi_cred_handle_init_ptr = globus_gsi_cred_handle_init;
	globus_gsi_cred_read_proxy_ptr = globus_gsi_cred_read_proxy;
	globus_gsi_cred_write_proxy_ptr = reinterpret_cast<globus_result_t (*)(globus_l_gsi_cred_handle_s*, char*)>(globus_gsi_cred_write_proxy);
	globus_gsi_proxy_assemble_cred_ptr = globus_gsi_proxy_assemble_cred;
	globus_gsi_proxy_create_req_ptr = globus_gsi_proxy_create_req;
	globus_gsi_proxy_handle_attrs_destroy_ptr = globus_gsi_proxy_handle_attrs_destroy;
	globus_gsi_proxy_handle_attrs_get_keybits_ptr = globus_gsi_proxy_handle_attrs_get_keybits;
	globus_gsi_proxy_handle_attrs_init_ptr = globus_gsi_proxy_handle_attrs_init;
	globus_gsi_proxy_handle_attrs_set_clock_skew_allowable_ptr = globus_gsi_proxy_handle_attrs_set_clock_skew_allowable;
	globus_gsi_proxy_handle_attrs_set_keybits_ptr = globus_gsi_proxy_handle_attrs_set_keybits;
	globus_gsi_proxy_handle_destroy_ptr = globus_gsi_proxy_handle_destroy;
	globus_gsi_proxy_handle_init_ptr = globus_gsi_proxy_handle_init;
	globus_gsi_proxy_handle_set_is_limited_ptr = globus_gsi_proxy_handle_set_is_limited;
	globus_gsi_proxy_handle_set_time_valid_ptr = globus_gsi_proxy_handle_set_time_valid;
	globus_gsi_proxy_handle_set_type_ptr = globus_gsi_proxy_handle_set_type;
	globus_gsi_proxy_inquire_req_ptr = globus_gsi_proxy_inquire_req;
	globus_gsi_proxy_sign_req_ptr = globus_gsi_proxy_sign_req;
	gss_accept_sec_context_ptr = gss_accept_sec_context;
	gss_compare_name_ptr = gss_compare_name;
	gss_context_time_ptr = gss_context_time;
	gss_delete_sec_context_ptr = gss_delete_sec_context;
	gss_display_name_ptr = gss_display_name;
	gss_import_cred_ptr = gss_import_cred;
	gss_import_name_ptr = gss_import_name;
	gss_inquire_context_ptr = gss_inquire_context;
	gss_release_buffer_ptr = gss_release_buffer;
	gss_release_cred_ptr = gss_release_cred;
	gss_release_name_ptr = gss_release_name;
	gss_unwrap_ptr = gss_unwrap;
	gss_wrap_ptr = gss_wrap;
	gss_nt_host_ip_ptr = &gss_nt_host_ip;
	globus_gss_assist_display_status_str_ptr = globus_gss_assist_display_status_str;
	globus_gss_assist_map_and_authorize_ptr = globus_gss_assist_map_and_authorize;
	globus_gss_assist_acquire_cred_ptr = globus_gss_assist_acquire_cred;
	globus_gss_assist_init_sec_context_ptr = globus_gss_assist_init_sec_context;
	globus_i_gsi_gss_assist_module_ptr = &globus_i_gsi_gss_assist_module;
#if defined(HAVE_EXT_VOMS)
	VOMS_Destroy_ptr = VOMS_Destroy;
	VOMS_ErrorMessage_ptr = VOMS_ErrorMessage;
	VOMS_Init_ptr = VOMS_Init;
	VOMS_Retrieve_ptr = VOMS_Retrieve;
	VOMS_SetVerificationType_ptr = VOMS_SetVerificationType;
#endif /* defined(HAVE_EXT_VOMS) */
#endif

	// If this fails, it means something already configured a threaded
	// model. That won't harm us, so ignore it.
	(*globus_thread_set_model_ptr)( GLOBUS_THREAD_MODEL_NONE );

	if ( (*globus_module_activate_ptr)(globus_i_gsi_gss_assist_module_ptr) ) {
		set_error_string( "couldn't activate globus gsi gss assist module" );
		activation_failed = true;
		return -1;
	}

	globus_gsi_activated = true;
	return 0;
#endif
}