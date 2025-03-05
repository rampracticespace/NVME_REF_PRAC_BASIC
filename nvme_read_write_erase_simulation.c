#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#define NAND_PAGE_SIZE 2048    // Typical NAND page size in bytes
#define NAND_BLOCK_SIZE 128    // Number of pages per block
#define NAND_NUM_BLOCKS 1024   // Total number of blocks
#define ECC_CORRECTION_THRESHOLD 5 // Simulate a simple ECC correction threshold

// Define the NAND Flash Memory (simulated)
typedef struct {
    uint8_t data[NAND_PAGE_SIZE];    // Each page has NAND_PAGE_SIZE bytes
    uint8_t spare[NAND_PAGE_SIZE / 8]; // Spare area (for metadata, ECC, etc.)
    int is_dirty; // Flag to indicate if the page has been modified
} NAND_Page;

typedef struct {
    NAND_Page pages[NAND_BLOCK_SIZE];  // Each block contains NAND_BLOCK_SIZE pages
    int is_bad;  // Flag indicating whether this block is marked as bad
} NAND_Block;

typedef struct {
    NAND_Block blocks[NAND_NUM_BLOCKS];  // Total NAND_NUM_BLOCKS blocks in the flash
    int bad_block_map[NAND_NUM_BLOCKS];  // Bad block management
} NAND_Flash;
typedef struct{
     float version;
     char  vendor[10];
 }NAND_version;
// NAND Controller that simulates interacting with the NAND memory
typedef struct {
    NAND_Flash flash;  // Simulated NAND flash memory
    NAND_version atrib;
    int command_queue[1024]; // Queue to hold commands for processing
    int queue_head; // Index for the head of the queue
    int queue_tail; // Index for the tail of the queue
} NAND_Controller;


// Simple ECC simulation function (parity-based for simplicity)
int ecc_check(uint8_t *data) {
    int error_count = 0;
    for (int i = 0; i < NAND_PAGE_SIZE; i++) {
        // Simulating ECC: Count errors based on a simple parity check
        int byte = data[i];
        while (byte) {
            error_count += byte & 1;
            byte >>= 1;
        }
    }
    return error_count;
}

// Function to simulate the correction of errors with ECC
int ecc_correct(uint8_t *data) {
    int errors = ecc_check(data);
    if (errors > ECC_CORRECTION_THRESHOLD) {
        // If errors exceed the threshold, simulate a correction by flipping bits
        for (int i = 0; i < NAND_PAGE_SIZE; i++) {
            data[i] ^= 0xFF;  // Flip bits to simulate correction
        }
        printf("ECC corrected %d errors.\n", errors);
        return 1; // Indicate correction
    }
    return 0; // No correction needed
}

// Function to initialize the NAND controller
void nand_init(NAND_Controller *controller) {
    memset(controller, 0, sizeof(NAND_Controller));  // Initialize the NAND controller to 0
    for (int i = 0; i < NAND_NUM_BLOCKS; i++) {
        controller->flash.blocks[i].is_bad = 0;
        controller->atrib.version  = 0.0;
        strcpy(controller->atrib.vendor,"RAM");
    }
    printf("NAND Controller initialized.\n");
}

void read_nand_atrib(NAND_Controller *controller){
   printf("nand Attributes is:\n");
   printf("_______________________________\n");
   printf("%s\t",controller->atrib.vendor);
   printf("%f\n",controller->atrib.version);
   printf("_______________________________\n");
}

// Function to simulate reading a page from NAND flash
int nand_read(NAND_Controller *controller, uint32_t block, uint32_t page, uint8_t *buffer) {
    if (block >= NAND_NUM_BLOCKS || page >= NAND_BLOCK_SIZE || controller->flash.blocks[block].is_bad) {
        printf("Error: Invalid block or page number or bad block.\n");
        return -1;
    }
    
    NAND_Page *nand_page = &controller->flash.blocks[block].pages[page];
    memset(buffer,0x00,NAND_PAGE_SIZE);
    // Simulate the ECC check during read
    memcpy(buffer, nand_page->data, NAND_PAGE_SIZE);
    if (ecc_check(buffer) > ECC_CORRECTION_THRESHOLD) {
        // If errors are detected, attempt to correct them
        if (!ecc_correct(buffer)) {
            printf("Read failed due to ECC error.\n");
            return -1;
        }
    }
    
    printf("Read data from block %d, page %d.\n", block, page);
    for(int i = 0;i < NAND_PAGE_SIZE;i++){
     printf("[0x%X \t]",nand_page->data[i]);
     if((i%32 == 0)&&(i !=0)){
     printf("\n");
     }
    }
    printf("\n");
    return 0;
}

