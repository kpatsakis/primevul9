void htp_tx_set_config(htp_tx_t *tx, htp_cfg_t *cfg, int is_cfg_shared) {
    if ((tx == NULL) || (cfg == NULL)) return;

    if ((is_cfg_shared != HTP_CONFIG_PRIVATE) && (is_cfg_shared != HTP_CONFIG_SHARED)) return;

    // If we're using a private configuration, destroy it.
    if (tx->is_config_shared == HTP_CONFIG_PRIVATE) {
        htp_config_destroy(tx->cfg);
    }

    tx->cfg = cfg;
    tx->is_config_shared = is_cfg_shared;
}