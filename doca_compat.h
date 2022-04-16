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
 * @file doca_compat.h
 * @page compat
 * @defgroup COMPAT Compatibility Management
 *
 * Lib to define compatibility with current version, define experimental Symbols.
 *
 * To set a Symbol (or specifically a function) as experimental:
 *
 * __DOCA_EXPERIMENTAL
 * int func_declare(int param1, int param2);
 *
 * To remove warnings of experimental compile with "-D DOCA_ALLOW_EXPERIMENTAL_API"
 *
 * @{
 */

#ifndef _DOCA_COMPAT_H_
#define _DOCA_COMPAT_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifndef DOCA_ALLOW_EXPERIMENTAL_API

/**
 * @brief To set a Symbol (or specifically a function) as experimental
 */
#define __DOCA_EXPERIMENTAL \
__attribute__((deprecated("Symbol is defined as experimental"), section(".text.experimental")))

#else

#define __DOCA_EXPERIMENTAL \
__attribute__((section(".text.experimental")))

#endif

#ifdef __cplusplus
}
#endif

/** @} */

#endif
