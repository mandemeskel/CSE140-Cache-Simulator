#include "tips.h"

/* The following two functions are defined in util.c */

/* finds the highest 1 bit, and returns its position, else 0xFFFFFFFF */
unsigned int uint_log2(word w);

/* return random int from 0..x-1 */
int randomint(int x);

/*
  This function allows the lfu information to be displayed

    assoc_index - the cache unit that contains the block to be modified
    block_index - the index of the block to be modified

  returns a string representation of the lfu information
 */
char *lfu_to_string(int assoc_index, int block_index)
{
    /* Buffer to print lfu information -- increase size as needed. */
    static char buffer[9];
    sprintf(buffer, "%u", cache[assoc_index].block[block_index].accessCount);

    return buffer;
}

/*
  This function allows the lru information to be displayed

    assoc_index - the cache unit that contains the block to be modified
    block_index - the index of the block to be modified

  returns a string representation of the lru information
 */
char *lru_to_string(int assoc_index, int block_index)
{
    /* Buffer to print lru information -- increase size as needed. */
    static char buffer[9];
    sprintf(buffer, "%u", cache[assoc_index].block[block_index].lru.value);

    return buffer;
}

/*
  This function initializes the lfu information

    assoc_index - the cache unit that contains the block to be modified
    block_number - the index of the block to be modified

*/
void init_lfu(int assoc_index, int block_index)
{
    cache[assoc_index].block[block_index].accessCount = 0;
}

/*
  This function initializes the lru information

    assoc_index - the cache unit that contains the block to be modified
    block_number - the index of the block to be modified

*/
void init_lru(int assoc_index, int block_index)
{
    cache[assoc_index].block[block_index].lru.value = 0;
}

// constants
const int BYTES_IN_WORD = 4;
const int BITS_IN_BYTE = 8;
const int BITS_IN_WORD = BYTES_IN_WORD * BITS_IN_BYTE;

// returns the transferunit mode for accessDRAM()
TransferUnit getTransferUnit();

// converts a word into an array of bytes and puts into the byte array that is passed
void wordToByteArray(word data, byte * bytes);

// converts a byte array into a word, at the given word offset
word byteArrayToWord(byte * bytes, int word_offset);

// inserts a word into a block at the passed word offset
void insertWordIntoBlock(cacheBlock * dst, word data, int word_offset);

// functions to find bits to breakdown cache addresses
int getOffsetBits();
int getIndexBits();
int getTagBits();

// calculates and returns the offset from this address in number of words
int getOffsetInWords(address);

// calculates and returns the offset from this address in number of bytes
int getOffsetInBytes(address);

// calculates and returns the index from this address
int getIndex(address);

// calculates and returns the tag from this address
int getTag(address);

// returns the cache set associated with this address
cacheSet * getCacheSet(address);

// returns the cache block associated with this address
cacheBlock * getCacheBlock(address, cacheSet *);

// returns the word in this block that the address is saved in or null for miss
word * getWord(address, cacheBlock *);

// handles cache misses by pulling a block from memory and adding it to the cache
int handleMiss(address);

// returns a block that we can write data to, find block using LRU or random cache replacement, if no empty block was found in the cache set
cacheBlock * getWriteableBlock(cacheSet *);

// commits block to memory at given address
int writeBlockToMemory(address, cacheBlock *);

// performs a read on this address and returns the word that was found
word * cacheRead(address);

// performs a write on this address
void cacheWrite(address, word *);

// sanity check, runs unit tests on helper functions
void runTests();

// runs tests on functions to get bits, offset, tag, and index
void runBaseTests();

// runs tests for getCacheSet(), getCacheBlock(), and getWord()
void runCacheTests();

// tests getWriteableBlock()
void testGetWriteableBlock();

// tests writeBlockToMemory()
void testWriteBlockToMemory();

// tests handleMiss()
void testHandleMiss();

// tests cacheRead()
void testCacheRead();

// test cacheWrite()
void testCacheWrite();

// sets cache parameters for tests
void setCacheParams(int words_in_block, int num_sets, int blocks_in_set);

// checks if the passed values are equal, prints msg and value
int assertTrue(int expected, int actual, char * test_msg);

// checks if the passed values are NOT equal, prints msg and value
int assertFalse(int expected, int actual, char * test_msg);

