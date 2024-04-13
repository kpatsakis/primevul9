static void eeprom_readword(struct ql3_adapter *qdev,
			    u32 eepromAddr, unsigned short *value)
{
	fm93c56a_select(qdev);
	fm93c56a_cmd(qdev, (int)FM93C56A_READ, eepromAddr);
	fm93c56a_datain(qdev, value);
	fm93c56a_deselect(qdev);
}