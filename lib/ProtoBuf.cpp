//
// Created by Holger on 13.04.2016.
//

#include "ProtoBuf.h"


bool CProtoBuf::parseMessage(std::vector<uint8_t>::const_iterator &it, int fieldNumber, uint32_t &messageSize) {

    return getWireType(it, fieldNumber, 2, messageSize);
}

bool CProtoBuf::parseVarInt(std::vector<uint8_t>::const_iterator &it, int fieldNumber, uint32_t &varInt) {

    return getWireType(it, fieldNumber, 0, varInt);
}

bool CProtoBuf::getWireType(std::vector<uint8_t>::const_iterator &it, int fieldNumber, int wireType, uint32_t &varInt) {

    int fn = 0, wt = 0;
    getTag(it, fn, wt);
    if (fn == fieldNumber && wt == wireType) {
        getVarInt(it, varInt);
        return true;
    }
    return false;
}

void CProtoBuf::getTag(std::vector<uint8_t>::const_iterator &it, int &field_number, int &wire_type) {

    uint8_t tag = *it++;
    wire_type = (tag & 0x3);
    field_number = tag >> 3;
}

void CProtoBuf::getVarInt(std::vector<uint8_t>::const_iterator &it, uint32_t &var) {

    uint8_t step = 0;
    uint8_t val = 0;
    var = 0;
    do {
        val = *it++;
        var |= (val & 0x7f) << step++ * 7;
    } while (val & 0x80);
}


void CProtoBuf::writeVarInt(std::vector<uint8_t>& out, int position, uint32_t var) {

    putTag(out, position, 0);
    putVarInt(out, var);
}

void CProtoBuf::writeMessage(std::vector<uint8_t>& out, int position, const std::vector<uint8_t>& data) {

    putTag(out,position, 2);
    putVarInt(out, data.size());

    for(auto d : data){
        out.push_back(d);
    }
}

void CProtoBuf::putTag(std::vector<uint8_t>& out, int field_number, int wire_type) {

    uint8_t tag = (field_number<<3) | (wire_type & 0x3);
    out.push_back(tag);
}

void CProtoBuf::putVarInt(std::vector<uint8_t>& out, uint32_t var) {

    uint8_t val = 0;
    uint32_t mask = 0x7f;
    uint8_t step = 0;
    uint32_t remaining = var;

    do {

        val = (var & mask) >> step;
        mask <<= 7;
        step  += 7;
        if(remaining /= 0x7f) {
            val |= 0x80;
        }
        out.push_back(val);

    } while (remaining);
}
