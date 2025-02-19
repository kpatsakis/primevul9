int ieee80211_rx(struct ieee80211_device *ieee, struct sk_buff *skb,
		 struct ieee80211_rx_stats *rx_stats)
{
	struct net_device *dev = ieee->dev;
	struct ieee80211_hdr_4addr *hdr;
	size_t hdrlen;
	u16 fc, type, stype, sc;
	struct net_device_stats *stats;
	unsigned int frag;
	u8 *payload;
	u16 ethertype;
#ifdef NOT_YET
	struct net_device *wds = NULL;
	struct sk_buff *skb2 = NULL;
	struct net_device *wds = NULL;
	int frame_authorized = 0;
	int from_assoc_ap = 0;
	void *sta = NULL;
#endif
	u8 dst[ETH_ALEN];
	u8 src[ETH_ALEN];
	struct ieee80211_crypt_data *crypt = NULL;
	int keyidx = 0;
	int can_be_decrypted = 0;

	hdr = (struct ieee80211_hdr_4addr *)skb->data;
	stats = &ieee->stats;

	if (skb->len < 10) {
		printk(KERN_INFO "%s: SKB length < 10\n", dev->name);
		goto rx_dropped;
	}

	fc = le16_to_cpu(hdr->frame_ctl);
	type = WLAN_FC_GET_TYPE(fc);
	stype = WLAN_FC_GET_STYPE(fc);
	sc = le16_to_cpu(hdr->seq_ctl);
	frag = WLAN_GET_SEQ_FRAG(sc);
	hdrlen = ieee80211_get_hdrlen(fc);

	if (skb->len < hdrlen) {
		printk(KERN_INFO "%s: invalid SKB length %d\n",
			dev->name, skb->len);
		goto rx_dropped;
	}

	/* Put this code here so that we avoid duplicating it in all
	 * Rx paths. - Jean II */
#ifdef CONFIG_WIRELESS_EXT
#ifdef IW_WIRELESS_SPY		/* defined in iw_handler.h */
	/* If spy monitoring on */
	if (ieee->spy_data.spy_number > 0) {
		struct iw_quality wstats;

		wstats.updated = 0;
		if (rx_stats->mask & IEEE80211_STATMASK_RSSI) {
			wstats.level = rx_stats->rssi;
			wstats.updated |= IW_QUAL_LEVEL_UPDATED;
		} else
			wstats.updated |= IW_QUAL_LEVEL_INVALID;

		if (rx_stats->mask & IEEE80211_STATMASK_NOISE) {
			wstats.noise = rx_stats->noise;
			wstats.updated |= IW_QUAL_NOISE_UPDATED;
		} else
			wstats.updated |= IW_QUAL_NOISE_INVALID;

		if (rx_stats->mask & IEEE80211_STATMASK_SIGNAL) {
			wstats.qual = rx_stats->signal;
			wstats.updated |= IW_QUAL_QUAL_UPDATED;
		} else
			wstats.updated |= IW_QUAL_QUAL_INVALID;

		/* Update spy records */
		wireless_spy_update(ieee->dev, hdr->addr2, &wstats);
	}
#endif				/* IW_WIRELESS_SPY */
#endif				/* CONFIG_WIRELESS_EXT */

#ifdef NOT_YET
	hostap_update_rx_stats(local->ap, hdr, rx_stats);
#endif

	if (ieee->iw_mode == IW_MODE_MONITOR) {
		stats->rx_packets++;
		stats->rx_bytes += skb->len;
		ieee80211_monitor_rx(ieee, skb, rx_stats);
		return 1;
	}

	can_be_decrypted = (is_multicast_ether_addr(hdr->addr1) ||
			    is_broadcast_ether_addr(hdr->addr2)) ?
	    ieee->host_mc_decrypt : ieee->host_decrypt;

