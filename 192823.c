static int hclge_tm_pg_to_pri_map_cfg(struct hclge_dev *hdev,
				      u8 pg_id, u8 pri_bit_map)
{
	struct hclge_pg_to_pri_link_cmd *map;
	struct hclge_desc desc;

	hclge_cmd_setup_basic_desc(&desc, HCLGE_OPC_TM_PG_TO_PRI_LINK, false);

	map = (struct hclge_pg_to_pri_link_cmd *)desc.data;

	map->pg_id = pg_id;
	map->pri_bit_map = pri_bit_map;

	return hclge_cmd_send(&hdev->hw, &desc, 1);
}