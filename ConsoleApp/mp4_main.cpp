#include "mp4_main.h"

static DWORD file_counter = 0;

DWORD Be2Le(DWORD be_dw)
{
	DWORD le_dw;
	BYTE *le = (BYTE *)&le_dw;
	BYTE *be = (BYTE *)&be_dw;
	le[0] = be[3];
	le[1] = be[2];
	le[2] = be[1];
	le[3] = be[0];
	return le_dw;
}

BOOL IsValidMp4Header(MP4_HEADER *hdr)
{
	if ((hdr->ftyp.size == 0x18000000) && (hdr->ftyp.type == MOV_ATOM_TYPE_FILE_TYPE))
		if ((hdr->free.size == 0x08000000) && (hdr->free.type == MOV_ATOM_TYPE_FREE))
			if (hdr->mdat.type == MOV_ATOM_TYPE_MEDIA_DATA)
				return TRUE;
	return FALSE;
}

LONGLONG FindValidHeader(FileEx *file, LONGLONG threshold_offset, BOOL down)
{
	assert(file);

	DWORD rw = 0;
	BYTE cluster[CLUSTER_SIZE] = {0};
	LONGLONG offset = 0;

	file->GetPointer(&offset);
	while((offset >= 0) && (down ? (offset <= threshold_offset) : (offset >= threshold_offset))) {
		file->SetPointer(offset);
		file->Read(cluster, CLUSTER_SIZE);
		if (IsValidMp4Header((MP4_HEADER *)&cluster)) {
			file->SetPointer(offset);
			return offset;
		}
		if (!down) offset -= CLUSTER_SIZE;
		else offset += CLUSTER_SIZE;
	}
	return -1;
}

BOOL ProcessHeader(FileEx *in_file, LONGLONG header_offset, TCHAR *out_dir)
{
	assert(in_file);

	BYTE cluster[CLUSTER_SIZE] = {0};
	BYTE end_signature[] = MP4_END_SIGNATURE;
	LONGLONG end_signature_offset = 0;
	MP4_HEADER *hdr = (MP4_HEADER *)cluster;
	DWORD mdat_size = 0;
	LONGLONG tail_offset;
	LONGLONG tail_size = 0;

	in_file->SetPointer(header_offset);
	if (CLUSTER_SIZE == in_file->Read(cluster, CLUSTER_SIZE)) {
		mdat_size = Be2Le(hdr->mdat.size);
		if (mdat_size) {
			end_signature_offset = in_file->Find(end_signature, strlen((const char *)end_signature));
			if (end_signature_offset != -1) {
				tail_offset = (header_offset + 2*CLUSTER_SIZE);
				tail_size = (end_signature_offset + strlen((const char *)end_signature)) - tail_offset;
				

				int x = 0;
			}
		}
	}
	in_file->SetPointer(header_offset);
	return TRUE;
}

int mp4_main(TCHAR *in_file_name, TCHAR *out_dir)
{
	Mp4File video_file(_T("D:\\Work\\31868\\001_1562_01.MP4"));
	if (video_file.Open()) {
		video_file.CheckStructure();
	}



	LONGLONG header_offset = 0;
	if (in_file_name, out_dir) {
		FileEx file(in_file_name);
		if (file.Open()) {
			while ((header_offset = FindValidHeader(&file, file.GetSize() - 1, TRUE)) >= 0) {
				ProcessHeader(&file, header_offset, out_dir);
				file.SetPointer(header_offset + CLUSTER_SIZE);
			}
		}
	}
	return -1;
}
