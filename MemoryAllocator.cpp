#include <iostream>
#include <list>

class IAllocator {
public:
    virtual void* allocate(int size) = 0;
    virtual int free(void* ptr) = 0;        // status code returning -1 if error

    virtual int availableMemory() = 0;
    virtual int totalMemory() = 0; 
};

class Allocator : public IAllocator {
protected:
    struct MemoryBlock {
        void *mBeginAddress;
        void *mNextAddress;
        unsigned int mSize;
        unsigned int mUnused;             // padding

        MemoryBlock(void *beginAddress, int size) {
            mBeginAddress = beginAddress;
            mSize = size;
            mNextAddress = (void*)((uint8_t*)mBeginAddress + size);
        }

        void modifyBlock(int size) {
            mSize = size;
            mNextAddress = (void*)((uint8_t*)mBeginAddress + size);
        }

        MemoryBlock* splitBlock(int usedSize) {
            int remSize = mSize - usedSize;
            void* _nextAddress = (void*)((uint8_t*)mBeginAddress + usedSize + 1);
            MemoryBlock *_newFreeBlock = new MemoryBlock(_nextAddress, remSize);
            mSize = remSize;
            mNextAddress = _nextAddress;
        }
    };
};

class AllocatorImpl : public Allocator {
private:
    static AllocatorImpl *mAllocator;
    AllocatorImpl() {}

    int mTotalSize;
    void *memoryChunk;

    std::list<MemoryBlock*> mFreeList;         // list of addresses where memory is beginnging
    std::list<MemoryBlock*> mUsedList;

    void tryToMergeBlocks(MemoryBlock* memoryBlock) {
        // 1. find in list
        // 2. see if next block is also in free list
        // 3. recusively merge
        // 4. do a second pass since beginning 
    }

public:
    static AllocatorImpl* getAllocator() {
        if (mAllocator == nullptr) {
            AllocatorImpl *_mAllocator = new AllocatorImpl();
            mAllocator = _mAllocator;
        }
        return mAllocator;
    }

    void init(int size) {
        mTotalSize = size;
        unsigned char *_memoryChunk = new unsigned char[size]();
        memoryChunk = (void*)memoryChunk;

        MemoryBlock *_memoryBlock = new MemoryBlock(memoryChunk, size);
        mFreeList.insert(_memoryBlock);
    }

    // init must be called
    // allocation size must be less than total memory reserved
    void* allocate(int size)  {
        if (mAllocator == nullptr || size > mTotalSize)
            return nullptr;
        
        // 1. iterate through memory see if there is a matching or smaller block 
        //      available

        for (auto& _memoryBlock : mFreeList) {
            // 1. remove from free list
            if (_memoryBlock->mSize >= size) {
                mFreeList.remove(_memoryBlock);
            }

            // 2. create a new block and remaining free memory to that block
            MemoryBlock* _memoryBlockFree = _memoryBlock->splitBlock(size);

            // 3. modify block and add to used list block
            mFreeList.insert(_memoryBlockFree);
            mFreeList.remove(_memoryBlock);
            mUsedList.insert(_memoryBlock);

            return _memoryBlockFree->mBeginAddress;
        }
        return nullptr;
    }

    int free(void* address) {
        for (auto &_memorryBlock : mFreeList) {
            if (_memorryBlock->mBeginAddress == address) {
                mUsedList.remove(_memorryBlock);
                mFreeList.insert(_memoryBlock);

                tryToMergeBlocks(_memorryBlock);
            }
        }
        return -1;
    }

};

