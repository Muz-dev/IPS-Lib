#include "patcher.h"

#define TITLE_ID "3CA12DFAAF9C82DA064D1698DF79CDA1"
#define OFFSET 0x100


int main() {
    const auto patch = new Patcher(TITLE_ID, "output", OFFSET);
    patch->createPatch("mov w0, #1", 0x001f2a2c);
    patch->createPatch("nop", 0x001deae0);
    patch->generatePatch();
}

