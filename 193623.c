static int gg_notify105_ex(struct gg_session *sess, uin_t *userlist, char *types, int count)
{
	int i = 0;

	if (!userlist || !count)
		return gg_send_packet(sess, GG_NOTIFY105_LIST_EMPTY, NULL);

	while (i < count) {
		gg_tvbuilder_t *tvb = gg_tvbuilder_new(sess, NULL);
		gg_tvbuilder_expected_size(tvb, 2100);

		while (i < count) {
			size_t prev_size = gg_tvbuilder_get_size(tvb);
			gg_tvbuilder_write_uin(tvb, userlist[i], 0);
			gg_tvbuilder_write_uint8(tvb,
				(types == NULL) ? GG_USER_NORMAL : types[i]);

			/* Oryginalny klient wysyła maksymalnie 2048 bajtów 
			 * danych w każdym pakiecie tego typu.
			 */
			if (gg_tvbuilder_get_size(tvb) > 2048) {
				gg_tvbuilder_strip(tvb, prev_size);
				break;
			}
			i++;
		}

		if (!gg_tvbuilder_send(tvb, (i < count) ?
			GG_NOTIFY105_FIRST : GG_NOTIFY105_LAST)) {
			return -1;
		}
	}

	return 0;
}