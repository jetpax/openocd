
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "log.h"
#include "imp.h"

#include "target/xpc56.h"


#define CFLASH0_BASE	0x0
#define CFLASH0_CONF	0xC3F88000

#define DFLASH0_BASE	0x800000
#define DFLASH0_CONF 	0xC3F8C000

// SPC564Bxx or SCP56ECxx
//#define CFLASH1_BASE	0x180000
//#define CFLASH1_CONF 	0xC3FB8000


/* xpc56 program functions */

FLASH_BANK_COMMAND_HANDLER(xpc56f_flash_bank_command)
{
	MMM

	if (CMD_ARGC < 6)
		return ERROR_COMMAND_SYNTAX_ERROR;

	//struct xpc56f_flash_bank *xpc56f_info;
	//xpc56f_info = malloc(sizeof(struct xpc56f_flash_bank));
	//bank->driver_priv = xpc56f_info;
	//xpc56f_info->probed = 0;

	return ERROR_OK;
}

static int xpc56f_protect(struct flash_bank *bank, int set, int first, int last)
{
	LOG_INFO("%s", __func__);
	set = !!set;

	assert(last < bank->num_sectors);

	struct target *target = bank->target;
	if (target->state != TARGET_HALTED) {
		LOG_ERROR("Target not halted");
		return ERROR_TARGET_NOT_HALTED;
	}

	uintptr_t conf = (uintptr_t)bank->driver_priv;

	uint32_t change = 0;

	for (int i = first; i <= last; i++) {
		if (bank->sectors[i].is_protected != set) {
			change |= BIT(i);
			bank->sectors[i].is_protected = set;
		}
	}

	if (change) {
		// todo: manage high address space
		uint32_t lml = xpc56_read_u32(target, conf + 0x4);
		uint32_t sll = xpc56_read_u32(target, conf + 0xC);

		printf("protect change %08x %08x -- %04x\n", lml, sll, change);

		if ((lml & BIT(31)) == 0 || (sll & BIT(31)) == 0) {
			xpc56_write_u32(target, conf + 0x4, 0xa1a11111);
			xpc56_write_u32(target, conf + 0xC, 0xc3c33333);
		}

		// write same lml based value in both registers
		xpc56_write_u32(target, conf + 0x4, lml ^ change);
		xpc56_write_u32(target, conf + 0xC, lml ^ change);
	}

	return ERROR_OK;
}

static int xpc56f_erase(struct flash_bank *bank, int first, int last)
{
	struct target *target = bank->target;
	if (target->state != TARGET_HALTED) {
		LOG_ERROR("Target not halted");
		return ERROR_TARGET_NOT_HALTED;
	}

	uint32_t to_erase = 0;
	uint32_t offset = 0;
	uintptr_t conf = (uintptr_t)bank->driver_priv;

	for (int i = first; i <= last; i++) {
		if (bank->sectors[i].is_erased != 1) {
			to_erase |= BIT(i);
			offset = bank->sectors[i].offset;
		}
	}

	if (to_erase) {
		// todo: should test and fail
		xpc56f_protect(bank, 0, first, last);

		xpc56_write_u32(target, conf, 0x4);		// select erase op
		xpc56_write_u32(target, conf + 0x10, to_erase);	// select sectors to erase
		xpc56_write_u32(target, bank->base + offset, 0x12345678);	// interlock write
		xpc56_write_u32(target, conf, 0x5);		// start erasing

		uint32_t mcr;
		uint32_t cnt = 0;
		do {
			mcr = xpc56_read_u32(target, conf);	// check end
		} while ((mcr & 0x400) == 0 && cnt++ < 10000);
		// note: cnt ~= 1500 at clock 10000kHz for 6 sectors/256k

		if ((mcr & 0x600) != 0x600) {
			LOG_ERROR("Erase failed: mcr=0x%x\n", mcr);
			return ERROR_FLASH_OPERATION_FAILED;
		}

		for (int i = first; i <= last; i++)
			bank->sectors[i].is_erased = 1;
	}

	return ERROR_OK;
}


static int xpc56f_write(struct flash_bank *bank, const uint8_t *buffer, uint32_t offset, uint32_t count)
{
	#if 0
	struct target *target = bank->target;
	struct xpc56_common *xpc56 = target->arch_info;
	uint32_t cur_size, cur_buffer_size, page_size;

	if (bank->target->state != TARGET_HALTED) {
		LOG_ERROR("Target not halted");
		return ERROR_TARGET_NOT_HALTED;
	}

	page_size = bank->sectors[0].size;
	if ((offset % page_size) != 0) {
		LOG_WARNING("offset 0x%" PRIx32 " breaks required %" PRIu32 "-byte alignment",
			offset,
			page_size);
		return ERROR_FLASH_DST_BREAKS_ALIGNMENT;
	}

	LOG_DEBUG("offset is 0x%08" PRIx32 "", offset);
	LOG_DEBUG("count is %" PRId32 "", count);

	if (ERROR_OK != xpc56_jtagprg_enterprogmode(xpc56))
		return ERROR_FAIL;

	cur_size = 0;
	while (count > 0) {
		if (count > page_size)
			cur_buffer_size = page_size;
		else
			cur_buffer_size = count;
		xpc56_jtagprg_writeflashpage(xpc56,
			buffer + cur_size,
			cur_buffer_size,
			offset + cur_size,
			page_size);
		count -= cur_buffer_size;
		cur_size += cur_buffer_size;

		keep_alive();
	}

	return xpc56_jtagprg_leaveprogmode(xpc56);
	#endif
	return ERROR_OK;
}

