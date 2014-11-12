#ifndef SIGNATURE_H
#define SIGNATURE_H

// *.tif, *.tiff
// 49 49 2A 00
// 4D 4D 00 2A

// *.htm, *.html
// 3C 21 44 4F 43 54 59 50 45 - "<!DOCTYPE" 
// 3C 68 74 6D 6C - "<html"

enum SignatureType {
	kUnknown,
	kStructuredStorage,
	kJpeg,
	kZip,
	kRar,
	kExe,
	kPdf,
	kPng,
	kGif,
	kRtf,
	kCab,
	kKdc,
	kRpt,
	kMht,
	kMdb,
	kBmp,
	kHtc,
	kBak,
	k7z
};

static const char *signature_description[] = {
	"Unknown signature",
	"Structured Storage (doc, xls, ppt, mst ...)",
	"jpeg",
	"zip",
	"rar",
	"exe, Portable executable (exe, dll ...)",
	"pdf",
	"png",
	"gif",
	"Rtf",
	"cab, MS Cabinet File",
	"kdc",
	"rpt",
	"mht",
	"mdb, MS Access",
	"bmp",
	"htc",
	"bak, MS backup"
	"7z"
} ;

class Signature
{
public:
	Signature(void);
	~Signature(void);
	static SignatureType WhatIsIt(const char *buffer);
};

#endif //SIGNATURE_H

