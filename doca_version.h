/*
 * Copyright (c) 2021 NVIDIA CORPORATION & AFFILIATES, ALL RIGHTS RESERVED.
 *
 * This software product is a proprietary product of NVIDIA CORPORATION &
 * AFFILIATES (the "Company") and all right, title, and interest in and to the
 * software product, including all associated intellectual property rights, are
 * and shall remain exclusively with the Company.
 *
 * This software product is governed by the End User License Agreement
 * provided with the software product.
 *
 */

/**
 * @file doca_version.h
 * @page version
 * @defgroup VERSION Version Management
 *
 * Define functions to get the DOCA version, and compare against it.
 *
 * @{
 */

#ifndef _DOCA_VERSION_H_
#define _DOCA_VERSION_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>

/**
 * @brief Major version number 0-255
 */
#define DOCA_VER_MAJOR 1
/**
 * @brief Minor version number 0-255
 */
#define DOCA_VER_MINOR 2
/**
 * @brief Patch version number 0-999
 */
#define DOCA_VER_PATCH 6

/**
 * @brief Macro of version number for comparisons
 */
#define DOCA_VERSION_NUM(major, minor, patch) ((size_t)((major) << 24 | (minor) << 16 | (patch)))

/**
 * @brief Macro of current version number for comparisons
 */
#define DOCA_CURRENT_VERSION_NUM DOCA_VERSION_NUM(DOCA_VER_MAJOR, DOCA_VER_MINOR, DOCA_VER_PATCH)

/**
 * @brief Return 1 if the version specified is equal to current
 */
#define DOCA_VERSION_EQ_CURRENT(major, minor, patch) \
	(DOCA_VERSION_NUM(major, minor, patch) == DOCA_CURRENT_VERSION_NUM)

/**
 * @brief Return 1 if the version specified is less then or equal to current
 */
#define DOCA_VERSION_LTE_CURRENT(major, minor, patch) \
	(DOCA_VERSION_NUM(major, minor, patch) <= DOCA_CURRENT_VERSION_NUM)

/**
 * @brief Function returning version string
 *
 * @return
 * version string, using the format major.minor.patch
 */
static inline const char *doca_version(void)
{
	static char version[12];

	snprintf(version, sizeof(version), "%d.%d.%03d", DOCA_VER_MAJOR, DOCA_VER_MINOR,
		 DOCA_VER_PATCH);
	return version;
}

#ifdef __cplusplus
}
#endif

/** @} */

#endif
