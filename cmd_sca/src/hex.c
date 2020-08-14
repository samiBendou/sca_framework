//
// Created by Sami Dahoux (s.dahoux@emse.fr) on 26/06/2020.
//

#include "hex.h"

void HEX_parse_chars(const char *word, uint8_t *bytes, size_t len)
{
    char buffer[3] = "00";
    size_t init_idx =  word[1] != 'x' ? 0 : 2;
    for (size_t idx = init_idx; idx < 2 * len; idx += 2)
    {
        buffer[0] = word[idx];
        buffer[1] = word[idx + 1];
        bytes[(idx - init_idx) >> 1] = strtol(buffer, NULL, 16);
    }
}

void HEX_stringify_bytes(char *str, const uint8_t *bytes, size_t len)
{
    char *ptr = str;
    size_t idx = 0;
    for (; idx < len - 4; idx += 4)
    {
        sprintf(ptr, "%02x%02x%02x%02x ", bytes[idx], bytes[idx + 1], bytes[idx + 2], bytes[idx + 3]);
        ptr += 9;
    }
    sprintf(ptr, "%02x%02x%02x%02x", bytes[idx], bytes[idx + 1], bytes[idx + 2], bytes[idx + 3]);
}

void HEX_stringify_words(char *str, const uint32_t *words, size_t len)
{
    char *ptr = str;
    size_t idx = 0;
    for (; idx < len - 1; idx++)
    {
        sprintf(ptr, "%08lx ", words[idx]);
        ptr += 9;
    }
    sprintf(ptr, "%08lx", words[idx]);
}

void HEX_bytes_to_words(uint32_t *words, const uint8_t *bytes, size_t len)
{
    for (size_t idx = 0; idx < len; idx += 4)
    {
        words[idx / 4] = ((uint32_t)bytes[idx] << 24) +
                         ((uint32_t)bytes[idx + 1] << 16) +
                         ((uint32_t)bytes[idx + 2] << 8) +
                         (uint32_t)bytes[idx + 3];
    }
}

void HEX_words_to_bytes(uint8_t *bytes, const uint32_t *words, size_t len)
{
    size_t words_idx;
    for (size_t idx = 0; idx < len; idx += 4)
    {
        words_idx = idx / 4;
        bytes[idx] = (words[words_idx] >> 24);
        bytes[idx + 1] = (words[words_idx] >> 16) & 0xff;
        bytes[idx + 2] = (words[words_idx] >> 8) & 0xff;
        bytes[idx + 3] = words[words_idx] & 0xff;
    }
}

void HEX_random_words(uint32_t *words, int seed, size_t len)
{
    srand(seed);
    for (size_t idx = 0; idx < len; idx++)
    {
        words[idx] = rand();
    }
}
