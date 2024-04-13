static struct kvm *kvm_create_vm(unsigned long type)
{
	struct kvm *kvm = kvm_arch_alloc_vm();
	int r = -ENOMEM;
	int i;

	if (!kvm)
		return ERR_PTR(-ENOMEM);

	spin_lock_init(&kvm->mmu_lock);
	mmgrab(current->mm);
	kvm->mm = current->mm;
	kvm_eventfd_init(kvm);
	mutex_init(&kvm->lock);
	mutex_init(&kvm->irq_lock);
	mutex_init(&kvm->slots_lock);
	INIT_LIST_HEAD(&kvm->devices);

	BUILD_BUG_ON(KVM_MEM_SLOTS_NUM > SHRT_MAX);

	if (init_srcu_struct(&kvm->srcu))
		goto out_err_no_srcu;
	if (init_srcu_struct(&kvm->irq_srcu))
		goto out_err_no_irq_srcu;

	refcount_set(&kvm->users_count, 1);
	for (i = 0; i < KVM_ADDRESS_SPACE_NUM; i++) {
		struct kvm_memslots *slots = kvm_alloc_memslots();

		if (!slots)
			goto out_err_no_arch_destroy_vm;
		/* Generations must be different for each address space. */
		slots->generation = i;
		rcu_assign_pointer(kvm->memslots[i], slots);
	}

	for (i = 0; i < KVM_NR_BUSES; i++) {
		rcu_assign_pointer(kvm->buses[i],
			kzalloc(sizeof(struct kvm_io_bus), GFP_KERNEL_ACCOUNT));
		if (!kvm->buses[i])
			goto out_err_no_arch_destroy_vm;
	}

	kvm->max_halt_poll_ns = halt_poll_ns;

	r = kvm_arch_init_vm(kvm, type);
	if (r)
		goto out_err_no_arch_destroy_vm;

	r = hardware_enable_all();
	if (r)
		goto out_err_no_disable;

#ifdef CONFIG_HAVE_KVM_IRQFD
	INIT_HLIST_HEAD(&kvm->irq_ack_notifier_list);
#endif

	r = kvm_init_mmu_notifier(kvm);
	if (r)
		goto out_err_no_mmu_notifier;

	r = kvm_arch_post_init_vm(kvm);
	if (r)
		goto out_err;

	mutex_lock(&kvm_lock);
	list_add(&kvm->vm_list, &vm_list);
	mutex_unlock(&kvm_lock);

	preempt_notifier_inc();

	return kvm;

out_err:
#if defined(CONFIG_MMU_NOTIFIER) && defined(KVM_ARCH_WANT_MMU_NOTIFIER)
	if (kvm->mmu_notifier.ops)
		mmu_notifier_unregister(&kvm->mmu_notifier, current->mm);
#endif
out_err_no_mmu_notifier:
	hardware_disable_all();
out_err_no_disable:
	kvm_arch_destroy_vm(kvm);
out_err_no_arch_destroy_vm:
	WARN_ON_ONCE(!refcount_dec_and_test(&kvm->users_count));
	for (i = 0; i < KVM_NR_BUSES; i++)
		kfree(kvm_get_bus(kvm, i));
	for (i = 0; i < KVM_ADDRESS_SPACE_NUM; i++)
		kvm_free_memslots(kvm, __kvm_memslots(kvm, i));
	cleanup_srcu_struct(&kvm->irq_srcu);
out_err_no_irq_srcu:
	cleanup_srcu_struct(&kvm->srcu);
out_err_no_srcu:
	kvm_arch_free_vm(kvm);
	mmdrop(current->mm);
	return ERR_PTR(r);
}