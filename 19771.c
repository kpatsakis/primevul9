void sev_es_unmap_ghcb(struct vcpu_svm *svm)
{
	if (!svm->ghcb)
		return;

	if (svm->ghcb_sa_free) {
		/*
		 * The scratch area lives outside the GHCB, so there is a
		 * buffer that, depending on the operation performed, may
		 * need to be synced, then freed.
		 */
		if (svm->ghcb_sa_sync) {
			kvm_write_guest(svm->vcpu.kvm,
					ghcb_get_sw_scratch(svm->ghcb),
					svm->ghcb_sa, svm->ghcb_sa_len);
			svm->ghcb_sa_sync = false;
		}

		kfree(svm->ghcb_sa);
		svm->ghcb_sa = NULL;
		svm->ghcb_sa_free = false;
	}

	trace_kvm_vmgexit_exit(svm->vcpu.vcpu_id, svm->ghcb);

	sev_es_sync_to_ghcb(svm);

	kvm_vcpu_unmap(&svm->vcpu, &svm->ghcb_map, true);
	svm->ghcb = NULL;
}