//
// Created by Holger on 13.04.2016.
//

#ifndef UNTITLED_PROTOBUF_H
#define UNTITLED_PROTOBUF_H

#include <cstdint>
#include <vector>
#include <string>

class CProtoBuf {

public:
//    static CProtoBuf parse(std::vector<char>& data);

    static bool parseMessage(std::vector<uint8_t>::const_iterator& it, int fieldNumber, uint32_t& messageSize);
    static bool parseVarInt(std::vector<uint8_t>::const_iterator& it, int fieldNumber, uint32_t& varInt);

    static void writeMessage(std::vector<uint8_t>& out, int fieldNumber, const std::vector<uint8_t>& data);
    static void writeVarInt(std::vector<uint8_t>& out, int position, uint32_t var);
private:
//    std::vector<char>& dataBuffer;
//    std::vector<char>::const_iterator dataBegin;
//    std::vector<char>::const_iterator dataEnd;


    static bool getWireType(std::vector<uint8_t>::const_iterator& it, int fieldNumber, int wireType, uint32_t& varInt);
    static void getTag(std::vector<uint8_t>::const_iterator& it, int& field_number, int& wire_type);
    static void getVarInt(std::vector<uint8_t>::const_iterator& it, uint32_t& var);

    static void putTag(std::vector<uint8_t>& out, int field_number, int wire_type);
    static void putVarInt(std::vector<uint8_t>& out, uint32_t var);

};


#endif //UNTITLED_PROTOBUF_H