	if (can_be_decrypted) {
		if (skb->len >= hdrlen + 3) {
			/* Top two-bits of byte 3 are the key index */
			keyidx = skb->data[hdrlen + 3] >> 6;
		}

		/* ieee->crypt[] is WEP_KEY (4) in length.  Given that keyidx
		 * is only allowed 2-bits of storage, no value of keyidx can
		 * be provided via above code that would result in keyidx
		 * being out of range */
		crypt = ieee->crypt[keyidx];

#ifdef NOT_YET
		sta = NULL;

		/* Use station specific key to override default keys if the
		 * receiver address is a unicast address ("individual RA"). If
		 * bcrx_sta_key parameter is set, station specific key is used
		 * even with broad/multicast targets (this is against IEEE
		 * 802.11, but makes it easier to use different keys with
		 * stations that do not support WEP key mapping). */

		if (!(hdr->addr1[0] & 0x01) || local->bcrx_sta_key)
			(void)hostap_handle_sta_crypto(local, hdr, &crypt,
						       &sta);
#endif

		/* allow NULL decrypt to indicate an station specific override
		 * for default encryption */
		if (crypt && (crypt->ops == NULL ||
			      crypt->ops->decrypt_mpdu == NULL))
			crypt = NULL;

		if (!crypt && (fc & IEEE80211_FCTL_PROTECTED)) {
			/* This seems to be triggered by some (multicast?)
			 * frames from other than current BSS, so just drop the
			 * frames silently instead of filling system log with
			 * these reports. */
			IEEE80211_DEBUG_DROP("Decryption failed (not set)"
					     " (SA=" MAC_FMT ")\n",
					     MAC_ARG(hdr->addr2));
			ieee->ieee_stats.rx_discards_undecryptable++;
			goto rx_dropped;
		}
	}
#ifdef NOT_YET
	if (type != WLAN_FC_TYPE_DATA) {
		if (type == WLAN_FC_TYPE_MGMT && stype == WLAN_FC_STYPE_AUTH &&
		    fc & IEEE80211_FCTL_PROTECTED && ieee->host_decrypt &&
		    (keyidx = hostap_rx_frame_decrypt(ieee, skb, crypt)) < 0) {
			printk(KERN_DEBUG "%s: failed to decrypt mgmt::auth "
			       "from " MAC_FMT "\n", dev->name,
			       MAC_ARG(hdr->addr2));
			/* TODO: could inform hostapd about this so that it
			 * could send auth failure report */
			goto rx_dropped;
		}

		if (ieee80211_rx_frame_mgmt(ieee, skb, rx_stats, type, stype))
			goto rx_dropped;
		else
			goto rx_exit;
	}
#endif
	/* drop duplicate 802.11 retransmissions (IEEE 802.11 Chap. 9.29) */
	if (sc == ieee->prev_seq_ctl)
		goto rx_dropped;
	else
		ieee->prev_seq_ctl = sc;

	/* Data frame - extract src/dst addresses */
	if (skb->len < IEEE80211_3ADDR_LEN)
		goto rx_dropped;

	switch (fc & (IEEE80211_FCTL_FROMDS | IEEE80211_FCTL_TODS)) {
	case IEEE80211_FCTL_FROMDS:
		memcpy(dst, hdr->addr1, ETH_ALEN);
		memcpy(src, hdr->addr3, ETH_ALEN);
		break;
	case IEEE80211_FCTL_TODS:
		memcpy(dst, hdr->addr3, ETH_ALEN);
		memcpy(src, hdr->addr2, ETH_ALEN);
		break;
	case IEEE80211_FCTL_FROMDS | IEEE80211_FCTL_TODS:
		if (skb->len < IEEE80211_4ADDR_LEN)
			goto rx_dropped;
		memcpy(dst, hdr->addr3, ETH_ALEN);
		memcpy(src, hdr->addr4, ETH_ALEN);
		break;
	case 0:
		memcpy(dst, hdr->addr1, ETH_ALEN);
		memcpy(src, hdr->addr2, ETH_ALEN);
		break;
	}

#ifdef NOT_YET
	if (hostap_rx_frame_wds(ieee, hdr, fc, &wds))
		goto rx_dropped;
	if (wds) {
		skb->dev = dev = wds;
		stats = hostap_get_stats(dev);
	}

