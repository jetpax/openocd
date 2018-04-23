

// HELPERS

#define BIT(x) (1<<(x))
#define CHECK_RETVAL(action)			\
	do {					\
		int __retval = (action);	\
		if (__retval != ERROR_OK) {	\
			LOG_DEBUG("error while calling \"%s\"",	\
				# action);     \
			return __retval;	\
		}				\
	} while (0)


uint32_t xpc56_read_u32(struct target *target, target_addr_t address);
void xpc56_write_u32(struct target *target, target_addr_t address, uint32_t value);
