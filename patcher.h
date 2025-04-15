#pragma once
#include <iostream>
#include <filesystem>
#include <fstream>
#ifdef _WIN32
#include <winsock2.h>
#else
#include <arpa/inet.h>
#endif


#include "keystone/keystone.h"

struct ipsRecord {
    uint address;
    ushort size;
    unsigned char* data = nullptr;
};

class Patcher {
public:
    Patcher(const char* buildId, const char* output, size_t offset);
    void createPatch(const char* patch_asm, uint patch_addr);
    void generatePatch() const;
    ~Patcher();
private:
    ipsRecord** mIpsRecords = nullptr;
    const char* mTitleId = nullptr;
    const char* mOutputFolder = nullptr;
    int mIpsRecordCount = 0;
    size_t mOffset = 0;
};

inline Patcher::Patcher(const char* buildId, const char* output, size_t startOffset) {
    mTitleId = buildId;
    mIpsRecords = new ipsRecord*[1024];
    mOutputFolder = output;
    mIpsRecordCount = 0;
    mOffset = startOffset;
}

inline void Patcher::createPatch(const char* patch_asm, const uint patch_addr) {
    ks_engine* ks;
    size_t asm_size, patch_count;
    unsigned char* output;

    mIpsRecords[mIpsRecordCount] = new ipsRecord;

    ks_err err = ks_open(KS_ARCH_ARM64, KS_MODE_LITTLE_ENDIAN, &ks);

    if (err != KS_ERR_OK) {
        std::cout << "ks failed to open with error: " << err << std::endl;
    }
    if (ks_asm(ks, patch_asm, 0, &output , &asm_size, &patch_count)) {
        std::cout << "ERROR: ks_asm failed with error:" << ks_errno(ks) << std::endl;
    }

    mIpsRecords[mIpsRecordCount]->data = output;

    for (size_t i = 0; i < asm_size; ++i) {
        std::cout << std::hex << (int)output[i] << " ";
    }
    std::cout << std::endl;


    const ushort length = (ushort)asm_size;
    mIpsRecords[mIpsRecordCount]->size = length;
    mIpsRecords[mIpsRecordCount]->address = htonl(patch_addr + mOffset);

    mIpsRecordCount++;
    ks_close(ks);
}

inline void Patcher::generatePatch() const {
    if (mIpsRecordCount == 0)
      return;

    std::filesystem::create_directories(mOutputFolder);
    const std::filesystem::path filePath = std::filesystem::path(mOutputFolder) / (std::string(mTitleId) + ".ips");
    std::ofstream outputFile(filePath);



    for (size_t i = 0; i < mIpsRecords[mIpsRecordCount -1]->size; ++i) {
        std::cout << std::hex << (int)mIpsRecords[mIpsRecordCount - 1]->data[i] << " ";
    }
    std::cout << std::endl;



    outputFile.write("IPS32", 0x5);
    for (uint i = 0; i < mIpsRecordCount; i++) {
      const ushort length_big = htons(mIpsRecords[i]->size);
      outputFile.write((const char*)&mIpsRecords[i]->address, 0x4);
      outputFile.write((const char*)&length_big, 0x2);
      outputFile.write((const char*)mIpsRecords[i]->data, mIpsRecords[i]->size);
    }
    outputFile.write("EEOF", 0x4);
    outputFile.close();
}

inline Patcher::~Patcher() {
    for (uint i = 0; i < mIpsRecordCount; ++i) {
        ks_free(mIpsRecords[i]->data);
        delete mIpsRecords[i];
    }
    delete[] mIpsRecords;
}