/*
  This is the primary function you are filling out,
  You are free to add helper functions if you need them

  @param addr 32-bit byte address
  @param data a pointer to a SINGLE word (32-bits of data)
  @param we   if we == READ, then data used to return
              information back to CPU

              if we == WRITE, then data used to
              update Cache/DRAM
*/
void accessMemory(address addr, word *data, WriteEnable we)
{
    /* Nowhere else to put tests */
    runTests();

    /* Declare variables here */

    /* handle the case of no cache at all - leave this in */
    if (assoc == 0)
    {
        accessDRAM(addr, (byte *)data, WORD_SIZE, we);
        return;
    }

    /*
  You need to read/write between memory (via the accessDRAM() function) and
  the cache (via the cache[] global structure defined in tips.h)

  Remember to read tips.h for all the global variables that tell you the
  cache parameters

  The same code should handle random, LFU, and LRU policies. Test the policy
  variable (see tips.h) to decide which policy to execute. The LRU policy
  should be written such that no two blocks (when their valid bit is VALID)
  will ever be a candidate for replacement. In the case of a tie in the
  least number of accesses for LFU, you use the LRU information to determine
  which block to replace.

  Your cache should be able to support write-through mode (any writes to
  the cache get immediately copied to main memory also) and write-back mode
  (and writes to the cache only gets copied to main memory when the block
  is kicked out of the cache.

  Also, cache should do allocate-on-write. This means, a write operation
  will bring in an entire block if the block is not already in the cache.

  To properly work with the GUI, the code needs to tell the GUI code
  when to redraw and when to flash things. Descriptions of the animation
  functions can be found in tips.h
  */

    /* Start adding code here */

    /* This call to accessDRAM occurs when you modify any of the
     cache parameters. It is provided as a stop gap solution.
     At some point, ONCE YOU HAVE MORE OF YOUR CACHELOGIC IN PLACE,
     THIS LINE SHOULD BE REMOVED.
  */
    accessDRAM(addr, (byte *)data, WORD_SIZE, we);
}

// returns the transferunit mode for accessDRAM()
TransferUnit getTransferUnit() {
    TransferUnit transferUnit;
    int words_in_block = block_size / BYTES_IN_WORD;

    switch(words_in_block) {
        case 2:
            transferUnit = DOUBLEWORD_SIZE;
        break;
        case 3:
            transferUnit = QUADWORD_SIZE;
        break;
        case 4:
            transferUnit = QUADWORD_SIZE;
        break;
        case 5:
            transferUnit = OCTWORD_SIZE;
        break;
    }

    return transferUnit;
}

// converts a word into an array of bytes
void wordToByteArray(word data, byte * bytes) {
    byte temp = 0;
    int shift_amount = 0;
    for(int index = 0; index < BYTES_IN_WORD; index++) {
        
        // get just the byte we want
        shift_amount = BITS_IN_WORD - (BITS_IN_BYTE * index + 1);
        temp = data << shift_amount;

        // right align our byte
        shift_amount = BITS_IN_WORD - BITS_IN_BYTE;
        bytes[index] = temp >> shift_amount;

    }
}

// converts a byte array into a word, at the given word offset
word byteArrayToWord(byte * bytes, int word_offset) {
    word data = 0;
    int offset = word_offset * BYTES_IN_WORD;
    
    // we have to construct a word which is 4 byes, from the offset which is
    // the lowest byte of the word
    unsigned int temp = 0;
    for(int index = 0; index < BYTES_IN_WORD; index++) {
        temp = bytes[offset + index]; // prevents data loss from left shifts 
        data += temp << (BITS_IN_BYTE * index);
    }
    
    return data;
}

// inserts a word into a block at the passed word offset
void insertWordIntoBlock(cacheBlock * dst, word data, int word_offset) {
    int offset = word_offset * BYTES_IN_WORD;
    byte bytes[BYTES_IN_WORD];
    
    wordToByteArray(data, bytes);
    for(int index = 0; index < BYTES_IN_WORD; index++)
        dst->data[offset + index] = bytes[index];
}

/**
 *  Number bits of the block offset
 **/
int getOffsetBits() {
    return uint_log2(block_size / BYTES_IN_WORD);
}

/**
 *  Number bits of the set index
 **/
int getIndexBits() {
    return uint_log2(set_count);
}

/**
 *  Number bits of the block tag
 **/
int getTagBits() {
    return 0;
}

// calculates and returns the offset from this address in number of words
int getOffsetInWords(address addrss) {
    int offset = addrss % (block_size / BYTES_IN_WORD);
    return offset;
}

// calculates and returns the offset from this address in number of bytes
int getOffsetInBytes(address addrss) {
    return getOffsetInWords(addrss) * BYTES_IN_WORD;
}