	if (ieee->iw_mode == IW_MODE_MASTER && !wds &&
	    (fc & (IEEE80211_FCTL_TODS | IEEE80211_FCTL_FROMDS)) ==
	    IEEE80211_FCTL_FROMDS && ieee->stadev
	    && !compare_ether_addr(hdr->addr2, ieee->assoc_ap_addr)) {
		/* Frame from BSSID of the AP for which we are a client */
		skb->dev = dev = ieee->stadev;
		stats = hostap_get_stats(dev);
		from_assoc_ap = 1;
	}
#endif

	dev->last_rx = jiffies;

#ifdef NOT_YET
	if ((ieee->iw_mode == IW_MODE_MASTER ||
	     ieee->iw_mode == IW_MODE_REPEAT) && !from_assoc_ap) {
		switch (hostap_handle_sta_rx(ieee, dev, skb, rx_stats,
					     wds != NULL)) {
		case AP_RX_CONTINUE_NOT_AUTHORIZED:
			frame_authorized = 0;
			break;
		case AP_RX_CONTINUE:
			frame_authorized = 1;
			break;
		case AP_RX_DROP:
			goto rx_dropped;
		case AP_RX_EXIT:
			goto rx_exit;
		}
	}
#endif

	/* Nullfunc frames may have PS-bit set, so they must be passed to
	 * hostap_handle_sta_rx() before being dropped here. */

	stype &= ~IEEE80211_STYPE_QOS_DATA;

	if (stype != IEEE80211_STYPE_DATA &&
	    stype != IEEE80211_STYPE_DATA_CFACK &&
	    stype != IEEE80211_STYPE_DATA_CFPOLL &&
	    stype != IEEE80211_STYPE_DATA_CFACKPOLL) {
		if (stype != IEEE80211_STYPE_NULLFUNC)
			IEEE80211_DEBUG_DROP("RX: dropped data frame "
					     "with no data (type=0x%02x, "
					     "subtype=0x%02x, len=%d)\n",
					     type, stype, skb->len);
		goto rx_dropped;
	}

	/* skb: hdr + (possibly fragmented, possibly encrypted) payload */

	if ((fc & IEEE80211_FCTL_PROTECTED) && can_be_decrypted &&
	    (keyidx = ieee80211_rx_frame_decrypt(ieee, skb, crypt)) < 0)
		goto rx_dropped;

	hdr = (struct ieee80211_hdr_4addr *)skb->data;

	/* skb: hdr + (possibly fragmented) plaintext payload */
	// PR: FIXME: hostap has additional conditions in the "if" below:
	// ieee->host_decrypt && (fc & IEEE80211_FCTL_PROTECTED) &&
	if ((frag != 0) || (fc & IEEE80211_FCTL_MOREFRAGS)) {
		int flen;
		struct sk_buff *frag_skb = ieee80211_frag_cache_get(ieee, hdr);
		IEEE80211_DEBUG_FRAG("Rx Fragment received (%u)\n", frag);

		if (!frag_skb) {
			IEEE80211_DEBUG(IEEE80211_DL_RX | IEEE80211_DL_FRAG,
					"Rx cannot get skb from fragment "
					"cache (morefrag=%d seq=%u frag=%u)\n",
					(fc & IEEE80211_FCTL_MOREFRAGS) != 0,
					WLAN_GET_SEQ_SEQ(sc), frag);
			goto rx_dropped;
		}

		flen = skb->len;
		if (frag != 0)
			flen -= hdrlen;

		if (frag_skb->tail + flen > frag_skb->end) {
			printk(KERN_WARNING "%s: host decrypted and "
			       "reassembled frame did not fit skb\n",
			       dev->name);
			ieee80211_frag_cache_invalidate(ieee, hdr);
			goto rx_dropped;
		}

		if (frag == 0) {
			/* copy first fragment (including full headers) into
			 * beginning of the fragment cache skb */
			skb_copy_from_linear_data(skb, skb_put(frag_skb, flen), flen);
		} else {
			/* append frame payload to the end of the fragment
			 * cache skb */
			skb_copy_from_linear_data_offset(skb, hdrlen,
				      skb_put(frag_skb, flen), flen);
		}
		dev_kfree_skb_any(skb);
		skb = NULL;

		if (fc & IEEE80211_FCTL_MOREFRAGS) {
			/* more fragments expected - leave the skb in fragment
			 * cache for now; it will be delivered to upper layers
			 * after all fragments have been received */
			goto rx_exit;
		}

		/* this was the last fragment and the frame will be
		 * delivered, so remove skb from fragment cache */
		skb = frag_skb;
		hdr = (struct ieee80211_hdr_4addr *)skb->data;
		ieee80211_frag_cache_invalidate(ieee, hdr);
	}

