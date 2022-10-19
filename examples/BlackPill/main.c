// Target MCU: STM32F401CCU6

#include <stdint.h>
#include <stdio.h>

#include "bsd.h"

int _sbrk; // missing nano.spec requirement when using <stdio.h>
char PRINT_BUF[255] = {0};
//#define printf_dbg(...)
#define printf_dbg(...) { uint8_t len = snprintf(PRINT_BUF, sizeof(PRINT_BUF), __VA_ARGS__); USART_DBG_DataTxGeneric(USART1, PRINT_BUF, len); }
//__attribute__ ((used)) int _write (int fd, char *ptr, int len) { (void)fd; USART_DBG_DataTxGeneric(USART1, ptr, len); return len; }; // nah, direct use of 'printf()' drags along too much garbage

static char *bytes2str(const uint8_t *data, uint16_t data_len, char *buf, size_t buf_size) {
	const char RADIX_SEQ[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	const uint8_t *const data_end = data + data_len;
	char *const buf_beg = buf;
	char *const buf_end = buf + buf_size;

	for(;;) {
		if (data >= data_end) { break; };
		if (buf+3 >= buf_end) { break; };
		*buf++ = RADIX_SEQ[(((*data)>>4) & 0xF)];
		*buf++ = RADIX_SEQ[( (*data++)   & 0xF)];
	};
	*buf = 0;

	return buf_beg;
};

static int w25qxx_test(void) {
	int ret;
	char read_buf[W25QXX_UID_LEN] = {0}; // W25QXX_UID_LEN == 8

	ret = w25qxx_init(&w25qxx_dev);
	if (ret != W25QXX_RET_SUCCESS) { // W25QXX_RET_SUCCESS == 0
		printf_dbg("w25qxx_init(): Failed with code: %02X\r\n", ret);
		return ret;
	};

	ret = w25qxx_get_unique_id(&w25qxx_dev, read_buf);
	if (ret) { printf_dbg("w25qxx_get_unique_id(): Failed with code: %02X (desc: '%s')\r\n", ret, w25qxx_ret2str(ret)); return ret; };
	char uid_str[W25QXX_UID_LEN * 2 + 1] = {0};
	printf_dbg("Chip 1 UID: '%s'\r\n", bytes2str(read_buf, W25QXX_UID_LEN, uid_str, sizeof(uid_str)));

	w25qxx_info_t info;
	ret = w25qxx_get_info(&w25qxx_dev, &info);
	if (ret) { printf_dbg("w25qxx_get_info(): Failed with code: %02X (desc: '%s')\r\n", ret, w25qxx_ret2str(ret)); return ret; };
	printf_dbg(
		"Manufacturer:\t%s\r\n"
		"Dev_type:\t%s\r\n"
		"Dev_cap:\t%u Kbytes\r\n"
		"Interface:\t%s\r\n"
		"Intf_mode:\t%s\r\n"
		"Addr_mode:\t%.2s\r\n"
		//"Supply volt:\t%.1f..%.1f\r\n" // NOTE: 'nano.specs' don't support printing float in minimal config
		"VCC x10:\t%d..%d\r\n"
		"Supply curr:\t%u ma\r\n"
		"Temp_range:\t%d*C..%d*C\r\n"
		"Driver vers:\t%X.%X\r\n\r\n",
		info.manufacturer,
		info.chip,
		w25qxx_get_storage_capacity_bytes(&w25qxx_dev)/1024,
		info.interface,
		info.intf_mode,
		info.addr_mode,
		//info.supply_volt_min, info.supply_volt_max,
		(int)(info.supply_volt_min*10), (int)(info.supply_volt_max*10),
		info.max_current_ma,
		info.temperature_max, info.temperature_min,
		(info.driver_version >> 4) & 0xF, info.driver_version & 0xF
	);

	const unsigned int val_addr = 0x00000000;
	const int val = 0x12345678;

	ret = w25qxx_overwrite_verify(&w25qxx_dev, val_addr, &val, sizeof(val));
	if (ret) { printf_dbg("w25qxx_overwrite_verify(): Failed with code: %02X (desc: '%s')\r\n", ret, w25qxx_ret2str(ret));  return ret; };

	ret = w25qxx_read_fast(&w25qxx_dev, val_addr, read_buf, sizeof(ret));
	if (ret) { printf_dbg("w25qxx_read_fast(): Failed with code: %02X (desc: '%s')\r\n", ret, w25qxx_ret2str(ret));  return ret; };

	ret = *(int *)read_buf;
	printf_dbg("Chip 1 read data: [%u bytes @ 0x%08X]: %X\r\n", sizeof(ret), val_addr, ret);

	return 0;
};

int main(void) {

	board_init();
	printf_dbg("Starting @ %uMHz\r\n", SystemCoreClock/1000000);

	w25qxx_test();

	while(1) { LedTog(LED_BLUE); delay_ms(100); };

	return 0;
};
