static void sas_print_parent_topology_bug(struct domain_device *child,
						 struct ex_phy *parent_phy,
						 struct ex_phy *child_phy)
{
	static const char *ex_type[] = {
		[SAS_EDGE_EXPANDER_DEVICE] = "edge",
		[SAS_FANOUT_EXPANDER_DEVICE] = "fanout",
	};
	struct domain_device *parent = child->parent;

	pr_notice("%s ex %016llx phy%02d <--> %s ex %016llx phy%02d has %c:%c routing link!\n",
		  ex_type[parent->dev_type],
		  SAS_ADDR(parent->sas_addr),
		  parent_phy->phy_id,

		  ex_type[child->dev_type],
		  SAS_ADDR(child->sas_addr),
		  child_phy->phy_id,

		  sas_route_char(parent, parent_phy),
		  sas_route_char(child, child_phy));
}