	/* skb: hdr + (possible reassembled) full MSDU payload; possibly still
	 * encrypted/authenticated */
	if ((fc & IEEE80211_FCTL_PROTECTED) && can_be_decrypted &&
	    ieee80211_rx_frame_decrypt_msdu(ieee, skb, keyidx, crypt))
		goto rx_dropped;

	hdr = (struct ieee80211_hdr_4addr *)skb->data;
	if (crypt && !(fc & IEEE80211_FCTL_PROTECTED) && !ieee->open_wep) {
		if (		/*ieee->ieee802_1x && */
			   ieee80211_is_eapol_frame(ieee, skb)) {
			/* pass unencrypted EAPOL frames even if encryption is
			 * configured */
		} else {
			IEEE80211_DEBUG_DROP("encryption configured, but RX "
					     "frame not encrypted (SA=" MAC_FMT
					     ")\n", MAC_ARG(hdr->addr2));
			goto rx_dropped;
		}
	}

	if (crypt && !(fc & IEEE80211_FCTL_PROTECTED) && !ieee->open_wep &&
	    !ieee80211_is_eapol_frame(ieee, skb)) {
		IEEE80211_DEBUG_DROP("dropped unencrypted RX data "
				     "frame from " MAC_FMT
				     " (drop_unencrypted=1)\n",
				     MAC_ARG(hdr->addr2));
		goto rx_dropped;
	}

	/* If the frame was decrypted in hardware, we may need to strip off
	 * any security data (IV, ICV, etc) that was left behind */
	if (!can_be_decrypted && (fc & IEEE80211_FCTL_PROTECTED) &&
	    ieee->host_strip_iv_icv) {
		int trimlen = 0;

		/* Top two-bits of byte 3 are the key index */
		if (skb->len >= hdrlen + 3)
			keyidx = skb->data[hdrlen + 3] >> 6;

		/* To strip off any security data which appears before the
		 * payload, we simply increase hdrlen (as the header gets
		 * chopped off immediately below). For the security data which
		 * appears after the payload, we use skb_trim. */

		switch (ieee->sec.encode_alg[keyidx]) {
		case SEC_ALG_WEP:
			/* 4 byte IV */
			hdrlen += 4;
			/* 4 byte ICV */
			trimlen = 4;
			break;
		case SEC_ALG_TKIP:
			/* 4 byte IV, 4 byte ExtIV */
			hdrlen += 8;
			/* 8 byte MIC, 4 byte ICV */
			trimlen = 12;
			break;
		case SEC_ALG_CCMP:
			/* 8 byte CCMP header */
			hdrlen += 8;
			/* 8 byte MIC */
			trimlen = 8;
			break;
		}

		if (skb->len < trimlen)
			goto rx_dropped;

		__skb_trim(skb, skb->len - trimlen);

		if (skb->len < hdrlen)
			goto rx_dropped;
	}

	/* skb: hdr + (possible reassembled) full plaintext payload */

	payload = skb->data + hdrlen;
	ethertype = (payload[6] << 8) | payload[7];

#ifdef NOT_YET
	/* If IEEE 802.1X is used, check whether the port is authorized to send
	 * the received frame. */
	if (ieee->ieee802_1x && ieee->iw_mode == IW_MODE_MASTER) {
		if (ethertype == ETH_P_PAE) {
			printk(KERN_DEBUG "%s: RX: IEEE 802.1X frame\n",
			       dev->name);
			if (ieee->hostapd && ieee->apdev) {
				/* Send IEEE 802.1X frames to the user
				 * space daemon for processing */
				prism2_rx_80211(ieee->apdev, skb, rx_stats,
						PRISM2_RX_MGMT);
				ieee->apdevstats.rx_packets++;
				ieee->apdevstats.rx_bytes += skb->len;
				goto rx_exit;
			}
		} else if (!frame_authorized) {
			printk(KERN_DEBUG "%s: dropped frame from "
			       "unauthorized port (IEEE 802.1X): "
			       "ethertype=0x%04x\n", dev->name, ethertype);
			goto rx_dropped;
		}
	}
#endif