// calculates and returns the index from this address
int getIndex(address addrss) {
    int temp = (int) addrss / block_size;
    return temp % set_count;
}

// calculates and returns the tag from this address
int getTag(address addrss) {
    return addrss >> (getOffsetBits() + getIndexBits());
}

// returns the cache set associated with this address
cacheSet * getCacheSet(address addrss) {
    int index = getIndex(addrss);
    return &(cache[index]);
}

// returns the cache block associated with this address
cacheBlock * getCacheBlock(address addrss, cacheSet * set) {
    int tag = getTag(addrss);

    for(int index = 0; index < assoc; index++ )
        if(set->block[index].tag == tag) return &(set->block[index]);

    return NULL;
}

// TODO: call free on malloc called here
// returns the word in this block that the address is saved in or null for miss
word * getWord(address addrss, cacheBlock * block) {
    int offset = getOffsetInWords(addrss);
    word * data = malloc(sizeof(word));
    *data = byteArrayToWord(block->data, offset);
    return data;
}

// handles cache misses by pulling a block from memory and adding it to the cache
int handleMiss(address addrss) {
    cacheSet * set = getCacheSet(addrss);
    cacheBlock * block = getWriteableBlock(set);
    TransferUnit transferUnit = getTransferUnit();

    // calculate address and save block to memory
    if(block->valid == VALID && block->dirty == DIRTY) {

        unsigned int index = getIndex(addrss);
        unsigned int tag = block->tag;
        unsigned int offset = 0;
        address old_adrs = offset;
        old_adrs += index << getOffsetBits();
        old_adrs += tag << (getIndexBits() + getOffsetBits());
        // int writeStatus = accessDRAM(old_adrs, block->data, transferUnit, WRITE);
        int writeStatus = writeBlockToMemory(old_adrs, block);

        if(writeStatus != 0) {
            printf("handleMiss() failed to persist block being replaced. \n");
            return -1;
        }

    }

    int status = accessDRAM(addrss, block->data, transferUnit, READ);
    if(status == 0) {

        block->dirty = VIRGIN;
        block->lru.value = 0;
        block->tag = getTag(addrss);
        return 1;

    }

    return -1;
}

// returns a block that we can write data to, find block using LRU or random cache replacement, if no empty block was found in the cache set
cacheBlock * getWriteableBlock(cacheSet * set) {
    int lru_block = 0;

    // look for an empty block
    for(int index = 0; index < assoc; index++) {

        if(set->block[index].valid == INVALID) 
            return &(set->block[index]);

        if(set->block[index].lru.value < set->block[lru_block].lru.value)
            lru_block = index;
    }

    if(policy == LRU) return &(set->block[lru_block]);
    
    int random = randomint(assoc);
    return &(set->block[random]);
}

// commits block to memory at given address
int writeBlockToMemory(address addrss, cacheBlock * block) {
    TransferUnit transferUnit = getTransferUnit();

    int status = accessDRAM(addrss, block->data, transferUnit, WRITE);

    if(status == 0) {
        block->dirty = VIRGIN;
        return 1;
    }

    return -1;
}

// performs a read on this address and returns the word that was found
word * cacheRead(address addrss) {
    word * data = NULL;
    return data;
}

// performs a write on this address
void cacheWrite(address addrss, word * word) {

}

// sanity check, runs unit tests on helper functions
void runTests() {
    printf("Hello tests \n");

    runBaseTests();

    printf("\n\n");

    runCacheTests();

    printf("\n\n");

    testGetWriteableBlock();

    printf("\n\n");

    testWriteBlockToMemory();

    printf("\n\n");

    testHandleMiss();

    printf("\n\n");

    testCacheRead();

    printf("\n\n");

    testCacheWrite();

    printf("Tests finished \n");
}

