//
// Created by Sami Dahoux (s.dahoux@emse.fr) on 26/06/2020.
//

#include "run.h"

void RUN_all_help()
{
    int option_idx = 0;
    int cmd_idx = 1;
    xil_printf("Label\t\tDescription\t\t\tOptions\n\r");
    for (; cmd_idx < CMD_COUNT_TYPE; cmd_idx++)
    {
        xil_printf("%s\t\t%s\t\t", CMD_labels[cmd_idx], CMD_descriptions[cmd_idx]);
        for (option_idx = 0; CMD_allowed_options[cmd_idx][option_idx] != CMD_NULL_OPTION; option_idx++)
        {
            xil_printf("-%c %s ",
                       CMD_allowed_options[cmd_idx][option_idx],
                       CMD_opt_type_labels[CMD_allowed_types[cmd_idx][option_idx]]);
        }
        xil_printf("\n\r");
    }
}

void RUN_cmd_help(const CMD_type_t type)
{
    size_t option_idx = 0;
    xil_printf("Label\t\tDescription\t\t\tOptions\n\r");
    xil_printf("%s\t\t%s\t\t", CMD_labels[type], CMD_descriptions[type]);

    for (; CMD_allowed_options[type][option_idx] != CMD_NULL_OPTION; option_idx++)
    {
        xil_printf("-%c %s ",
                   CMD_allowed_options[type][option_idx],
                   CMD_opt_type_labels[CMD_allowed_types[type][option_idx]]);
    }
    xil_printf("\n\r");
}

void RUN_help(const CMD_cmd_t *cmd)
{
    if (cmd == NULL)
    {
        RUN_all_help();
        return;
    }
    size_t option_idx;
    CMD_type_t help_cmd_type;
    if ((option_idx = CMD_find_option((const CMD_opt_t **)cmd->options, 'c')) == CMD_ERR_NOT_FOUND ||
        (CMD_get_type(cmd->options[option_idx]->value.string, &help_cmd_type) == CMD_ERR_NOT_FOUND))
        RUN_all_help();
    else
        RUN_cmd_help(help_cmd_type);
}

void RUN_quit()
{
    xil_printf("*** Exiting ***\n\r");
}

void RUN_tiny_aes(const CMD_cmd_t *cmd, int plain_idx, int cipher_idx, int key_idx)
{
    xil_printf("*** Start tiny AES ***\n\r");
    struct AES_ctx ctx;
    uint8_t key[HEX_BYTES_SIZE], block[HEX_BYTES_SIZE];
    memcpy(key, cmd->options[key_idx]->value.bytes, HEX_BYTES_SIZE);
    xil_printf("Key: ");
    HEX_print_bytes(key);
    xil_printf("\n\r");
    if (plain_idx != CMD_ERR_NOT_FOUND)
    {
        memcpy(block, cmd->options[plain_idx]->value.bytes, HEX_BYTES_SIZE);
        xil_printf("Plain: ");
        HEX_print_bytes(block);
        xil_printf("\n\r");
        AES_init_ctx(&ctx, key);
        AES_ECB_encrypt(&ctx, block);
        xil_printf("Cipher: ");
        HEX_print_bytes(block);
        xil_printf("\n\r");
    }
    else
    {
        memcpy(block, cmd->options[cipher_idx]->value.bytes, HEX_BYTES_SIZE);
        xil_printf("Cipher: ");
        HEX_print_bytes(block);
        xil_printf("\n\r");
        AES_init_ctx_iv(&ctx, key, block);
        AES_ECB_decrypt(&ctx, block);
        xil_printf("Plain: ");
        HEX_print_bytes(block);
        xil_printf("\n\r");
    }
}

