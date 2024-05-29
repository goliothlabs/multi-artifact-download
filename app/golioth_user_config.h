/*
 * Copyright (c) 2024 Golioth, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#define CONFIG_GOLIOTH_AUTO_LOG_TO_CLOUD 1
#define CONFIG_GOLIOTH_COAP_REQUEST_QUEUE_MAX_ITEMS 20
#define CONFIG_GOLIOTH_DEBUG_LOG
#define CONFIG_GOLIOTH_FW_UPDATE

// Ensure that this is at least as large as the maximum number
// of OTA artifacts your device could download
#define CONFIG_GOLIOTH_OTA_MAX_NUM_COMPONENTS 4
