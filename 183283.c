char *csr_pskeytostr(uint16_t pskey)
{
	switch (pskey) {
	case CSR_PSKEY_BDADDR:
		return "Bluetooth address";
	case CSR_PSKEY_COUNTRYCODE:
		return "Country code";
	case CSR_PSKEY_CLASSOFDEVICE:
		return "Class of device";
	case CSR_PSKEY_DEVICE_DRIFT:
		return "Device drift";
	case CSR_PSKEY_DEVICE_JITTER:
		return "Device jitter";
	case CSR_PSKEY_MAX_ACLS:
		return "Maximum ACL links";
	case CSR_PSKEY_MAX_SCOS:
		return "Maximum SCO links";
	case CSR_PSKEY_MAX_REMOTE_MASTERS:
		return "Maximum remote masters";
	case CSR_PSKEY_ENABLE_MASTERY_WITH_SLAVERY:
		return "Support master and slave roles simultaneously";
	case CSR_PSKEY_H_HC_FC_MAX_ACL_PKT_LEN:
		return "Maximum HCI ACL packet length";
	case CSR_PSKEY_H_HC_FC_MAX_SCO_PKT_LEN:
		return "Maximum HCI SCO packet length";
	case CSR_PSKEY_H_HC_FC_MAX_ACL_PKTS:
		return "Maximum number of HCI ACL packets";
	case CSR_PSKEY_H_HC_FC_MAX_SCO_PKTS:
		return "Maximum number of HCI SCO packets";
	case CSR_PSKEY_LC_FC_BUFFER_LOW_WATER_MARK:
		return "Flow control low water mark";
	case CSR_PSKEY_LC_MAX_TX_POWER:
		return "Maximum transmit power";
	case CSR_PSKEY_TX_GAIN_RAMP:
		return "Transmit gain ramp rate";
	case CSR_PSKEY_LC_POWER_TABLE:
		return "Radio power table";
	case CSR_PSKEY_LC_PEER_POWER_PERIOD:
		return "Peer transmit power control interval";
	case CSR_PSKEY_LC_FC_POOLS_LOW_WATER_MARK:
		return "Flow control pool low water mark";
	case CSR_PSKEY_LC_DEFAULT_TX_POWER:
		return "Default transmit power";
	case CSR_PSKEY_LC_RSSI_GOLDEN_RANGE:
		return "RSSI at bottom of golden receive range";
	case CSR_PSKEY_LC_COMBO_DISABLE_PIO_MASK:
		return "Combo: PIO lines and logic to disable transmit";
	case CSR_PSKEY_LC_COMBO_PRIORITY_PIO_MASK:
		return "Combo: priority activity PIO lines and logic";
	case CSR_PSKEY_LC_COMBO_DOT11_CHANNEL_PIO_BASE:
		return "Combo: 802.11b channel number base PIO line";
	case CSR_PSKEY_LC_COMBO_DOT11_BLOCK_CHANNELS:
		return "Combo: channels to block either side of 802.11b";
	case CSR_PSKEY_LC_MAX_TX_POWER_NO_RSSI:
		return "Maximum transmit power when peer has no RSSI";
	case CSR_PSKEY_LC_CONNECTION_RX_WINDOW:
		return "Receive window size during connections";
	case CSR_PSKEY_LC_COMBO_DOT11_TX_PROTECTION_MODE:
		return "Combo: which TX packets shall we protect";
	case CSR_PSKEY_LC_ENHANCED_POWER_TABLE:
		return "Radio power table";
	case CSR_PSKEY_LC_WIDEBAND_RSSI_CONFIG:
		return "RSSI configuration for use with wideband RSSI";
	case CSR_PSKEY_LC_COMBO_DOT11_PRIORITY_LEAD:
		return "Combo: How much notice will we give the Combo Card";
	case CSR_PSKEY_BT_CLOCK_INIT:
		return "Initial value of Bluetooth clock";
	case CSR_PSKEY_TX_MR_MOD_DELAY:
		return "TX Mod delay";
	case CSR_PSKEY_RX_MR_SYNC_TIMING:
		return "RX MR Sync Timing";
	case CSR_PSKEY_RX_MR_SYNC_CONFIG:
		return "RX MR Sync Configuration";
	case CSR_PSKEY_LC_LOST_SYNC_SLOTS:
		return "Time in ms for lost sync in low power modes";
	case CSR_PSKEY_RX_MR_SAMP_CONFIG:
		return "RX MR Sync Configuration";
	case CSR_PSKEY_AGC_HYST_LEVELS:
		return "AGC hysteresis levels";
	case CSR_PSKEY_RX_LEVEL_LOW_SIGNAL:
		return "ANA_RX_LVL at low signal strengths";
	case CSR_PSKEY_AGC_IQ_LVL_VALUES:
		return "ANA_IQ_LVL values for AGC algorithmn";
	case CSR_PSKEY_MR_FTRIM_OFFSET_12DB:
		return "ANA_RX_FTRIM offset when using 12 dB IF atten ";
	case CSR_PSKEY_MR_FTRIM_OFFSET_6DB:
		return "ANA_RX_FTRIM offset when using 6 dB IF atten ";
	case CSR_PSKEY_NO_CAL_ON_BOOT:
		return "Do not calibrate radio on boot";
	case CSR_PSKEY_RSSI_HI_TARGET:
		return "RSSI high target";
	case CSR_PSKEY_PREFERRED_MIN_ATTENUATION:
		return "Preferred minimum attenuator setting";
	case CSR_PSKEY_LC_COMBO_DOT11_PRIORITY_OVERRIDE:
		return "Combo: Treat all packets as high priority";
	case CSR_PSKEY_LC_MULTISLOT_HOLDOFF:
		return "Time till single slot packets are used for resync";
	case CSR_PSKEY_FREE_KEY_PIGEON_HOLE:
		return "Link key store bitfield";
	case CSR_PSKEY_LINK_KEY_BD_ADDR0:
		return "Bluetooth address + link key 0";
	case CSR_PSKEY_LINK_KEY_BD_ADDR1:
		return "Bluetooth address + link key 1";
	case CSR_PSKEY_LINK_KEY_BD_ADDR2:
		return "Bluetooth address + link key 2";
	case CSR_PSKEY_LINK_KEY_BD_ADDR3:
		return "Bluetooth address + link key 3";
	case CSR_PSKEY_LINK_KEY_BD_ADDR4:
		return "Bluetooth address + link key 4";
	case CSR_PSKEY_LINK_KEY_BD_ADDR5:
		return "Bluetooth address + link key 5";
	case CSR_PSKEY_LINK_KEY_BD_ADDR6:
		return "Bluetooth address + link key 6";
	case CSR_PSKEY_LINK_KEY_BD_ADDR7:
		return "Bluetooth address + link key 7";
	case CSR_PSKEY_LINK_KEY_BD_ADDR8:
		return "Bluetooth address + link key 8";
	case CSR_PSKEY_LINK_KEY_BD_ADDR9:
		return "Bluetooth address + link key 9";
	case CSR_PSKEY_LINK_KEY_BD_ADDR10:
		return "Bluetooth address + link key 10";
	case CSR_PSKEY_LINK_KEY_BD_ADDR11:
		return "Bluetooth address + link key 11";
	case CSR_PSKEY_LINK_KEY_BD_ADDR12:
		return "Bluetooth address + link key 12";
	case CSR_PSKEY_LINK_KEY_BD_ADDR13:
		return "Bluetooth address + link key 13";
	case CSR_PSKEY_LINK_KEY_BD_ADDR14:
		return "Bluetooth address + link key 14";
	case CSR_PSKEY_LINK_KEY_BD_ADDR15:
		return "Bluetooth address + link key 15";
	case CSR_PSKEY_ENC_KEY_LMIN:
		return "Minimum encryption key length";
	case CSR_PSKEY_ENC_KEY_LMAX:
		return "Maximum encryption key length";
	case CSR_PSKEY_LOCAL_SUPPORTED_FEATURES:
		return "Local supported features block";
	case CSR_PSKEY_LM_USE_UNIT_KEY:
		return "Allow use of unit key for authentication?";
	case CSR_PSKEY_HCI_NOP_DISABLE:
		return "Disable the HCI Command_Status event on boot";
	case CSR_PSKEY_LM_MAX_EVENT_FILTERS:
		return "Maximum number of event filters";
	case CSR_PSKEY_LM_USE_ENC_MODE_BROADCAST:
		return "Allow LM to use enc_mode=2";
	case CSR_PSKEY_LM_TEST_SEND_ACCEPTED_TWICE:
		return "LM sends two LMP_accepted messages in test mode";
	case CSR_PSKEY_LM_MAX_PAGE_HOLD_TIME:
		return "Maximum time we hold a device around page";
	case CSR_PSKEY_AFH_ADAPTATION_RESPONSE_TIME:
		return "LM period for AFH adaption";
	case CSR_PSKEY_AFH_OPTIONS:
		return "Options to configure AFH";
	case CSR_PSKEY_AFH_RSSI_RUN_PERIOD:
		return "AFH RSSI reading period";
	case CSR_PSKEY_AFH_REENABLE_CHANNEL_TIME:
		return "AFH good channel adding time";
	case CSR_PSKEY_NO_DROP_ON_ACR_MS_FAIL:
		return "Complete link if acr barge-in role switch refused";
	case CSR_PSKEY_MAX_PRIVATE_KEYS:
		return "Max private link keys stored";
	case CSR_PSKEY_PRIVATE_LINK_KEY_BD_ADDR0:
		return "Bluetooth address + link key 0";
	case CSR_PSKEY_PRIVATE_LINK_KEY_BD_ADDR1:
		return "Bluetooth address + link key 1";
	case CSR_PSKEY_PRIVATE_LINK_KEY_BD_ADDR2:
		return "Bluetooth address + link key 2";
	case CSR_PSKEY_PRIVATE_LINK_KEY_BD_ADDR3:
		return "Bluetooth address + link key 3";
	case CSR_PSKEY_PRIVATE_LINK_KEY_BD_ADDR4:
		return "Bluetooth address + link key 4";
	case CSR_PSKEY_PRIVATE_LINK_KEY_BD_ADDR5:
		return "Bluetooth address + link key 5";
	case CSR_PSKEY_PRIVATE_LINK_KEY_BD_ADDR6:
		return "Bluetooth address + link key 6";
	case CSR_PSKEY_PRIVATE_LINK_KEY_BD_ADDR7:
		return "Bluetooth address + link key 7";
	case CSR_PSKEY_LOCAL_SUPPORTED_COMMANDS:
		return "Local supported commands";
	case CSR_PSKEY_LM_MAX_ABSENCE_INDEX:
		return "Maximum absence index allowed";
	case CSR_PSKEY_DEVICE_NAME:
		return "Local device's \"user friendly\" name";
	case CSR_PSKEY_AFH_RSSI_THRESHOLD:
		return "AFH RSSI threshold";
	case CSR_PSKEY_LM_CASUAL_SCAN_INTERVAL:
		return "Scan interval in slots for casual scanning";
	case CSR_PSKEY_AFH_MIN_MAP_CHANGE:
		return "The minimum amount to change an AFH map by";
	case CSR_PSKEY_AFH_RSSI_LP_RUN_PERIOD:
		return "AFH RSSI reading period when in low power mode";
	case CSR_PSKEY_HCI_LMP_LOCAL_VERSION:
		return "The HCI and LMP version reported locally";
	case CSR_PSKEY_LMP_REMOTE_VERSION:
		return "The LMP version reported remotely";
	case CSR_PSKEY_HOLD_ERROR_MESSAGE_NUMBER:
		return "Maximum number of queued HCI Hardware Error Events";
	case CSR_PSKEY_DFU_ATTRIBUTES:
		return "DFU attributes";
	case CSR_PSKEY_DFU_DETACH_TO:
		return "DFU detach timeout";
	case CSR_PSKEY_DFU_TRANSFER_SIZE:
		return "DFU transfer size";
	case CSR_PSKEY_DFU_ENABLE:
		return "DFU enable";
	case CSR_PSKEY_DFU_LIN_REG_ENABLE:
		return "Linear Regulator enabled at boot in DFU mode";
	case CSR_PSKEY_DFUENC_VMAPP_PK_MODULUS_MSB:
		return "DFU encryption VM application public key MSB";
	case CSR_PSKEY_DFUENC_VMAPP_PK_MODULUS_LSB:
		return "DFU encryption VM application public key LSB";
	case CSR_PSKEY_DFUENC_VMAPP_PK_M_DASH:
		return "DFU encryption VM application M dash";
	case CSR_PSKEY_DFUENC_VMAPP_PK_R2N_MSB:
		return "DFU encryption VM application public key R2N MSB";
	case CSR_PSKEY_DFUENC_VMAPP_PK_R2N_LSB:
		return "DFU encryption VM application public key R2N LSB";
	case CSR_PSKEY_BCSP_LM_PS_BLOCK:
		return "BCSP link establishment block";
	case CSR_PSKEY_HOSTIO_FC_PS_BLOCK:
		return "HCI flow control block";
	case CSR_PSKEY_HOSTIO_PROTOCOL_INFO0:
		return "Host transport channel 0 settings (BCSP ACK)";
	case CSR_PSKEY_HOSTIO_PROTOCOL_INFO1:
		return "Host transport channel 1 settings (BCSP-LE)";
	case CSR_PSKEY_HOSTIO_PROTOCOL_INFO2:
		return "Host transport channel 2 settings (BCCMD)";
	case CSR_PSKEY_HOSTIO_PROTOCOL_INFO3:
		return "Host transport channel 3 settings (HQ)";
	case CSR_PSKEY_HOSTIO_PROTOCOL_INFO4:
		return "Host transport channel 4 settings (DM)";
	case CSR_PSKEY_HOSTIO_PROTOCOL_INFO5:
		return "Host transport channel 5 settings (HCI CMD/EVT)";
	case CSR_PSKEY_HOSTIO_PROTOCOL_INFO6:
		return "Host transport channel 6 settings (HCI ACL)";
	case CSR_PSKEY_HOSTIO_PROTOCOL_INFO7:
		return "Host transport channel 7 settings (HCI SCO)";
	case CSR_PSKEY_HOSTIO_PROTOCOL_INFO8:
		return "Host transport channel 8 settings (L2CAP)";
	case CSR_PSKEY_HOSTIO_PROTOCOL_INFO9:
		return "Host transport channel 9 settings (RFCOMM)";
	case CSR_PSKEY_HOSTIO_PROTOCOL_INFO10:
		return "Host transport channel 10 settings (SDP)";
	case CSR_PSKEY_HOSTIO_PROTOCOL_INFO11:
		return "Host transport channel 11 settings (TEST)";
	case CSR_PSKEY_HOSTIO_PROTOCOL_INFO12:
		return "Host transport channel 12 settings (DFU)";
	case CSR_PSKEY_HOSTIO_PROTOCOL_INFO13:
		return "Host transport channel 13 settings (VM)";
	case CSR_PSKEY_HOSTIO_PROTOCOL_INFO14:
		return "Host transport channel 14 settings";
	case CSR_PSKEY_HOSTIO_PROTOCOL_INFO15:
		return "Host transport channel 15 settings";
	case CSR_PSKEY_HOSTIO_UART_RESET_TIMEOUT:
		return "UART reset counter timeout";
	case CSR_PSKEY_HOSTIO_USE_HCI_EXTN:
		return "Use hci_extn to route non-hci channels";
	case CSR_PSKEY_HOSTIO_USE_HCI_EXTN_CCFC:
		return "Use command-complete flow control for hci_extn";
	case CSR_PSKEY_HOSTIO_HCI_EXTN_PAYLOAD_SIZE:
		return "Maximum hci_extn payload size";
	case CSR_PSKEY_BCSP_LM_CNF_CNT_LIMIT:
		return "BCSP link establishment conf message count";
	case CSR_PSKEY_HOSTIO_MAP_SCO_PCM:
		return "Map SCO over PCM";
	case CSR_PSKEY_HOSTIO_AWKWARD_PCM_SYNC:
		return "PCM interface synchronisation is difficult";
	case CSR_PSKEY_HOSTIO_BREAK_POLL_PERIOD:
		return "Break poll period (microseconds)";
	case CSR_PSKEY_HOSTIO_MIN_UART_HCI_SCO_SIZE:
		return "Minimum SCO packet size sent to host over UART HCI";
	case CSR_PSKEY_HOSTIO_MAP_SCO_CODEC:
		return "Map SCO over the built-in codec";
	case CSR_PSKEY_PCM_CVSD_TX_HI_FREQ_BOOST:
		return "High frequency boost for PCM when transmitting CVSD";
	case CSR_PSKEY_PCM_CVSD_RX_HI_FREQ_BOOST:
		return "High frequency boost for PCM when receiving CVSD";
	case CSR_PSKEY_PCM_CONFIG32:
		return "PCM interface settings bitfields";
	case CSR_PSKEY_USE_OLD_BCSP_LE:
		return "Use the old version of BCSP link establishment";
	case CSR_PSKEY_PCM_CVSD_USE_NEW_FILTER:
		return "CVSD uses the new filter if available";
	case CSR_PSKEY_PCM_FORMAT:
		return "PCM data format";
	case CSR_PSKEY_CODEC_OUT_GAIN:
		return "Audio output gain when using built-in codec";
	case CSR_PSKEY_CODEC_IN_GAIN:
		return "Audio input gain when using built-in codec";
	case CSR_PSKEY_CODEC_PIO:
		return "PIO to enable when built-in codec is enabled";
	case CSR_PSKEY_PCM_LOW_JITTER_CONFIG:
		return "PCM interface settings for low jitter master mode";
	case CSR_PSKEY_HOSTIO_SCO_PCM_THRESHOLDS:
		return "Thresholds for SCO PCM buffers";
	case CSR_PSKEY_HOSTIO_SCO_HCI_THRESHOLDS:
		return "Thresholds for SCO HCI buffers";
	case CSR_PSKEY_HOSTIO_MAP_SCO_PCM_SLOT:
		return "Route SCO data to specified slot in pcm frame";
	case CSR_PSKEY_UART_BAUDRATE:
		return "UART Baud rate";
	case CSR_PSKEY_UART_CONFIG_BCSP:
		return "UART configuration when using BCSP";
	case CSR_PSKEY_UART_CONFIG_H4:
		return "UART configuration when using H4";
	case CSR_PSKEY_UART_CONFIG_H5:
		return "UART configuration when using H5";
	case CSR_PSKEY_UART_CONFIG_USR:
		return "UART configuration when under VM control";
	case CSR_PSKEY_UART_TX_CRCS:
		return "Use CRCs for BCSP or H5";
	case CSR_PSKEY_UART_ACK_TIMEOUT:
		return "Acknowledgement timeout for BCSP and H5";
	case CSR_PSKEY_UART_TX_MAX_ATTEMPTS:
		return "Max times to send reliable BCSP or H5 message";
	case CSR_PSKEY_UART_TX_WINDOW_SIZE:
		return "Transmit window size for BCSP and H5";
	case CSR_PSKEY_UART_HOST_WAKE:
		return "UART host wakeup";
	case CSR_PSKEY_HOSTIO_THROTTLE_TIMEOUT:
		return "Host interface performance control.";
	case CSR_PSKEY_PCM_ALWAYS_ENABLE:
		return "PCM port is always enable when chip is running";
	case CSR_PSKEY_UART_HOST_WAKE_SIGNAL:
		return "Signal to use for uart host wakeup protocol";
	case CSR_PSKEY_UART_CONFIG_H4DS:
		return "UART configuration when using H4DS";
	case CSR_PSKEY_H4DS_WAKE_DURATION:
		return "How long to spend waking the host when using H4DS";
	case CSR_PSKEY_H4DS_MAXWU:
		return "Maximum number of H4DS Wake-Up messages to send";
	case CSR_PSKEY_H4DS_LE_TIMER_PERIOD:
		return "H4DS Link Establishment Tsync and Tconf period";
	case CSR_PSKEY_H4DS_TWU_TIMER_PERIOD:
		return "H4DS Twu timer period";
	case CSR_PSKEY_H4DS_UART_IDLE_TIMER_PERIOD:
		return "H4DS Tuart_idle timer period";
	case CSR_PSKEY_ANA_FTRIM:
		return "Crystal frequency trim";
	case CSR_PSKEY_WD_TIMEOUT:
		return "Watchdog timeout (microseconds)";
	case CSR_PSKEY_WD_PERIOD:
		return "Watchdog period (microseconds)";
	case CSR_PSKEY_HOST_INTERFACE:
		return "Host interface";
	case CSR_PSKEY_HQ_HOST_TIMEOUT:
		return "HQ host command timeout";
	case CSR_PSKEY_HQ_ACTIVE:
		return "Enable host query task?";
	case CSR_PSKEY_BCCMD_SECURITY_ACTIVE:
		return "Enable configuration security";
	case CSR_PSKEY_ANA_FREQ:
		return "Crystal frequency";
	case CSR_PSKEY_PIO_PROTECT_MASK:
		return "Access to PIO pins";
	case CSR_PSKEY_PMALLOC_SIZES:
		return "pmalloc sizes array";
	case CSR_PSKEY_UART_BAUD_RATE:
		return "UART Baud rate (pre 18)";
	case CSR_PSKEY_UART_CONFIG:
		return "UART configuration bitfield";
	case CSR_PSKEY_STUB:
		return "Stub";
	case CSR_PSKEY_TXRX_PIO_CONTROL:
		return "TX and RX PIO control";
	case CSR_PSKEY_ANA_RX_LEVEL:
		return "ANA_RX_LVL register initial value";
	case CSR_PSKEY_ANA_RX_FTRIM:
		return "ANA_RX_FTRIM register initial value";
	case CSR_PSKEY_PSBC_DATA_VERSION:
		return "Persistent store version";
	case CSR_PSKEY_PCM0_ATTENUATION:
		return "Volume control on PCM channel 0";
	case CSR_PSKEY_LO_LVL_MAX:
		return "Maximum value of LO level control register";
	case CSR_PSKEY_LO_ADC_AMPL_MIN:
		return "Minimum value of the LO amplitude measured on the ADC";
	case CSR_PSKEY_LO_ADC_AMPL_MAX:
		return "Maximum value of the LO amplitude measured on the ADC";
	case CSR_PSKEY_IQ_TRIM_CHANNEL:
		return "IQ calibration channel";
	case CSR_PSKEY_IQ_TRIM_GAIN:
		return "IQ calibration gain";
	case CSR_PSKEY_IQ_TRIM_ENABLE:
		return "IQ calibration enable";
	case CSR_PSKEY_TX_OFFSET_HALF_MHZ:
		return "Transmit offset";
	case CSR_PSKEY_GBL_MISC_ENABLES:
		return "Global miscellaneous hardware enables";
	case CSR_PSKEY_UART_SLEEP_TIMEOUT:
		return "Time in ms to deep sleep if nothing received";
	case CSR_PSKEY_DEEP_SLEEP_STATE:
		return "Deep sleep state usage";
	case CSR_PSKEY_IQ_ENABLE_PHASE_TRIM:
		return "IQ phase enable";
	case CSR_PSKEY_HCI_HANDLE_FREEZE_PERIOD:
		return "Time for which HCI handle is frozen after link removal";
	case CSR_PSKEY_MAX_FROZEN_HCI_HANDLES:
		return "Maximum number of frozen HCI handles";
	case CSR_PSKEY_PAGETABLE_DESTRUCTION_DELAY:
		return "Delay from freezing buf handle to deleting page table";
	case CSR_PSKEY_IQ_TRIM_PIO_SETTINGS:
		return "IQ PIO settings";
	case CSR_PSKEY_USE_EXTERNAL_CLOCK:
		return "Device uses an external clock";
	case CSR_PSKEY_DEEP_SLEEP_WAKE_CTS:
		return "Exit deep sleep on CTS line activity";
	case CSR_PSKEY_FC_HC2H_FLUSH_DELAY:
		return "Delay from disconnect to flushing HC->H FC tokens";
	case CSR_PSKEY_RX_HIGHSIDE:
		return "Disable the HIGHSIDE bit in ANA_CONFIG";
	case CSR_PSKEY_TX_PRE_LVL:
		return "TX pre-amplifier level";
	case CSR_PSKEY_RX_SINGLE_ENDED:
		return "RX single ended";
	case CSR_PSKEY_TX_FILTER_CONFIG:
		return "TX filter configuration";
	case CSR_PSKEY_CLOCK_REQUEST_ENABLE:
		return "External clock request enable";
	case CSR_PSKEY_RX_MIN_ATTEN:
		return "Minimum attenuation allowed for receiver";
	case CSR_PSKEY_XTAL_TARGET_AMPLITUDE:
		return "Crystal target amplitude";
	case CSR_PSKEY_PCM_MIN_CPU_CLOCK:
		return "Minimum CPU clock speed with PCM port running";
	case CSR_PSKEY_HOST_INTERFACE_PIO_USB:
		return "USB host interface selection PIO line";
	case CSR_PSKEY_CPU_IDLE_MODE:
		return "CPU idle mode when radio is active";
	case CSR_PSKEY_DEEP_SLEEP_CLEAR_RTS:
		return "Deep sleep clears the UART RTS line";
	case CSR_PSKEY_RF_RESONANCE_TRIM:
		return "Frequency trim for IQ and LNA resonant circuits";
	case CSR_PSKEY_DEEP_SLEEP_PIO_WAKE:
		return "PIO line to wake the chip from deep sleep";
	case CSR_PSKEY_DRAIN_BORE_TIMERS:
		return "Energy consumption measurement settings";
	case CSR_PSKEY_DRAIN_TX_POWER_BASE:
		return "Energy consumption measurement settings";
	case CSR_PSKEY_MODULE_ID:
		return "Module serial number";
	case CSR_PSKEY_MODULE_DESIGN:
		return "Module design ID";
	case CSR_PSKEY_MODULE_SECURITY_CODE:
		return "Module security code";
	case CSR_PSKEY_VM_DISABLE:
		return "VM disable";
	case CSR_PSKEY_MOD_MANUF0:
		return "Module manufactuer data 0";
	case CSR_PSKEY_MOD_MANUF1:
		return "Module manufactuer data 1";
	case CSR_PSKEY_MOD_MANUF2:
		return "Module manufactuer data 2";
	case CSR_PSKEY_MOD_MANUF3:
		return "Module manufactuer data 3";
	case CSR_PSKEY_MOD_MANUF4:
		return "Module manufactuer data 4";
	case CSR_PSKEY_MOD_MANUF5:
		return "Module manufactuer data 5";
	case CSR_PSKEY_MOD_MANUF6:
		return "Module manufactuer data 6";
	case CSR_PSKEY_MOD_MANUF7:
		return "Module manufactuer data 7";
	case CSR_PSKEY_MOD_MANUF8:
		return "Module manufactuer data 8";
	case CSR_PSKEY_MOD_MANUF9:
		return "Module manufactuer data 9";
	case CSR_PSKEY_DUT_VM_DISABLE:
		return "VM disable when entering radiotest modes";
	case CSR_PSKEY_USR0:
		return "User configuration data 0";
	case CSR_PSKEY_USR1:
		return "User configuration data 1";
	case CSR_PSKEY_USR2:
		return "User configuration data 2";
	case CSR_PSKEY_USR3:
		return "User configuration data 3";
	case CSR_PSKEY_USR4:
		return "User configuration data 4";
	case CSR_PSKEY_USR5:
		return "User configuration data 5";
	case CSR_PSKEY_USR6:
		return "User configuration data 6";
	case CSR_PSKEY_USR7:
		return "User configuration data 7";
	case CSR_PSKEY_USR8:
		return "User configuration data 8";
	case CSR_PSKEY_USR9:
		return "User configuration data 9";
	case CSR_PSKEY_USR10:
		return "User configuration data 10";
	case CSR_PSKEY_USR11:
		return "User configuration data 11";
	case CSR_PSKEY_USR12:
		return "User configuration data 12";
	case CSR_PSKEY_USR13:
		return "User configuration data 13";
	case CSR_PSKEY_USR14:
		return "User configuration data 14";
	case CSR_PSKEY_USR15:
		return "User configuration data 15";
	case CSR_PSKEY_USR16:
		return "User configuration data 16";
	case CSR_PSKEY_USR17:
		return "User configuration data 17";
	case CSR_PSKEY_USR18:
		return "User configuration data 18";
	case CSR_PSKEY_USR19:
		return "User configuration data 19";
	case CSR_PSKEY_USR20:
		return "User configuration data 20";
	case CSR_PSKEY_USR21:
		return "User configuration data 21";
	case CSR_PSKEY_USR22:
		return "User configuration data 22";
	case CSR_PSKEY_USR23:
		return "User configuration data 23";
	case CSR_PSKEY_USR24:
		return "User configuration data 24";
	case CSR_PSKEY_USR25:
		return "User configuration data 25";
	case CSR_PSKEY_USR26:
		return "User configuration data 26";
	case CSR_PSKEY_USR27:
		return "User configuration data 27";
	case CSR_PSKEY_USR28:
		return "User configuration data 28";
	case CSR_PSKEY_USR29:
		return "User configuration data 29";
	case CSR_PSKEY_USR30:
		return "User configuration data 30";
	case CSR_PSKEY_USR31:
		return "User configuration data 31";
	case CSR_PSKEY_USR32:
		return "User configuration data 32";
	case CSR_PSKEY_USR33:
		return "User configuration data 33";
	case CSR_PSKEY_USR34:
		return "User configuration data 34";
	case CSR_PSKEY_USR35:
		return "User configuration data 35";
	case CSR_PSKEY_USR36:
		return "User configuration data 36";
	case CSR_PSKEY_USR37:
		return "User configuration data 37";
	case CSR_PSKEY_USR38:
		return "User configuration data 38";
	case CSR_PSKEY_USR39:
		return "User configuration data 39";
	case CSR_PSKEY_USR40:
		return "User configuration data 40";
	case CSR_PSKEY_USR41:
		return "User configuration data 41";
	case CSR_PSKEY_USR42:
		return "User configuration data 42";
	case CSR_PSKEY_USR43:
		return "User configuration data 43";
	case CSR_PSKEY_USR44:
		return "User configuration data 44";
	case CSR_PSKEY_USR45:
		return "User configuration data 45";
	case CSR_PSKEY_USR46:
		return "User configuration data 46";
	case CSR_PSKEY_USR47:
		return "User configuration data 47";
	case CSR_PSKEY_USR48:
		return "User configuration data 48";
	case CSR_PSKEY_USR49:
		return "User configuration data 49";
	case CSR_PSKEY_USB_VERSION:
		return "USB specification version number";
	case CSR_PSKEY_USB_DEVICE_CLASS_CODES:
		return "USB device class codes";
	case CSR_PSKEY_USB_VENDOR_ID:
		return "USB vendor identifier";
	case CSR_PSKEY_USB_PRODUCT_ID:
		return "USB product identifier";
	case CSR_PSKEY_USB_MANUF_STRING:
		return "USB manufacturer string";
	case CSR_PSKEY_USB_PRODUCT_STRING:
		return "USB product string";
	case CSR_PSKEY_USB_SERIAL_NUMBER_STRING:
		return "USB serial number string";
	case CSR_PSKEY_USB_CONFIG_STRING:
		return "USB configuration string";
	case CSR_PSKEY_USB_ATTRIBUTES:
		return "USB attributes bitmap";
	case CSR_PSKEY_USB_MAX_POWER:
		return "USB device maximum power consumption";
	case CSR_PSKEY_USB_BT_IF_CLASS_CODES:
		return "USB Bluetooth interface class codes";
	case CSR_PSKEY_USB_LANGID:
		return "USB language strings supported";
	case CSR_PSKEY_USB_DFU_CLASS_CODES:
		return "USB DFU class codes block";
	case CSR_PSKEY_USB_DFU_PRODUCT_ID:
		return "USB DFU product ID";
	case CSR_PSKEY_USB_PIO_DETACH:
		return "USB detach/attach PIO line";
	case CSR_PSKEY_USB_PIO_WAKEUP:
		return "USB wakeup PIO line";
	case CSR_PSKEY_USB_PIO_PULLUP:
		return "USB D+ pullup PIO line";
	case CSR_PSKEY_USB_PIO_VBUS:
		return "USB VBus detection PIO Line";
	case CSR_PSKEY_USB_PIO_WAKE_TIMEOUT:
		return "Timeout for assertion of USB PIO wake signal";
	case CSR_PSKEY_USB_PIO_RESUME:
		return "PIO signal used in place of bus resume";
	case CSR_PSKEY_USB_BT_SCO_IF_CLASS_CODES:
		return "USB Bluetooth SCO interface class codes";
	case CSR_PSKEY_USB_SUSPEND_PIO_LEVEL:
		return "USB PIO levels to set when suspended";
	case CSR_PSKEY_USB_SUSPEND_PIO_DIR:
		return "USB PIO I/O directions to set when suspended";
	case CSR_PSKEY_USB_SUSPEND_PIO_MASK:
		return "USB PIO lines to be set forcibly in suspend";
	case CSR_PSKEY_USB_ENDPOINT_0_MAX_PACKET_SIZE:
		return "The maxmimum packet size for USB endpoint 0";
	case CSR_PSKEY_USB_CONFIG:
		return "USB config params for new chips (>bc2)";
	case CSR_PSKEY_RADIOTEST_ATTEN_INIT:
		return "Radio test initial attenuator";
	case CSR_PSKEY_RADIOTEST_FIRST_TRIM_TIME:
		return "IQ first calibration period in test";
	case CSR_PSKEY_RADIOTEST_SUBSEQUENT_TRIM_TIME:
		return "IQ subsequent calibration period in test";
	case CSR_PSKEY_RADIOTEST_LO_LVL_TRIM_ENABLE:
		return "LO_LVL calibration enable";
	case CSR_PSKEY_RADIOTEST_DISABLE_MODULATION:
		return "Disable modulation during radiotest transmissions";
	case CSR_PSKEY_RFCOMM_FCON_THRESHOLD:
		return "RFCOMM aggregate flow control on threshold";
	case CSR_PSKEY_RFCOMM_FCOFF_THRESHOLD:
		return "RFCOMM aggregate flow control off threshold";
	case CSR_PSKEY_IPV6_STATIC_ADDR:
		return "Static IPv6 address";
	case CSR_PSKEY_IPV4_STATIC_ADDR:
		return "Static IPv4 address";
	case CSR_PSKEY_IPV6_STATIC_PREFIX_LEN:
		return "Static IPv6 prefix length";
	case CSR_PSKEY_IPV6_STATIC_ROUTER_ADDR:
		return "Static IPv6 router address";
	case CSR_PSKEY_IPV4_STATIC_SUBNET_MASK:
		return "Static IPv4 subnet mask";
	case CSR_PSKEY_IPV4_STATIC_ROUTER_ADDR:
		return "Static IPv4 router address";
	case CSR_PSKEY_MDNS_NAME:
		return "Multicast DNS name";
	case CSR_PSKEY_FIXED_PIN:
		return "Fixed PIN";
	case CSR_PSKEY_MDNS_PORT:
		return "Multicast DNS port";
	case CSR_PSKEY_MDNS_TTL:
		return "Multicast DNS TTL";
	case CSR_PSKEY_MDNS_IPV4_ADDR:
		return "Multicast DNS IPv4 address";
	case CSR_PSKEY_ARP_CACHE_TIMEOUT:
		return "ARP cache timeout";
	case CSR_PSKEY_HFP_POWER_TABLE:
		return "HFP power table";
	case CSR_PSKEY_DRAIN_BORE_TIMER_COUNTERS:
		return "Energy consumption estimation timer counters";
	case CSR_PSKEY_DRAIN_BORE_COUNTERS:
		return "Energy consumption estimation counters";
	case CSR_PSKEY_LOOP_FILTER_TRIM:
		return "Trim value to optimise loop filter";
	case CSR_PSKEY_DRAIN_BORE_CURRENT_PEAK:
		return "Energy consumption estimation current peak";
	case CSR_PSKEY_VM_E2_CACHE_LIMIT:
		return "Maximum RAM for caching EEPROM VM application";
	case CSR_PSKEY_FORCE_16MHZ_REF_PIO:
		return "PIO line to force 16 MHz reference to be assumed";
	case CSR_PSKEY_CDMA_LO_REF_LIMITS:
		return "Local oscillator frequency reference limits for CDMA";
	case CSR_PSKEY_CDMA_LO_ERROR_LIMITS:
		return "Local oscillator frequency error limits for CDMA";
	case CSR_PSKEY_CLOCK_STARTUP_DELAY:
		return "Clock startup delay in milliseconds";
	case CSR_PSKEY_DEEP_SLEEP_CORRECTION_FACTOR:
		return "Deep sleep clock correction factor";
	case CSR_PSKEY_TEMPERATURE_CALIBRATION:
		return "Temperature in deg C for a given internal setting";
	case CSR_PSKEY_TEMPERATURE_VS_DELTA_INTERNAL_PA:
		return "Temperature for given internal PA adjustment";
	case CSR_PSKEY_TEMPERATURE_VS_DELTA_TX_PRE_LVL:
		return "Temperature for a given TX_PRE_LVL adjustment";
	case CSR_PSKEY_TEMPERATURE_VS_DELTA_TX_BB:
		return "Temperature for a given TX_BB adjustment";
	case CSR_PSKEY_TEMPERATURE_VS_DELTA_ANA_FTRIM:
		return "Temperature for given crystal trim adjustment";
	case CSR_PSKEY_TEST_DELTA_OFFSET:
		return "Frequency offset applied to synthesiser in test mode";
	case CSR_PSKEY_RX_DYNAMIC_LVL_OFFSET:
		return "Receiver dynamic level offset depending on channel";
	case CSR_PSKEY_TEST_FORCE_OFFSET:
		return "Force use of exact value in PSKEY_TEST_DELTA_OFFSET";
	case CSR_PSKEY_RF_TRAP_BAD_DIVISION_RATIOS:
		return "Trap bad division ratios in radio frequency tables";
	case CSR_PSKEY_RADIOTEST_CDMA_LO_REF_LIMITS:
		return "LO frequency reference limits for CDMA in radiotest";
	case CSR_PSKEY_INITIAL_BOOTMODE:
		return "Initial device bootmode";
	case CSR_PSKEY_ONCHIP_HCI_CLIENT:
		return "HCI traffic routed internally";
	case CSR_PSKEY_RX_ATTEN_BACKOFF:
		return "Receiver attenuation back-off";
	case CSR_PSKEY_RX_ATTEN_UPDATE_RATE:
		return "Receiver attenuation update rate";
	case CSR_PSKEY_SYNTH_TXRX_THRESHOLDS:
		return "Local oscillator tuning voltage limits for tx and rx";
	case CSR_PSKEY_MIN_WAIT_STATES:
		return "Flash wait state indicator";
	case CSR_PSKEY_RSSI_CORRECTION:
		return "RSSI correction factor.";
	case CSR_PSKEY_SCHED_THROTTLE_TIMEOUT:
		return "Scheduler performance control.";
	case CSR_PSKEY_DEEP_SLEEP_USE_EXTERNAL_CLOCK:
		return "Deep sleep uses external 32 kHz clock source";
	case CSR_PSKEY_TRIM_RADIO_FILTERS:
		return "Trim rx and tx radio filters if true.";
	case CSR_PSKEY_TRANSMIT_OFFSET:
		return "Transmit offset in units of 62.5 kHz";
	case CSR_PSKEY_USB_VM_CONTROL:
		return "VM application will supply USB descriptors";
	case CSR_PSKEY_MR_ANA_RX_FTRIM:
		return "Medium rate value for the ANA_RX_FTRIM register";
	case CSR_PSKEY_I2C_CONFIG:
		return "I2C configuration";
	case CSR_PSKEY_IQ_LVL_RX:
		return "IQ demand level for reception";
	case CSR_PSKEY_MR_TX_FILTER_CONFIG:
		return "TX filter configuration used for enhanced data rate";
	case CSR_PSKEY_MR_TX_CONFIG2:
		return "TX filter configuration used for enhanced data rate";
	case CSR_PSKEY_USB_DONT_RESET_BOOTMODE_ON_HOST_RESET:
		return "Don't reset bootmode if USB host resets";
	case CSR_PSKEY_LC_USE_THROTTLING:
		return "Adjust packet selection on packet error rate";
	case CSR_PSKEY_CHARGER_TRIM:
		return "Trim value for the current charger";
	case CSR_PSKEY_CLOCK_REQUEST_FEATURES:
		return "Clock request is tristated if enabled";
	case CSR_PSKEY_TRANSMIT_OFFSET_CLASS1:
		return "Transmit offset / 62.5 kHz for class 1 radios";
	case CSR_PSKEY_TX_AVOID_PA_CLASS1_PIO:
		return "PIO line asserted in class1 operation to avoid PA";
	case CSR_PSKEY_MR_PIO_CONFIG:
		return "PIO line asserted in class1 operation to avoid PA";
	case CSR_PSKEY_UART_CONFIG2:
		return "The UART Sampling point";
	case CSR_PSKEY_CLASS1_IQ_LVL:
		return "IQ demand level for class 1 power level";
	case CSR_PSKEY_CLASS1_TX_CONFIG2:
		return "TX filter configuration used for class 1 tx power";
	case CSR_PSKEY_TEMPERATURE_VS_DELTA_INTERNAL_PA_CLASS1:
		return "Temperature for given internal PA adjustment";
	case CSR_PSKEY_TEMPERATURE_VS_DELTA_EXTERNAL_PA_CLASS1:
		return "Temperature for given internal PA adjustment";
	case CSR_PSKEY_TEMPERATURE_VS_DELTA_TX_PRE_LVL_MR:
		return "Temperature adjustment for TX_PRE_LVL in EDR";
	case CSR_PSKEY_TEMPERATURE_VS_DELTA_TX_BB_MR_HEADER:
		return "Temperature for a given TX_BB in EDR header";
	case CSR_PSKEY_TEMPERATURE_VS_DELTA_TX_BB_MR_PAYLOAD:
		return "Temperature for a given TX_BB in EDR payload";
	case CSR_PSKEY_RX_MR_EQ_TAPS:
		return "Adjust receiver configuration for EDR";
	case CSR_PSKEY_TX_PRE_LVL_CLASS1:
		return "TX pre-amplifier level in class 1 operation";
	case CSR_PSKEY_ANALOGUE_ATTENUATOR:
		return "TX analogue attenuator setting";
	case CSR_PSKEY_MR_RX_FILTER_TRIM:
		return "Trim for receiver used in EDR.";
	case CSR_PSKEY_MR_RX_FILTER_RESPONSE:
		return "Filter response for receiver used in EDR.";
	case CSR_PSKEY_PIO_WAKEUP_STATE:
		return "PIO deep sleep wake up state ";
	case CSR_PSKEY_MR_TX_IF_ATTEN_OFF_TEMP:
		return "TX IF atten off temperature when using EDR.";
	case CSR_PSKEY_LO_DIV_LATCH_BYPASS:
		return "Bypass latch for LO dividers";
	case CSR_PSKEY_LO_VCO_STANDBY:
		return "Use standby mode for the LO VCO";
	case CSR_PSKEY_SLOW_CLOCK_FILTER_SHIFT:
		return "Slow clock sampling filter constant";
	case CSR_PSKEY_SLOW_CLOCK_FILTER_DIVIDER:
		return "Slow clock filter fractional threshold";
	case CSR_PSKEY_USB_ATTRIBUTES_POWER:
		return "USB self powered";
	case CSR_PSKEY_USB_ATTRIBUTES_WAKEUP:
		return "USB responds to wake-up";
	case CSR_PSKEY_DFU_ATTRIBUTES_MANIFESTATION_TOLERANT:
		return "DFU manifestation tolerant";
	case CSR_PSKEY_DFU_ATTRIBUTES_CAN_UPLOAD:
		return "DFU can upload";
	case CSR_PSKEY_DFU_ATTRIBUTES_CAN_DOWNLOAD:
		return "DFU can download";
	case CSR_PSKEY_UART_CONFIG_STOP_BITS:
		return "UART: stop bits";
	case CSR_PSKEY_UART_CONFIG_PARITY_BIT:
		return "UART: parity bit";
	case CSR_PSKEY_UART_CONFIG_FLOW_CTRL_EN:
		return "UART: hardware flow control";
	case CSR_PSKEY_UART_CONFIG_RTS_AUTO_EN:
		return "UART: RTS auto-enabled";
	case CSR_PSKEY_UART_CONFIG_RTS:
		return "UART: RTS asserted";
	case CSR_PSKEY_UART_CONFIG_TX_ZERO_EN:
		return "UART: TX zero enable";
	case CSR_PSKEY_UART_CONFIG_NON_BCSP_EN:
		return "UART: enable BCSP-specific hardware";
	case CSR_PSKEY_UART_CONFIG_RX_RATE_DELAY:
		return "UART: RX rate delay";
	case CSR_PSKEY_UART_SEQ_TIMEOUT:
		return "UART: BCSP ack timeout";
	case CSR_PSKEY_UART_SEQ_RETRIES:
		return "UART: retry limit in sequencing layer";
	case CSR_PSKEY_UART_SEQ_WINSIZE:
		return "UART: BCSP transmit window size";
	case CSR_PSKEY_UART_USE_CRC_ON_TX:
		return "UART: use BCSP CRCs";
	case CSR_PSKEY_UART_HOST_INITIAL_STATE:
		return "UART: initial host state";
	case CSR_PSKEY_UART_HOST_ATTENTION_SPAN:
		return "UART: host attention span";
	case CSR_PSKEY_UART_HOST_WAKEUP_TIME:
		return "UART: host wakeup time";
	case CSR_PSKEY_UART_HOST_WAKEUP_WAIT:
		return "UART: host wakeup wait";
	case CSR_PSKEY_BCSP_LM_MODE:
		return "BCSP link establishment mode";
	case CSR_PSKEY_BCSP_LM_SYNC_RETRIES:
		return "BCSP link establishment sync retries";
	case CSR_PSKEY_BCSP_LM_TSHY:
		return "BCSP link establishment Tshy";
	case CSR_PSKEY_UART_DFU_CONFIG_STOP_BITS:
		return "DFU mode UART: stop bits";
	case CSR_PSKEY_UART_DFU_CONFIG_PARITY_BIT:
		return "DFU mode UART: parity bit";
	case CSR_PSKEY_UART_DFU_CONFIG_FLOW_CTRL_EN:
		return "DFU mode UART: hardware flow control";
	case CSR_PSKEY_UART_DFU_CONFIG_RTS_AUTO_EN:
		return "DFU mode UART: RTS auto-enabled";
	case CSR_PSKEY_UART_DFU_CONFIG_RTS:
		return "DFU mode UART: RTS asserted";
	case CSR_PSKEY_UART_DFU_CONFIG_TX_ZERO_EN:
		return "DFU mode UART: TX zero enable";
	case CSR_PSKEY_UART_DFU_CONFIG_NON_BCSP_EN:
		return "DFU mode UART: enable BCSP-specific hardware";
	case CSR_PSKEY_UART_DFU_CONFIG_RX_RATE_DELAY:
		return "DFU mode UART: RX rate delay";
	case CSR_PSKEY_AMUX_AIO0:
		return "Multiplexer for AIO 0";
	case CSR_PSKEY_AMUX_AIO1:
		return "Multiplexer for AIO 1";
	case CSR_PSKEY_AMUX_AIO2:
		return "Multiplexer for AIO 2";
	case CSR_PSKEY_AMUX_AIO3:
		return "Multiplexer for AIO 3";
	case CSR_PSKEY_LOCAL_NAME_SIMPLIFIED:
		return "Local Name (simplified)";
	case CSR_PSKEY_EXTENDED_STUB:
		return "Extended stub";
	default:
		return "Unknown";
	}
}