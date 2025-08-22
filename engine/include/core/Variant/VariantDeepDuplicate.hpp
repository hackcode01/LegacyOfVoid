#ifndef __ENGINE_VARIANT_DEEP_DUPLICATE_HPP__
#define __ENGINE_VARIANT_DEEP_DUPLICATE_HPP__

/**
 * This would be ideally declared nested in Variant, but that would cause circular
 * includes with Array and Dictionary, for instance.
 * Also, this enum is be exposed via Resource.
 */
enum ResourceDeepDuplicateMode {
    RESOURCE_DEEP_DUPLICATE_NONE,
    RESOURCE_DEEP_DUPLICATE_INTERNAL,
    RESOURCE_DEEP_DUPLICATE_ALL,
    RESOURCE_DEEP_DUPLICATE_MAX
};

#endif
