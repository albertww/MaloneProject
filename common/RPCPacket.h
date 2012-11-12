/*
**	Author: Albert
**	Date: 2012-04-17
**	Description:  the RPC packet is used for translation between network, the packer is used to pack packet
**    and the unpaker is used to unpack packet
*/

#ifndef __RPC_PACKET_H__
#define __RPC_PACKET_H__

//const int MAX_RPC_PACKET_LEN = 1024;
#define MAX_RPC_PACKET_LEN 1024
#define RPC_PACKET_HEADER_LEN 6

extern const char flags[4];

class CRPCPacketHeader
{
public:
	int m_PacketLen;
	char m_Flags;
};

/*  the data format is:
	-------------------------------------------
	| packet_length |start flag |  ... datas ... | end flag |
	-------------------------------------------
	|      4 bytes      | 0xDDEE  |  ...    ...     |  0x5599|
	-------------------------------------------
*/

class CRPCPacket
{
public:
	CRPCPacket();
	CRPCPacket(const char *data, int len);
	virtual ~CRPCPacket();

	int GetPacketLen();
	void SetPacketLen(int len);
	char * GetBuffer();

private:
	const int m_MaxBufferLen;
	int m_PacketLen;
	char m_PacketBuffer[MAX_RPC_PACKET_LEN];
};


// pack data
class CDataPacker
{
public:
	CDataPacker();
	virtual ~CDataPacker();
	// @return, 0 if failed, may be buffer is not big enough, 1 suc 
	int SetBuffer(char *buffer, int len);
	// @return, 0 if failed, may be due to data is beyond the buffer len, 1 suc
	int WriteInt(int val);
	// @return, 0 if failed, may be due to data is beyond the buffer len, 1 suc
	int WriteShort(short val);
	// @return, 0 if failed, may be due to data is beyond the buffer len, 1 suc
	int WriteByte(char c);
	// @return, 0 if failed, may be due to data is beyond the buffer len, 1 suc
	int WriteBinary(const char *data, int count);
	// @return, 0 if failed, else return the data length, this method will finish packing data process,
	// such as write data length, write checksum, and etc
	int Finish();
	int GetPacketLen();
	int IsValid();

private:
	int m_IsValid;
	int m_BufferLen;
	char *m_DataPtr;
	int m_CurrentPos;
	// where to start storage data, must skip packet length and start flag
	int m_StartOffset;
	// data must not beyond the end offset, we should reserve some place for end flag, checksum and etc
	int m_EndOffset;
	int m_PacketLen;
};

// unpack data
class CDataUnPacker
{
public:
	CDataUnPacker();
	virtual ~CDataUnPacker();
	// @return, 0 buffer is not long enough
	// -1 buffer data is not right to compose a packet, 1 suc
	int SetBuffer(char *buffer, int len);
	// @param val, storage the result
	// @return, 0 failed, may be due to data is beyond the buffer len, 1 suc
	int ReadInt(int &val);
	// @param val, storage the result
	// @return, 0 failed, may be due to data is beyond the buffer len, 1 suc
	int ReadShort(short &val);
	// @param val, storage the result
	// @return, 0 failed, may be due to data is beyond the buffer len, 1 suc
	int ReadByte(char &c);
	// @return, 0 failed, may be due to data is beyond the buffer len, 1 suc
	int ReadBinary(char *data, int count);
	// @return, 1 if still have data, 0 read to end
	int HaveData();
	int GetPacketLen();
	int IsValid();
	virtual CRPCPacket * GenPacket();
private:
	int m_IsValid;
	int m_BufferLen;
	char *m_DataPtr;
	int m_CurrentPos;
	// where to start storage data, must skip packet length and start flag
	int m_StartOffset;
	// data must not beyond the end offset, we should reserve some place for end flag, checksum and etc
	int m_EndOffset;
	int m_PacketLen;
};

#endif