static int xpc56f_probe(struct flash_bank *bank)
{
	struct target *target = bank->target;
	//struct xpc56f_flash_bank *xpc56f_info = bank->driver_priv;
	//struct xpc56_common *xpc56 = target->arch_info;
	//const struct xpc56f_type *xpc56_info = NULL;
	//int i;
	//uint32_t device_id;

	if (target->state != TARGET_HALTED) {
		LOG_ERROR("Target not halted");
		return ERROR_TARGET_NOT_HALTED;
	}

	printf("BANK   %d %x %x # %x %x # %d %p # %d %p # %p\n", bank->bank_number
	, bank->base
	, bank->size
	, bank->erased_value
	, bank->default_padded_value
	, bank->num_sectors
	, bank->sectors
	, bank->num_prot_blocks
	, bank->prot_blocks
	, bank->next
	);

	if (bank->size)
		return ERROR_OK;

	uintptr_t addr;
	if (bank->base == CFLASH0_BASE)
		addr = CFLASH0_CONF;
	else if (bank->base == DFLASH0_BASE)
		addr = DFLASH0_CONF;
	else {
		LOG_ERROR("Base address not matching known configurations");
		return ERROR_FLASH_BANK_INVALID;
	}
	// member highjacking
	bank->driver_priv = (void*) addr;

	uint32_t mcr = xpc56_read_u32(target, addr);
	// total bank size in kb
	const uint16_t MCR_SIZE[] = {128, 256, 512, 1024, 1536, 2048, 64, 96 };
	bank->size = MCR_SIZE[(mcr >> 24) & 7] * 1024;

	printf("MCR %08x   %d   %d \n", mcr, (mcr >> 24) & 7, bank->size);

	mcr = xpc56_read_u32(target, addr);
	printf("MCR %08x   %d   %d \n", mcr, (mcr >> 24) & 7, bank->size);
	mcr = xpc56_read_u32(target, addr);
	printf("MCR %08x   %d   %d \n", mcr, (mcr >> 24) & 7, bank->size);

	// sector cfg in kb for low address space, 0-terminated
	const uint8_t MCR_LAS[][9] = {
		{0},
		{128, 128, 0},
		{32, 16, 16, 32, 32, 128, 0},
		{0},
		{0},
		{16, 16, 16, 16, 16, 16, 0},
		{16, 16, 16, 16, 0},
		{16, 16, 32, 32, 16, 16, 64, 64, 0},
	};
	const uint8_t *las = MCR_LAS[(mcr >> 20) & 7];
	while (las[bank->num_sectors])
		bank->num_sectors++;

	bank->sectors = malloc(sizeof(struct flash_sector) * bank->num_sectors);
	uint32_t offset = 0;
	for (int i = 0; i < bank->num_sectors; i++) {
		bank->sectors[i].offset = offset;
		bank->sectors[i].size = las[i] * 1024;
		offset += bank->sectors[i].size;
		bank->sectors[i].is_erased = -1;
		bank->sectors[i].is_protected = 1;
	}

	// todo: add support for mid and high address space cfg
	assert(offset == bank->size);

	return ERROR_OK;
}

static int xpc56f_auto_probe(struct flash_bank *bank)
{
	MMM
	if (bank->size)
		return ERROR_OK;
	else
		return xpc56f_probe(bank);
}

static int xpc56f_protect_check(struct flash_bank *bank)
{
	LOG_INFO("%s", __func__);
	return ERROR_OK;
}

static int xpc56f_info(struct flash_bank *bank, char *buf, int buf_size)
{
	MMM
	struct target *target = bank->target;

	if (target->state != TARGET_HALTED) {
		LOG_ERROR("Target not halted");
		return ERROR_TARGET_NOT_HALTED;
	}

	snprintf(buf, buf_size, "xpc56 flash dummy info\n");
	return ERROR_OK;
}


COMMAND_HANDLER(xpc56f_handle_dummy_command)
{
	return ERROR_OK;
}

static const struct command_registration xpc56f_exec_command_handlers[] = {
	{
		.name = "dummy",
		.usage = "foo",
		.handler = xpc56f_handle_dummy_command,
		.mode = COMMAND_EXEC,
		.help = "bar",
	},
	COMMAND_REGISTRATION_DONE
};
static const struct command_registration xpc56f_command_handlers[] = {
	{
		.name = "xpc56",
		.mode = COMMAND_ANY,
		.help = "xPC56 flash command group",
		.usage = "",
		.chain = xpc56f_exec_command_handlers,
	},
	COMMAND_REGISTRATION_DONE
};

struct flash_driver xpc56_flash = {
	.name = "xpc56",
	.commands = xpc56f_command_handlers,
	.flash_bank_command = xpc56f_flash_bank_command,
	.erase = xpc56f_erase,
	.protect = xpc56f_protect,
	.write = xpc56f_write,
	.read = default_flash_read,
	.probe = xpc56f_probe,
	.auto_probe = xpc56f_auto_probe,
	.erase_check = default_flash_blank_check,
	.protect_check = xpc56f_protect_check,
	.info = xpc56f_info,
};
