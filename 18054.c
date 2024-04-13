static NTSTATUS inherit_new_acl(files_struct *fsp)
{
	TALLOC_CTX *frame = talloc_stackframe();
	char *parent_name = NULL;
	struct security_descriptor *parent_desc = NULL;
	NTSTATUS status = NT_STATUS_OK;
	struct security_descriptor *psd = NULL;
	const struct dom_sid *owner_sid = NULL;
	const struct dom_sid *group_sid = NULL;
	uint32_t security_info_sent = (SECINFO_OWNER | SECINFO_GROUP | SECINFO_DACL);
	struct security_token *token = fsp->conn->session_info->security_token;
	bool inherit_owner = lp_inherit_owner(SNUM(fsp->conn));
	bool inheritable_components = false;
	bool try_builtin_administrators = false;
	const struct dom_sid *BA_U_sid = NULL;
	const struct dom_sid *BA_G_sid = NULL;
	bool try_system = false;
	const struct dom_sid *SY_U_sid = NULL;
	const struct dom_sid *SY_G_sid = NULL;
	size_t size = 0;

	if (!parent_dirname(frame, fsp->fsp_name->base_name, &parent_name, NULL)) {
		TALLOC_FREE(frame);
		return NT_STATUS_NO_MEMORY;
	}

	status = SMB_VFS_GET_NT_ACL(fsp->conn,
				    parent_name,
				    (SECINFO_OWNER | SECINFO_GROUP | SECINFO_DACL),
				    frame,
				    &parent_desc);
	if (!NT_STATUS_IS_OK(status)) {
		TALLOC_FREE(frame);
		return status;
	}

	inheritable_components = sd_has_inheritable_components(parent_desc,
					fsp->is_directory);

	if (!inheritable_components && !inherit_owner) {
		TALLOC_FREE(frame);
		/* Nothing to inherit and not setting owner. */
		return NT_STATUS_OK;
	}

	/* Create an inherited descriptor from the parent. */

	if (DEBUGLEVEL >= 10) {
		DEBUG(10,("inherit_new_acl: parent acl for %s is:\n",
			fsp_str_dbg(fsp) ));
		NDR_PRINT_DEBUG(security_descriptor, parent_desc);
	}

	/* Inherit from parent descriptor if "inherit owner" set. */
	if (inherit_owner) {
		owner_sid = parent_desc->owner_sid;
		group_sid = parent_desc->group_sid;
	}

	if (owner_sid == NULL) {
		if (security_token_has_builtin_administrators(token)) {
			try_builtin_administrators = true;
		} else if (security_token_is_system(token)) {
			try_builtin_administrators = true;
			try_system = true;
		}
	}

	if (group_sid == NULL &&
	    token->num_sids == PRIMARY_GROUP_SID_INDEX)
	{
		if (security_token_is_system(token)) {
			try_builtin_administrators = true;
			try_system = true;
		}
	}

	if (try_builtin_administrators) {
		struct unixid ids;
		bool ok;

		ZERO_STRUCT(ids);
		ok = sids_to_unixids(&global_sid_Builtin_Administrators, 1, &ids);
		if (ok) {
			switch (ids.type) {
			case ID_TYPE_BOTH:
				BA_U_sid = &global_sid_Builtin_Administrators;
				BA_G_sid = &global_sid_Builtin_Administrators;
				break;
			case ID_TYPE_UID:
				BA_U_sid = &global_sid_Builtin_Administrators;
				break;
			case ID_TYPE_GID:
				BA_G_sid = &global_sid_Builtin_Administrators;
				break;
			default:
				break;
			}
		}
	}

	if (try_system) {
		struct unixid ids;
		bool ok;

		ZERO_STRUCT(ids);
		ok = sids_to_unixids(&global_sid_System, 1, &ids);
		if (ok) {
			switch (ids.type) {
			case ID_TYPE_BOTH:
				SY_U_sid = &global_sid_System;
				SY_G_sid = &global_sid_System;
				break;
			case ID_TYPE_UID:
				SY_U_sid = &global_sid_System;
				break;
			case ID_TYPE_GID:
				SY_G_sid = &global_sid_System;
				break;
			default:
				break;
			}
		}
	}

	if (owner_sid == NULL) {
		owner_sid = BA_U_sid;
	}

	if (owner_sid == NULL) {
		owner_sid = SY_U_sid;
	}

	if (group_sid == NULL) {
		group_sid = SY_G_sid;
	}

	if (try_system && group_sid == NULL) {
		group_sid = BA_G_sid;
	}

	if (owner_sid == NULL) {
		owner_sid = &token->sids[PRIMARY_USER_SID_INDEX];
	}
	if (group_sid == NULL) {
		if (token->num_sids == PRIMARY_GROUP_SID_INDEX) {
			group_sid = &token->sids[PRIMARY_USER_SID_INDEX];
		} else {
			group_sid = &token->sids[PRIMARY_GROUP_SID_INDEX];
		}
	}

	status = se_create_child_secdesc(frame,
			&psd,
			&size,
			parent_desc,
			owner_sid,
			group_sid,
			fsp->is_directory);
	if (!NT_STATUS_IS_OK(status)) {
		TALLOC_FREE(frame);
		return status;
	}

	/* If inheritable_components == false,
	   se_create_child_secdesc()
	   creates a security desriptor with a NULL dacl
	   entry, but with SEC_DESC_DACL_PRESENT. We need
	   to remove that flag. */

	if (!inheritable_components) {
		security_info_sent &= ~SECINFO_DACL;
		psd->type &= ~SEC_DESC_DACL_PRESENT;
	}

	if (DEBUGLEVEL >= 10) {
		DEBUG(10,("inherit_new_acl: child acl for %s is:\n",
			fsp_str_dbg(fsp) ));
		NDR_PRINT_DEBUG(security_descriptor, psd);
	}

	if (inherit_owner) {
		/* We need to be root to force this. */
		become_root();
	}
	status = SMB_VFS_FSET_NT_ACL(fsp,
			security_info_sent,
			psd);
	if (inherit_owner) {
		unbecome_root();
	}
	TALLOC_FREE(frame);
	return status;
}