// Function to simulate writing a page to NAND flash
int nand_write(NAND_Controller *controller, uint32_t block, uint32_t page, const uint8_t *data) {
    if (block >= NAND_NUM_BLOCKS || page >= NAND_BLOCK_SIZE || controller->flash.blocks[block].is_bad) {
        printf("Error: Invalid block or page number or bad block.\n");
        return -1;
    }
    
    // Simulate writing data to the NAND page
    memcpy(controller->flash.blocks[block].pages[page].data, data, NAND_PAGE_SIZE);
    controller->flash.blocks[block].pages[page].is_dirty = 1; // Mark page as dirty
    printf("Written data to block %d, page %d.\n", block, page);
    return 0;
}

// Function to simulate erasing a block (NAND flash typically supports block erasing)
int nand_erase(NAND_Controller *controller, uint32_t block) {
    if (block >= NAND_NUM_BLOCKS) {
        printf("Error: Invalid block number.\n");
        return -1;
    }

    if (controller->flash.blocks[block].is_bad) {
        printf("Error: Block is marked as bad.\n");
        return -1;
    }

    // Simulate erasing the block (clear all data and spare area)
    memset(&controller->flash.blocks[block], 0xFF, sizeof(NAND_Block)); // Typically 0xFF is used for erased state
    controller->flash.blocks[block].is_bad = 0; // Reset bad block flag
    printf("Erased block %d.\n", block);
    return 0;
}

// Function to simulate flushing dirty pages to NAND
int nand_flush(NAND_Controller *controller) {
    printf("Flushing dirty pages...\n");

    for (int block = 0; block < NAND_NUM_BLOCKS; block++) {
        if (controller->flash.blocks[block].is_bad) {
            continue; // Skip bad blocks
        }

        for (int page = 0; page < NAND_BLOCK_SIZE; page++) {
            NAND_Page *nand_page = &controller->flash.blocks[block].pages[page];
            if (nand_page->is_dirty) {
                // Simulate writing dirty page to NAND flash
                // In a real system, this would involve writing data to physical memory
                printf("Flushing block %d, page %d.\n", block, page);
                
                // Clear the dirty flag after the write
                nand_page->is_dirty = 0;
            }
        }
    }

    printf("Flush operation completed.\n");
    return 0;
}

// Function to simulate marking a block as bad (for bad block management)
int nand_mark_bad_block(NAND_Controller *controller, uint32_t block) {
    if (block >= NAND_NUM_BLOCKS) {
        printf("Error: Invalid block number.\n");
        return -1;
    }
    
    controller->flash.blocks[block].is_bad = 1;
    printf("Block %d marked as bad.\n", block);
    return 0;
}

// Simulating NAND controller command queue processing
void process_commands(NAND_Controller *controller) {
    while (controller->queue_head != controller->queue_tail) {
        int command = controller->command_queue[controller->queue_head];
        controller->queue_head = (controller->queue_head + 1) % 1024;
    
        // Simulating command processing (simple example)
        printf("Processing command %d\n", command);
        switch (command) {
            case 1: // Read command
                printf("Read command issued.\n");
                break;
            case 2: // Write command
                printf("Write command issued.\n");
                break;
            case 3: // Erase command
                printf("Erase command issued.\n");
                break;
            case 4: //flush command
                printf("Flush command issued.\n");
                break;
            default:
                printf("Unknown command.\n");
        }
    }
}

int main() {
    NAND_Controller controller;
    nand_init(&controller);
    
    read_nand_atrib(&controller);
    // Example of adding commands to the queue
    controller.command_queue[controller.queue_tail++] = 1;  // Read command
    controller.command_queue[controller.queue_tail++] = 2;  // Write command
    controller.command_queue[controller.queue_tail++] = 3;  // Erase command
    controller.command_queue[controller.queue_tail++] = 4;  // flush command


    // Process the commands in the queue
    process_commands(&controller);

    // Run some simulated NAND operations
    uint8_t write_data[NAND_PAGE_SIZE] = {0x01, 0x02, 0x03, 0x04, 0x05}; // Example data
    nand_write(&controller, 0, 0, write_data);

    uint8_t buffer[NAND_PAGE_SIZE];
    nand_read(&controller, 0, 0, buffer);

    nand_erase(&controller, 0);
    nand_mark_bad_block(&controller, 1);
    //nand_read(&controller, 1, 0, buffer);
    nand_erase(&controller, 1);

    return 0;
}


