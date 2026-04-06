#include "skele.h"
#include <stk/stk.h>
#include <stk/stk_log.h>

uint64_t skele_tick_ns = 1000000000ULL / SKELE_DEFAULT_TICK_RATE;

static uint8_t stk_initialized = 0;

const char *skele_version(void) { return "0.0.0"; }

void skele_set_tick_rate(uint8_t rate)
{
	skele_tick_ns = 1000000000ULL / (rate ? rate : SKELE_DEFAULT_TICK_RATE);
}

uint8_t skele_init(void) { return SKELE_INIT_SUCCESS; }

void skele_shutdown(void) {}

void skele_tick(void) {}

uint8_t skele_stk_setup(void)
{
	if (stk_initialized)
		return SKELE_INIT_SUCCESS;

	stk_set_log_prefix("skele");
	stk_set_module_init_fn("skele_mod_init");
	stk_set_module_shutdown_fn("skele_mod_shutdown");
	stk_set_module_name_fn("skele_mod_name");
	stk_set_module_version_fn("skele_mod_ver");
	stk_set_module_description_fn("skele_mod_desc");
	stk_set_module_deps_sym("skele_mod_deps");

	if (stk_init() != STK_INIT_SUCCESS) {
		stk_log(STK_LOG_ERROR, "failed to initialize stk");
		return SKELE_INIT_FAILURE;
	}

	stk_initialized = 1;
	stk_log(STK_LOG_INFO, "skele v%s", SKELE_VERSION);
	return SKELE_INIT_SUCCESS;
}

void skele_stk_teardown(void)
{
	if (!stk_initialized)
		return;
	stk_shutdown();
	stk_initialized = 0;
}
