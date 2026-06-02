#pragma once
#include <stdint.h>

typedef enum {
    AMDSMI_STATUS_SUCCESS = 0,
    AMDSMI_STATUS_FAIL = 1
} amdsmi_status_t;

typedef enum {
    AMDSMI_INIT_AMD_GPUS = 1
} amdsmi_init_flags_t;

typedef enum {
    AMDSMI_TEMPERATURE_EDGE = 0
} amdsmi_temperature_type_t;

typedef void* amdsmi_socket_handle;
typedef void* amdsmi_processor_handle;

typedef struct {
    uint32_t vram_value;
    uint32_t vram_total;
} amdsmi_vram_usage_t;

typedef struct {
    uint32_t gfx_activity;
} amdsmi_engine_usage_t;

inline amdsmi_status_t amdsmi_init(uint64_t flags) { (void)flags; return AMDSMI_STATUS_FAIL; }
inline amdsmi_status_t amdsmi_shut_down() { return AMDSMI_STATUS_SUCCESS; }
inline amdsmi_status_t amdsmi_get_socket_handles(uint32_t* socket_count, amdsmi_socket_handle* sockets) { (void)socket_count; (void)sockets; return AMDSMI_STATUS_FAIL; }
inline amdsmi_status_t amdsmi_get_processor_handles(amdsmi_socket_handle socket, uint32_t* processor_count, amdsmi_processor_handle* processors) { (void)socket; (void)processor_count; (void)processors; return AMDSMI_STATUS_FAIL; }
inline amdsmi_status_t amdsmi_get_gpu_cache_temperature(amdsmi_processor_handle processor, amdsmi_temperature_type_t type, uint64_t* temperature) { (void)processor; (void)type; (void)temperature; return AMDSMI_STATUS_FAIL; }
inline amdsmi_status_t amdsmi_get_gpu_activity_descriptor(amdsmi_processor_handle processor, amdsmi_engine_usage_t* engine) { (void)processor; (void)engine; return AMDSMI_STATUS_FAIL; }
inline amdsmi_status_t amdsmi_get_gpu_vram_usage(amdsmi_processor_handle processor, amdsmi_vram_usage_t* vram) { (void)processor; (void)vram; return AMDSMI_STATUS_FAIL; }
