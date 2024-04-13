static int bond_kobj_init(struct slave *slave)
{
	int err;

	err = kobject_init_and_add(&slave->kobj, &slave_ktype,
				   &(slave->dev->dev.kobj), "bonding_slave");
	if (err)
		kobject_put(&slave->kobj);

	return err;
}