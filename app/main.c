/*
 * Copyright (c) 2024 Golioth, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <semaphore.h>

#include <openssl/evp.h>

#include <golioth/client.h>
#include <golioth/ota.h>

#define TAG "main"

sem_t manifest_sem;

void on_manifest(struct golioth_client *client,
                 const struct golioth_response *response,
                 const char *path,
                 const uint8_t *payload,
                 size_t payload_size,
                 void *arg)
{
    struct golioth_ota_manifest *manifest = arg;

    /* Parse payload into manifest struct */

    golioth_ota_payload_as_manifest(payload, payload_size, manifest);

    /* Notify main thread that we have received a manifest */

    sem_post(&manifest_sem);
}

enum golioth_status receive_block(const struct golioth_ota_component *component,
                                  uint32_t offset,
                                  uint8_t *block_buffer,
                                  size_t block_size,
                                  bool is_last,
                                  void *arg)
{
    /* Update hash with received data */

    EVP_DigestUpdate(arg, block_buffer, block_size);

    return GOLIOTH_OK;
}

int main(void)
{

    /* Retrieve Golioth PSK credentials from environment */

    char *golioth_psk_id = getenv("GOLIOTH_PSK_ID");
    if ((!golioth_psk_id) || strlen(golioth_psk_id) <= 0)
    {
        fprintf(stderr, "PSK ID is not specified.\n");
        return 1;
    }
    char *golioth_psk = getenv("GOLIOTH_PSK");
    if ((!golioth_psk) || strlen(golioth_psk) <= 0)
    {
        fprintf(stderr, "PSK is not specified.\n");
        return 1;
    }

    /* Create Golioth client */

    struct golioth_client_config config = {
        .credentials =
            {
                .auth_type = GOLIOTH_TLS_AUTH_TYPE_PSK,
                .psk =
                    {
                        .psk_id = golioth_psk_id,
                        .psk_id_len = strlen(golioth_psk_id),
                        .psk = golioth_psk,
                        .psk_len = strlen(golioth_psk),
                    },
            },
    };
    struct golioth_client *client = golioth_client_create(&config);
    assert(client);

    /* Initialize manifest received semaphore */

    sem_init(&manifest_sem, 0, 0);

    /* Register for OTA manifest updates */

    struct golioth_ota_manifest manifest = {0};
    golioth_ota_observe_manifest_async(client, on_manifest, &manifest);

    while (1)
    {
        /* Wait until we've received a manifest */

        int err = sem_wait(&manifest_sem);

        /* Don't attempt to process manifest if sem_wait is signaled */

        if (0 != err)
        {
            continue;
        }

        /* Iterate through the components in the manifest */

        for (int i = 0; i < manifest.num_components; i++)
        {
            struct golioth_ota_component artifact = manifest.components[i];
            printf("\nName: %s, Version: %s, Size: %d\n", artifact.package, artifact.version, artifact.size);

            /* Initialize hash context */

            EVP_MD_CTX *sha_ctx = EVP_MD_CTX_new();
            EVP_DigestInit_ex(sha_ctx, EVP_sha256(), NULL);

            /* Download component */

            golioth_ota_download_component(client, &artifact, receive_block, sha_ctx);

            /* Finalize and print hash */

            char md[32];
            EVP_DigestFinal_ex(sha_ctx, md, NULL);
            EVP_MD_CTX_free(sha_ctx);

            printf("Received SHA256: ");
            for (int i = 0; i < 32; i++)
            {
                printf("%02X", md[i]);
            }
            printf("\n");
        }
    }

    return 0;
}
