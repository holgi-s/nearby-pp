//
// Created by Holger on 17.03.2016.
//

#ifndef UNTITLED_CNEARBYMESSAGE_H
#define UNTITLED_CNEARBYMESSAGE_H

#include <vector>


class CNearbyMessage {

public:

    enum eType {
        Command = 1,
        Acknowledge = 2,
        Event = 3,
    };

    enum eRequest{
        Request = 1,
        Accept = 2,
        Message = 4,
        Ping = 5,
        Quit = 8193,
        MessageUDP = 8194,
    };


    static void dump(const std::vector<uint8_t>& bytes, bool addEndl = true);

    int parse(const std::vector<uint8_t>& bytes);

    bool wantAck() { return mType == Command; };

    int getCommand() { return mCmd; };
    int getSequence() { return mSeq; };

    bool isRequest(){ return mCmd == Request && wantAck(); };
    bool isAccept(){ return mCmd == Accept && wantAck(); };

    bool isMessage() { return (mCmd == Message && wantAck()) || (mCmd == MessageUDP && mType == Event); };
    bool isPing() { return mCmd == Ping && wantAck(); };
    bool isQuit() { return mCmd == Quit; };

    bool isReliable() { return wantAck(); };

    std::vector<uint8_t>& getData() { return mData; };

    std::vector<uint8_t> buildAck();
    std::vector<uint8_t> buildAccept(uint32_t sequenceNumber, const std::vector<uint8_t>& payload);
    std::vector<uint8_t> buildPing(uint32_t sequenceNumber);
    std::vector<uint8_t> buildMessage(uint32_t sequenceNumber, const std::vector<uint8_t>& payload);



    bool getRequestPayload(std::string &remoteDID, std::string &remoteEP,
                           std::string &localEP,
                           std::vector<uint8_t> &payload);

    bool getMessagePayload(std::vector<uint8_t> &message);

    static uint32_t readLong(std::vector<uint8_t>::const_iterator it, bool net2host = true);
    static std::vector<uint8_t> writeLong(uint32_t value);
    static void writeLong(std::vector<uint8_t>::iterator& it, const uint32_t value);
private:

    uint32_t mCmd = 0;
    uint32_t mType = 0;
    uint32_t mSeq = 0;

    std::vector<uint8_t> mData;
};


#endif //UNTITLED_CNEARBYMESSAGE_H