void RUN_hw_aes(const CMD_cmd_t *cmd, int plain_idx, int cipher_idx, int key_idx)
{
    xil_printf("*** Start hardware AES ***\n\r");
    uint32_t key[HEX_WORDS_SIZE], block[HEX_WORDS_SIZE];
    HEX_bytes_to_words(cmd->options[key_idx]->value.bytes, key);
    AES_HW_write_key(key);
    AES_HW_read_key(key);
    xil_printf("Key: ");
    HEX_print_words(key);
    xil_printf("\n\r");
    if (plain_idx != CMD_ERR_NOT_FOUND)
    {
        HEX_bytes_to_words(cmd->options[plain_idx]->value.bytes, block);
        AES_HW_clear(AES_HW_ENCRYPT);
        AES_HW_write_input(block);
        AES_HW_read_input(block);
        xil_printf("Plain: ");
        HEX_print_words(block);
        xil_printf("\n\r");
        AES_HW_launch();
        AES_HW_read_output(block);
        xil_printf("Cipher: ");
        HEX_print_words(block);
        xil_printf("\n\r");
    }
    else
    {
        HEX_bytes_to_words(cmd->options[cipher_idx]->value.bytes, block);
        AES_HW_clear(AES_HW_DECRYPT);
        AES_HW_write_input(block);
        AES_HW_read_input(block);
        xil_printf("Cipher: ");
        HEX_print_words(block);
        xil_printf("\n\r");
        AES_HW_launch();
        AES_HW_read_output(block);
        xil_printf("Plain: ");
        HEX_print_words(block);
        xil_printf("\n\r");
    }
}

RUN_status_t RUN_aes(const CMD_cmd_t *cmd)
{
    if (cmd == NULL)
    {
        return RUN_FAILURE;
    }
    const CMD_opt_t **options_ptr = (const CMD_opt_t **)cmd->options;
    int cipher_idx = CMD_find_option(options_ptr, 'c');
    int plain_idx = CMD_find_option(options_ptr, 'p');
    int key_idx = CMD_find_option(options_ptr, 'k');
    int hw_idx = CMD_find_option(options_ptr, 'h');

    if ((cipher_idx != CMD_ERR_NOT_FOUND && plain_idx != CMD_ERR_NOT_FOUND) ||
        (cipher_idx == CMD_ERR_NOT_FOUND && plain_idx == CMD_ERR_NOT_FOUND) ||
        key_idx == CMD_ERR_NOT_FOUND)
    {
        return RUN_FAILURE;
    }

    if (hw_idx == CMD_ERR_NOT_FOUND)
    {
        RUN_tiny_aes(cmd, plain_idx, cipher_idx, key_idx);
    }
    else
    {
        RUN_hw_aes(cmd, plain_idx, cipher_idx, key_idx);
    }
    return RUN_SUCCESS;
}

RUN_status_t RUN_cmd()
{
    char line[CMD_LINE_SIZE], buffer[CMD_LINE_SIZE];
    CMD_cmd_t cmd;
    CMD_err_t error_ret;
    int error_idx;

    CMD_init(&cmd);
    do
    {
        IO_clear_line(line, CMD_LINE_SIZE);
        CMD_clear(&cmd);
        xil_printf(">");
        if (IO_get_line(line, CMD_LINE_SIZE) != IO_SUCCESS)
        {
            fprintf(stderr, "Read error: errno=%d\n\r", errno);
            return RUN_FAILURE;
        }
        strcpy(buffer, line);
        switch ((error_ret = CMD_parse_line(buffer, &cmd)))
        {
        case CMD_ERR_NONE:
            break;
        case CMD_ERR_NOT_FOUND:
            fprintf(stderr, "Unknown command: %s\n\r", strtok(line, " "));
            continue;
        case CMD_ERR_ALLOC:
            fprintf(stderr, "Allocation error: errno=%d\n\r", errno);
            return RUN_FAILURE;
        case CMD_ERR_FORMAT:
            fprintf(stderr, "Invalid format: %s\n\r", line);
            continue;
        default:
            fprintf(stderr, "Unexpected error: %d\n\r", error_ret);
            continue;
        }
        if ((error_idx = CMD_check_options(cmd)) != CMD_ERR_NONE)
        {
            fprintf(stderr, "Invalid option: %c\n\r", cmd.options[error_idx]->label);
            continue;
        }
        switch (cmd.type)
        {
        case CMD_TYPE_NONE:
            break;
        case CMD_TYPE_HELP:
            RUN_help(&cmd);
            break;
        case CMD_TYPE_QUIT:
            RUN_quit();
            return RUN_SUCCESS;
        case CMD_TYPE_AES:
            RUN_aes(&cmd);
            break;
        default:
            fprintf(stderr, "Not implemented: %s\n\r", strtok(line, " "));
            break;
        }
    } while (1);
}