// tests getWriteableBlock()
void testGetWriteableBlock() {
    printf("Running getWriteableBlock() tests \n");
    int passed_tests = 0;
    int blocks_in_set = 3;

    // setup cache params, 2 words per block, 4 sets, 3-way assoc.
    setCacheParams(2, 4, blocks_in_set);

    unsigned int block0_lru = 34;
    unsigned int block1_lru = 12;
    unsigned int block2_lru = 41;
    address ad = 90;
    
    // setup blocks
    cacheSet * set = getCacheSet(ad);
    set->block[0].valid = INVALID;
    set->block[1].valid = VALID;
    set->block[2].valid = INVALID;
    set->block[0].lru.value = block0_lru;
    set->block[1].lru.value = block1_lru;
    set->block[2].lru.value = block2_lru;

    // test with LRU replacement policy
    policy = LRU;
    cacheBlock * block = getWriteableBlock(set);
    passed_tests += assertTrue(
        (int) &(set->block[0]),
        (int) block,
        "when policy is LRU, getWriteableBlock() should return a block with the lowest LRU"
    );

    set->block[0].valid = VALID;
    set->block[1].valid = VALID;
    set->block[2].valid = VALID;
    block = getWriteableBlock(set);
    passed_tests += assertTrue(
        (int) &(set->block[1]),
        (int) block,
        "when policy is LRU and all blocks are being used, getWriteableBlock() should return a block with the lowest LRU"
    );

    // test with random replacement policy
    policy = RANDOM;
    set->block[0].valid = VALID;
    set->block[1].valid = VALID;
    set->block[2].valid = INVALID;
    block = getWriteableBlock(set);
    passed_tests += assertTrue(
        (int) &(set->block[2]),
        (int) block,
        "when policy is random, getWriteableBlock() should return any block that is not being used"
    );

    set->block[0].valid = VALID;
    set->block[1].valid = VALID;
    set->block[2].valid = VALID;
    block = getWriteableBlock(set);
    passed_tests += assertFalse(
        (int) NULL,
        (int) block,
        "when policy is random and all blocks are being used, getWriteableBlock() should still return a block"
    );

    // reset cache params
    setCacheParams(0, 0, 0);

    printf("Passed %d/4 tests.\n", passed_tests);
}

// tests writeBlockToMemory()
void testWriteBlockToMemory() {
    printf("Running writeBlockToMemory() tests \n");
    int passed_tests = 0;
    int words_in_block = 2;
    int block_index = 0;
    address ad = 10;
    word expected_first_word = 10;
    word expected_second_word = 11;
    byte data[words_in_block * BYTES_IN_WORD];

    // setup cache params, 2 words per block, 4 sets, 3-way assoc.
    setCacheParams(words_in_block, 4, 3);

    // setup block
    cacheSet * set = getCacheSet(ad);
    cacheBlock * block = &(set->block[block_index]);
    block->dirty = DIRTY;
    insertWordIntoBlock(block, expected_first_word, 0);
    insertWordIntoBlock(block, expected_second_word, 1);

    // save and retrieve block
    int success = writeBlockToMemory(ad, block);
    // int waiter = 0;
    // while(waiter < 100000) waiter += 100;
    int status = accessDRAM(ad, data, DOUBLEWORD_SIZE, READ);

    if(status == 0) {
        
        printf("FAILED TO READ accessDRAM()\n");

    }

    word first_word = byteArrayToWord(data, 0);
    word second_word = byteArrayToWord(data, 1);

    passed_tests += assertTrue(
            1, 
            success, 
            "writeBlockToMemory() should return True i.e 1 if succesful"
        );
    passed_tests += assertTrue(
            expected_first_word, 
            first_word, 
            "writeBlockToMemory() should save all the words in the block, testing presence first word"
        );
    passed_tests += assertTrue(
            expected_second_word, 
            second_word, 
            "writeBlockToMemory() should save all the words in the block, testing presence second word"
        );
    passed_tests += assertTrue(
            VIRGIN, 
            block->dirty, 
            "writeBlockToMemory() should set the block to VIRGIN"
        );

    // reset cache params
    setCacheParams(0, 0, 0);

    printf("Passed %d/4 tests.\n", passed_tests);

}

// tests handleMiss(): tests retrieval of data and replacement algos
void testHandleMiss() {
    printf("Running handleMiss() tests \n");
    int passed_tests = 0;

    address ad = 88;
    int expected_tag = 11;
    int expected_lru = 0;
    word expected_word = 88;
    byte expected_bytes[BYTES_IN_WORD];

    wordToByteArray(expected_word, expected_bytes);

    // add data to memory
    accessDRAM(ad, expected_bytes, WORD_SIZE, WRITE);

    // setup cache params, 2 words per block, 4 sets, 3-way assoc.
    setCacheParams(2, 4, 3);

    // should add missing 
    cacheSet * set = getCacheSet(ad);
    int success = handleMiss(ad);

    cacheBlock * block = getCacheBlock(ad, set);
    unsigned int tag = 314567;
    unsigned int lru_value = 314567;
    byte * byte = NULL;
    word word_value = 0;

    if(block != NULL) {
        tag = block->tag;
        lru_value = block->lru.value;
        byte = &(block->data[getOffsetInBytes(ad)]);
        word_value = *(getWord(ad, block));
    }

    passed_tests += assertTrue(1, success, "handleMiss() should return True i.e 1 if succesful");
    passed_tests += assertTrue(expected_tag, tag, "handleMiss() should update the block tag");
    passed_tests += assertTrue(expected_lru, lru_value, "handleMiss() should reset the block's lru");
    passed_tests += assertTrue(expected_word, word_value, "handleMiss() should save the correct data to the cache");

    // reset cache params
    setCacheParams(0, 0, 0);

    printf("Passed %d/4 tests.\n", passed_tests);
}

