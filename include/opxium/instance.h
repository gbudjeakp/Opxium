#ifndef OPXIUM_INSTANCE_H
#define OPXIUM_INSTANCE_H

#include "types.h"

OpxiumResult opxium_instance_create(VkInstance* instance, const OpxiumConfig* config);
void opxium_instance_destroy(VkInstance instance);
bool opxium_check_validation_layer_support(void);

extern const char* const VALIDATION_LAYERS[];
extern const uint32_t VALIDATION_LAYER_COUNT;

#endif
