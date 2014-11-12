#include "Signature.h"


Signature::Signature(void)
{
}

Signature::~Signature(void)
{
}

SignatureType Signature::WhatIsIt(const char *buffer)
{
	char *byte = (char *)buffer;
	unsigned int *dword = (unsigned int *)buffer;

	if ((dword[0] == 0xE011CFD0)&&(dword[1] == 0xE11AB1A1)) {
		return kStructuredStorage;
	} else if ((byte[0] == 0xFF)&&(byte[1] == 0xD8)&&(byte[2] == 0xFF)) {
		return kJpeg;
	} else if (dword[0] == 0x04034B50) {
		return kZip;
	} else if (dword[0] == 0x21726152) {
		return kRar;
	} else if ((byte[0] == 0x4D)&&(byte[1] == 0x5A)) {
		return kExe;
	} else if (dword[0] == 0x46445025) {
		return kPdf;
	} else if (dword[0] == 0x474E5089) {
		return kPng;
	} else if ((byte[0] == 0x47)&&(byte[1] == 0x49)&&(byte[2] == 0x46)) {
		return kGif;
	} else if (dword[0] == 0x74725C7B) {
		return kRtf;
	} else if (dword[0] == 0x4643534D) {
		return kCab;
	} else if (dword[0] == 0x3143444B) {
		return kKdc;
	} else if (dword[0] == 0x32445052) {
		return kRpt;
	} else if (dword[0] == 0x454D494D) {
		return kMht;
	} else if (dword[0] == 0x00000100) {
		return kMdb;
	} else if ((byte[0] == 0x42)&&(byte[1] == 0x4D)) {
		return kBmp;
	} else if (dword[0] == 0x6275703C) {
		return kHtc;
	} else if (dword[0] == 0x45504154) {
		return kBak;
	} else if ((byte[0] == 0x37)&&(byte[1] == 0x7A)) {
		return k7z;
	} else {
		return kUnknown;
	}
}
