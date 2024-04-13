static bool cmd_allowed_from_miror(u32 cmd_id)
{
	/*
	 * Allow mirrors VM to call KVM_SEV_LAUNCH_UPDATE_VMSA to enable SEV-ES
	 * active mirror VMs. Also allow the debugging and status commands.
	 */
	if (cmd_id == KVM_SEV_LAUNCH_UPDATE_VMSA ||
	    cmd_id == KVM_SEV_GUEST_STATUS || cmd_id == KVM_SEV_DBG_DECRYPT ||
	    cmd_id == KVM_SEV_DBG_ENCRYPT)
		return true;

	return false;
}