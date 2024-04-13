static void enable_irq_window(struct kvm_vcpu *vcpu)
{
	struct vcpu_svm *svm = to_svm(vcpu);

	/*
	 * In case GIF=0 we can't rely on the CPU to tell us when GIF becomes
	 * 1, because that's a separate STGI/VMRUN intercept.  The next time we
	 * get that intercept, this function will be called again though and
	 * we'll get the vintr intercept. However, if the vGIF feature is
	 * enabled, the STGI interception will not occur. Enable the irq
	 * window under the assumption that the hardware will set the GIF.
	 */
	if (vgif_enabled(svm) || gif_set(svm)) {
		/*
		 * IRQ window is not needed when AVIC is enabled,
		 * unless we have pending ExtINT since it cannot be injected
		 * via AVIC. In such case, we need to temporarily disable AVIC,
		 * and fallback to injecting IRQ via V_IRQ.
		 */
		svm_toggle_avic_for_irq_window(vcpu, false);
		svm_set_vintr(svm);
	}
}