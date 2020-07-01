/**
 * @file aes_hw.h
 * @author Sami Dahoux (s.dahoux@emse.fr)
 * @brief Driver for the AES IP contained in the test system vivado_aes
 * 
 * In all the following contents, a block refers to a 16-bytes array or a 4-words array, which is the data format for the AES algorithm.
 * The driver API allows encryption and decryption, therefore, the input/ouput refers to either the plain text or the cipher text.
 */

#ifndef SCA_FRAMEWORK_AES_HW_H
#define SCA_FRAMEWORK_AES_HW_H

#include <stdint.h>
#include <unistd.h>
#include "xparameters.h"
#include "xil_io.h"

#define AES_HW_WORD_SIZE 4

#define AES_HW_BASE_ADDR XPAR_SIMPLE_AES_0_S_AXI_BASEADDR

#define AES_HW_DATA_POS_IN_0 0
#define AES_HW_DATA_POS_IN_1 1
#define AES_HW_DATA_POS_IN_2 2
#define AES_HW_DATA_POS_IN_3 3

#define AES_HW_DATA_POS_OUT_0 4
#define AES_HW_DATA_POS_OUT_1 5
#define AES_HW_DATA_POS_OUT_2 6
#define AES_HW_DATA_POS_OUT_3 7

#define AES_HW_DATA_POS_KEY_0 8
#define AES_HW_DATA_POS_KEY_1 9
#define AES_HW_DATA_POS_KEY_2 10
#define AES_HW_DATA_POS_KEY_3 11

#define AES_HW_STATUS_POS_IN 12
#define AES_HW_STATUS_POS_OUT 13

#define AES_HW_STATUS_NULL 0x00000000

#define AES_HW_STATUS_START 0x00000002
#define AES_HW_STATUS_RESET 0x00000001
#define AES_HW_STATUS_INV 0x00000004

#define AES_HW_STATUS_DONE 0x00000001

#define AES_HW_ADDR(pos) (AES_HW_BASE_ADDR + AES_HW_WORD_SIZE * pos)
#define AES_HW_STATUS_SET_1(status, status_reg) (status_reg | status)
#define AES_HW_STATUS_SET_0(status, status_reg) (status_reg & ~status)
#define AES_HW_STATUS_GET(status, status_reg) (status_reg & status)

typedef enum {
    AES_HW_ENCRYPT,
    AES_HW_DECRYPT
} AES_HW_mode_t;

/**
 * @brief Resets the hardware device and set the encryption mode
 * @param mode set encryption or decryption
 */
void AES_HW_clear(AES_HW_mode_t mode);

/**
 * @brief Writes input block data
 * @param bytes input block words
 */
void AES_HW_write_input(const uint32_t *bytes);

/**
 * @brief Writes key block data
 * @param bytes key block words
 */
void AES_HW_write_key(const uint32_t *bytes);

/**
 * @brief Reads input block data
 * @param bytes input block words
 */
void AES_HW_read_input(uint32_t *bytes);

/**
 * @brief Reads key block data
 * @param bytes key block words
 */
void AES_HW_read_key(uint32_t *bytes);

/**
 * @brief Reads outpout block data
 * @param bytes output block words
 */
void AES_HW_read_output(uint32_t *bytes);

/**
 * @brief Starts hardware AES computing and wait until done
 */
void AES_HW_launch();

#endif //SCA_FRAMEWORK_AES_HW_H