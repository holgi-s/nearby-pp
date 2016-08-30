//
// Created by Holger on 17.03.2016.
//

#include <stdint.h>
#include <iostream>
#include <iomanip>

#include "NearbyMessage.h"
#include "ProtoBuf.h"

#ifdef __WIN32__
#include <Winsock2.h>
#else
#include <arpa/inet.h>
#endif


void CNearbyMessage::dump(const std::vector<uint8_t>& bytes, bool addEndl) {

    std::cerr << std::setfill('0') << std::hex ;
    for (auto b : bytes) {
        std::cerr << std::setw(2) << std::setfill('0') << (int)b << " ";
    }
    if(addEndl){
        std::cerr << std::endl;
    }
}

uint32_t CNearbyMessage::readLong(std::vector<uint8_t>::const_iterator it, bool net2host) {
    uint32_t value = 0;
    value |= *it << 24;
    value |= *std::next(it,1) << 16;
    value |= *std::next(it,2) << 8;
    value |= *std::next(it,3);
    return net2host ? ntohl(value) : value;
}

std::vector<uint8_t> CNearbyMessage::writeLong(uint32_t value) {
    uint32_t net = htonl(value);

    std::vector<uint8_t> buffer;
    buffer.reserve(4);

    buffer.push_back( (uint8_t)((net&0xff000000) >> 24));
    buffer.push_back( (uint8_t)((net&0x00ff0000) >> 16));
    buffer.push_back( (uint8_t)((net&0x0000ff00) >>  8));
    buffer.push_back( (uint8_t)((net&0x000000ff)      ));

    return std::move(buffer);
}

void CNearbyMessage::writeLong(std::vector<uint8_t>::iterator& it, const uint32_t value) {

    *it++ = ( (uint8_t)((value&0xff000000) >> 24));
    *it++ = ( (uint8_t)((value&0x00ff0000) >> 16));
    *it++ = ( (uint8_t)((value&0x0000ff00) >>  8));
    *it++ = ( (uint8_t)((value&0x000000ff)      ));
}

int CNearbyMessage::parse(const std::vector<uint8_t>& bytes)
{
    auto it = std::begin(bytes);
    auto dataEnd = it;

    if (bytes.size() > 4) {

        uint32_t hostSize = readLong(it);

        if(bytes.size() >= hostSize+4) {
            std::advance(it, 4);

            //header message
            uint32_t headerSize = 0;
            CProtoBuf::parseMessage(it, 1, headerSize);

                CProtoBuf::parseVarInt(it, 1, mCmd);
                CProtoBuf::parseVarInt(it, 2, mType);
                CProtoBuf::parseVarInt(it, 3, mSeq);

            //payload message
            uint32_t payloadSize = 0;
            CProtoBuf::parseMessage(it, 2, payloadSize);

            dataEnd = std::next(it, payloadSize);
            mData = std::vector<uint8_t>(it, dataEnd);

            return (int)std::distance(std::begin(bytes),dataEnd);
        }
    }

    return 0;
}

bool CNearbyMessage::getRequestPayload(std::string &remoteName, std::string &remoteDevice,
                                       std::string &localDevice,
                                       std::vector<uint8_t> &payload)
{
    std::vector<uint8_t>::const_iterator it = std::begin(mData);

    uint32_t nameSize;
    CProtoBuf::parseMessage(it, 1, nameSize);
    remoteName = std::string(it, std::next(it, nameSize));
    std::advance(it, nameSize);

    uint32_t deviceSize;
    CProtoBuf::parseMessage(it, 2, deviceSize);
    remoteDevice = std::string(it, std::next(it, deviceSize));
    std::advance(it, deviceSize);

    uint32_t localSize;
    CProtoBuf::parseMessage(it, 3, localSize);
    localDevice = std::string(it, std::next(it, localSize));
    std::advance(it, localSize);

    uint32_t payloadSize;
    CProtoBuf::parseMessage(it, 4, payloadSize);
    payload = std::vector<uint8_t>(it, std::next(it, payloadSize));
    std::advance(it, payloadSize);

}

bool CNearbyMessage::getMessagePayload(std::vector<uint8_t> &message)
{
    std::vector<uint8_t>::const_iterator it = std::begin(mData);

    //payload message
    uint32_t dataSize = 0;
    CProtoBuf::parseMessage(it, 1, dataSize);

    message = std::vector<uint8_t >(it, std::next(it, dataSize));
}

std::vector<uint8_t> CNearbyMessage::buildAck() {

    std::vector<uint8_t> ack_header;
    std::vector<uint8_t> ack_data;
    std::vector<uint8_t> ack;

    ack_header.reserve(10);
    ack.reserve(12);

    CProtoBuf::writeVarInt(ack_header, 1, mCmd);
    CProtoBuf::writeVarInt(ack_header, 2, Acknowledge);
    CProtoBuf::writeVarInt(ack_header, 3, mSeq);

    CProtoBuf::writeMessage(ack, 1, ack_header);
    CProtoBuf::writeMessage(ack, 2, ack_data);

    return ack;
}

std::vector<uint8_t> CNearbyMessage::buildAccept(uint32_t sequenceNumber, const std::vector<uint8_t>& payload) {

    std::vector<uint8_t> msg_header;
    std::vector<uint8_t> msg_payload;
    std::vector<uint8_t> msg;

    msg_header.reserve(10);
    msg_payload.reserve(payload.size()+2);
    msg.reserve(12+payload.size()+2);

    CProtoBuf::writeMessage(msg_payload, 1, payload);

    CProtoBuf::writeVarInt(msg_header, 1, Accept);
    CProtoBuf::writeVarInt(msg_header, 2, Command);
    CProtoBuf::writeVarInt(msg_header, 3, sequenceNumber);

    CProtoBuf::writeMessage(msg, 1, msg_header);
    CProtoBuf::writeMessage(msg, 2, msg_payload);

    return std::move(msg);
}

std::vector<uint8_t> CNearbyMessage::buildPing(uint32_t sequenceNumber)
{
    std::vector<uint8_t> msg_header;
    std::vector<uint8_t> msg_data;
    std::vector<uint8_t> msg;

    msg_header.reserve(10);
    msg.reserve(12);

    CProtoBuf::writeVarInt(msg_header, 1, Ping);
    CProtoBuf::writeVarInt(msg_header, 2, Command);
    CProtoBuf::writeVarInt(msg_header, 3, sequenceNumber);

    CProtoBuf::writeMessage(msg, 1, msg_header);
    CProtoBuf::writeMessage(msg, 2, msg_data);

    return std::move(msg);
}

std::vector<uint8_t> CNearbyMessage::buildMessage(uint32_t sequenceNumber, const std::vector<uint8_t>& payload)
{
    std::vector<uint8_t> msg_header;
    std::vector<uint8_t> msg_payload;
    std::vector<uint8_t> msg;

    msg_header.reserve(10);
    msg.reserve(12 + payload.size() + 2);
    msg_payload.reserve(payload.size() + 2);

    CProtoBuf::writeMessage(msg_payload, 1, payload);

    CProtoBuf::writeVarInt(msg_header, 1, Message);
    CProtoBuf::writeVarInt(msg_header, 2, Command);
    CProtoBuf::writeVarInt(msg_header, 3, sequenceNumber);

    CProtoBuf::writeMessage(msg, 1, msg_header);
    CProtoBuf::writeMessage(msg, 2, msg_payload);

    return std::move(msg);
}