	/* convert hdr + possible LLC headers into Ethernet header */
	if (skb->len - hdrlen >= 8 &&
	    ((memcmp(payload, rfc1042_header, SNAP_SIZE) == 0 &&
	      ethertype != ETH_P_AARP && ethertype != ETH_P_IPX) ||
	     memcmp(payload, bridge_tunnel_header, SNAP_SIZE) == 0)) {
		/* remove RFC1042 or Bridge-Tunnel encapsulation and
		 * replace EtherType */
		skb_pull(skb, hdrlen + SNAP_SIZE);
		memcpy(skb_push(skb, ETH_ALEN), src, ETH_ALEN);
		memcpy(skb_push(skb, ETH_ALEN), dst, ETH_ALEN);
	} else {
		u16 len;
		/* Leave Ethernet header part of hdr and full payload */
		skb_pull(skb, hdrlen);
		len = htons(skb->len);
		memcpy(skb_push(skb, 2), &len, 2);
		memcpy(skb_push(skb, ETH_ALEN), src, ETH_ALEN);
		memcpy(skb_push(skb, ETH_ALEN), dst, ETH_ALEN);
	}

#ifdef NOT_YET
	if (wds && ((fc & (IEEE80211_FCTL_TODS | IEEE80211_FCTL_FROMDS)) ==
		    IEEE80211_FCTL_TODS) && skb->len >= ETH_HLEN + ETH_ALEN) {
		/* Non-standard frame: get addr4 from its bogus location after
		 * the payload */
		skb_copy_to_linear_data_offset(skb, ETH_ALEN,
					       skb->data + skb->len - ETH_ALEN,
					       ETH_ALEN);
		skb_trim(skb, skb->len - ETH_ALEN);
	}
#endif

	stats->rx_packets++;
	stats->rx_bytes += skb->len;

#ifdef NOT_YET
	if (ieee->iw_mode == IW_MODE_MASTER && !wds && ieee->ap->bridge_packets) {
		if (dst[0] & 0x01) {
			/* copy multicast frame both to the higher layers and
			 * to the wireless media */
			ieee->ap->bridged_multicast++;
			skb2 = skb_clone(skb, GFP_ATOMIC);
			if (skb2 == NULL)
				printk(KERN_DEBUG "%s: skb_clone failed for "
				       "multicast frame\n", dev->name);
		} else if (hostap_is_sta_assoc(ieee->ap, dst)) {
			/* send frame directly to the associated STA using
			 * wireless media and not passing to higher layers */
			ieee->ap->bridged_unicast++;
			skb2 = skb;
			skb = NULL;
		}
	}

	if (skb2 != NULL) {
		/* send to wireless media */
		skb2->dev = dev;
		skb2->protocol = __constant_htons(ETH_P_802_3);
		skb_reset_mac_header(skb2);
		skb_reset_network_header(skb2);
		/* skb2->network_header += ETH_HLEN; */
		dev_queue_xmit(skb2);
	}
#endif

	if (skb) {
		skb->protocol = eth_type_trans(skb, dev);
		memset(skb->cb, 0, sizeof(skb->cb));
		skb->ip_summed = CHECKSUM_NONE;	/* 802.11 crc not sufficient */
		if (netif_rx(skb) == NET_RX_DROP) {
			/* netif_rx always succeeds, but it might drop
			 * the packet.  If it drops the packet, we log that
			 * in our stats. */
			IEEE80211_DEBUG_DROP
			    ("RX: netif_rx dropped the packet\n");
			stats->rx_dropped++;
		}
	}

      rx_exit:
#ifdef NOT_YET
	if (sta)
		hostap_handle_sta_release(sta);
#endif
	return 1;

      rx_dropped:
	stats->rx_dropped++;

	/* Returning 0 indicates to caller that we have not handled the SKB--
	 * so it is still allocated and can be used again by underlying
	 * hardware as a DMA target */
	return 0;
}