static int __init nf_tables_module_init(void)
{
	int err;

	info = kmalloc(sizeof(struct nft_expr_info) * NFT_RULE_MAXEXPRS,
		       GFP_KERNEL);
	if (info == NULL) {
		err = -ENOMEM;
		goto err1;
	}

	err = nf_tables_core_module_init();
	if (err < 0)
		goto err2;

	err = nfnetlink_subsys_register(&nf_tables_subsys);
	if (err < 0)
		goto err3;

	pr_info("nf_tables: (c) 2007-2009 Patrick McHardy <kaber@trash.net>\n");
	return register_pernet_subsys(&nf_tables_net_ops);
err3:
	nf_tables_core_module_exit();
err2:
	kfree(info);
err1:
	return err;
}