// tests cacheRead()
void testCacheRead() {
    printf("Running cacheRead() tests \n");
    int passed_tests = 0;



    printf("Passed %d/100000 tests.\n", passed_tests);

}

// test cacheWrite()
void testCacheWrite() {
    printf("Running cacheWrite() tests \n");
    int passed_tests = 0;



    printf("Passed %d/100000 tests.\n", passed_tests);

}

// TODO: add test to check that getWord() returns the correct sized word
// runs tests for getCacheSet(), getCacheBlock(), and getWord()
void runCacheTests() {
    printf("Running cache function tests \n");

    address ad = 180;
    int offset = 0;
    int index = 2;
    int block_id = 0;
    int tag = 22;

    cacheSet * expected_set = &(cache[index]);
    cacheBlock * expected_block = &(expected_set->block[block_id]);
    expected_block->tag = tag; // we need to fool the test into thinking the data is in the cache
    expected_block->data[offset] = ad;
    byte * expected_data = &(expected_block->data[offset]);

    // setup cache params, 2 words per block, 4 sets, 3-way assoc.
    setCacheParams(2, 4, 3);
    
    cacheSet * set = getCacheSet(ad);
    cacheBlock * block = getCacheBlock(ad, set);
    word * data = getWord(ad, block);

    int passed_tests = 0;
    passed_tests += assertTrue((int)expected_set, (int)set, "testing getCacheSet()..");
    passed_tests += assertTrue((int)expected_block, (int)block, "testing getCacheBlock()..");
    passed_tests += assertTrue(*expected_data, *data, "testing getWord()..");

    printf("Passed %d/3 tests.\n", passed_tests);

    // reset cache params
    setCacheParams(0, 0, 0);
}


// runs tests on functions to get bits, offset, tag, and index
void runBaseTests() {
    printf("Running base function tests \n");

    address ad = 180;
    int expected_offsetbits = 1;
    int expected_indexbits = 2;

    int expected_offset = 0;
    int expected_index = 2;
    int expected_tag = 22;

    // setup cache params, 2 words per block, 4 sets, 3-way assoc.
    setCacheParams(2, 4, 3);
    
    int offsetbits = getOffsetBits();
    int indexbits = getIndexBits();

    int offset = getOffsetInBytes(ad);
    int index = getIndex(ad);
    int tag = getTag(ad);

    int passed_tests = 0;
    passed_tests += assertTrue(expected_offsetbits, offsetbits, "testing getOffsetBits()..");
    passed_tests += assertTrue(expected_indexbits, indexbits, "testing getIndexBits()..");

    passed_tests += assertTrue(expected_offset, offset, "testing getOffsetInBytes()..");
    passed_tests += assertTrue(expected_index, index, "testing getindex()..");
    passed_tests += assertTrue(expected_tag, tag, "testing getTag()..");

    printf("Passed %d/5 tests.\n", passed_tests);

    // reset cache params
    setCacheParams(0, 0, 0);
}

// sets cache parameters for tests
void setCacheParams(int words_in_block, int num_sets, int blocks_in_set) {
    block_size = words_in_block * BYTES_IN_WORD; // block size is in bytes
    set_count = num_sets;
    assoc = blocks_in_set;
}

// checks if the passed values are equal, prints msg and value
int assertTrue(int expected, int actual, char * test_msg) {
    int test = expected == actual;
    printf("%s", test_msg);
    printf("\n");

    if(test == 0)
        printf("test FAILED: expected value %d, actual value %d", expected, actual);
    else
        printf("test PASSED!");
    
    printf("\n");

    return test;   
}

// checks if the passed values are NOT equal, prints msg and value
int assertFalse(int expected, int actual, char * test_msg) {
    int test = expected != actual;
    printf("%s", test_msg);
    printf("\n");

    if(test == 0)
        printf("test FAILED: expected value %d and actual value %d are equal", expected, actual);
    else
        printf("test PASSED!");
    
    printf("\n");

    return test;   
}