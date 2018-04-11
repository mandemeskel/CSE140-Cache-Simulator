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

// functions to find bits to breakdown cache addresses
int getOffsetBits();
int getIndexBits();
int getTagBits();

// calculates and returns the offset from this address
int getOffset(address);

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

// handles cache misses by pulling a block from memory and adding it to the passed cache set
int handleMiss(address, cacheSet *);

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

// sets cache parameters for tests
void setCacheParams(int words_in_block, int num_sets, int blocks_in_set);

// checks if the test is true, prints msg and value
int assertTrue(int expected, int actual, char * test_msg);

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

/**
 *  Number bits of the block offset
 **/
int getOffsetBits() {
    return uint_log2(block_size);
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

// calculates and returns the offset from this address
int getOffset(address addrss) {
    return addrss % block_size;
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
    cacheSet * set = NULL;
    return set;
}

// returns the cache block associated with this address
cacheBlock * getCacheBlock(address addrss, cacheSet * set) {
    cacheBlock * block = NULL;
    return block;
}

// returns the word in this block that the address is saved in or null for miss
word * getWord(address addrss, cacheBlock * block) {
    word * data = NULL;
    return data;
}

// handles cache misses by pulling a block from memory and adding it to the passed cache set
int handleMiss(address addrss, cacheSet * set) {
    return 0;
}

// returns a block that we can write data to, find block using LRU or random cache replacement, if no empty block was found in the cache set
cacheBlock * getWriteableBlock(cacheSet * set) {
    cacheBlock * block = NULL;
    return block;
}

// commits block to memory at given address
int writeBlockToMemory(address addrss, cacheBlock * block) {
    return 0;
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

    printf("Tests finished \n");
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

    // setup cache params
    setCacheParams(2, 4, 3);
    
    int offsetbits = getOffsetBits();
    int indexbits = getIndexBits();

    int offset = getOffset(ad);
    int index = getIndex(ad);
    int tag = getTag(ad);

    int passed_tests = 0;
    passed_tests += assertTrue(expected_offsetbits, offsetbits, "testing getOffsetBits()..");
    passed_tests += assertTrue(expected_indexbits, indexbits, "testing getIndexBits()..");

    passed_tests += assertTrue(expected_offset, offset, "testing getOffset()..");
    passed_tests += assertTrue(expected_index, index, "testing getindex()..");
    passed_tests += assertTrue(expected_tag, tag, "testing getTag()..");

    printf("Passed %d/5 tests.\n", passed_tests);

    // reset cache params
    setCacheParams(0, 0, 0);
}

// sets cache parameters for tests
void setCacheParams(int words_in_block, int num_sets, int blocks_in_set) {
    block_size = words_in_block;
    set_count = num_sets;
    assoc = blocks_in_set;
}

// checks if the test is true, prints msg and value
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