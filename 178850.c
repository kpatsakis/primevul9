static const struct ccid_dependency *dccp_feat_ccid_deps(u8 ccid, bool is_local)
{
	static const struct ccid_dependency ccid2_dependencies[2][2] = {
		/*
		 * CCID2 mandates Ack Vectors (RFC 4341, 4.): as CCID is a TX
		 * feature and Send Ack Vector is an RX feature, `is_local'
		 * needs to be reversed.
		 */
		{	/* Dependencies of the receiver-side (remote) CCID2 */
			{
				.dependent_feat	= DCCPF_SEND_ACK_VECTOR,
				.is_local	= true,
				.is_mandatory	= true,
				.val		= 1
			},
			{ 0, 0, 0, 0 }
		},
		{	/* Dependencies of the sender-side (local) CCID2 */
			{
				.dependent_feat	= DCCPF_SEND_ACK_VECTOR,
				.is_local	= false,
				.is_mandatory	= true,
				.val		= 1
			},
			{ 0, 0, 0, 0 }
		}
	};
	static const struct ccid_dependency ccid3_dependencies[2][5] = {
		{	/*
			 * Dependencies of the receiver-side CCID3
			 */
			{	/* locally disable Ack Vectors */
				.dependent_feat	= DCCPF_SEND_ACK_VECTOR,
				.is_local	= true,
				.is_mandatory	= false,
				.val		= 0
			},
			{	/* see below why Send Loss Event Rate is on */
				.dependent_feat	= DCCPF_SEND_LEV_RATE,
				.is_local	= true,
				.is_mandatory	= true,
				.val		= 1
			},
			{	/* NDP Count is needed as per RFC 4342, 6.1.1 */
				.dependent_feat	= DCCPF_SEND_NDP_COUNT,
				.is_local	= false,
				.is_mandatory	= true,
				.val		= 1
			},
			{ 0, 0, 0, 0 },
		},
		{	/*
			 * CCID3 at the TX side: we request that the HC-receiver
			 * will not send Ack Vectors (they will be ignored, so
			 * Mandatory is not set); we enable Send Loss Event Rate
			 * (Mandatory since the implementation does not support
			 * the Loss Intervals option of RFC 4342, 8.6).
			 * The last two options are for peer's information only.
			*/
			{
				.dependent_feat	= DCCPF_SEND_ACK_VECTOR,
				.is_local	= false,
				.is_mandatory	= false,
				.val		= 0
			},
			{
				.dependent_feat	= DCCPF_SEND_LEV_RATE,
				.is_local	= false,
				.is_mandatory	= true,
				.val		= 1
			},
			{	/* this CCID does not support Ack Ratio */
				.dependent_feat	= DCCPF_ACK_RATIO,
				.is_local	= true,
				.is_mandatory	= false,
				.val		= 0
			},
			{	/* tell receiver we are sending NDP counts */
				.dependent_feat	= DCCPF_SEND_NDP_COUNT,
				.is_local	= true,
				.is_mandatory	= false,
				.val		= 1
			},
			{ 0, 0, 0, 0 }
		}
	};
	switch (ccid) {
	case DCCPC_CCID2:
		return ccid2_dependencies[is_local];
	case DCCPC_CCID3:
		return ccid3_dependencies[is_local];
	default:
		return NULL;
	}
}