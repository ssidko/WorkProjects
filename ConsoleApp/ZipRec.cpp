#include "ZipRec.h"
#include <assert.h>
#include <list>
#include <vector>
#include <iostream>
#include "WinConsole.h"

WinConsole con;

std::map<std::string, std::string> supported_args = {
	{ "file", "Input file path" },
	{ "out_dir", "Output directory for result" },
	{ "offset", "Input file offset" },
	{ "force_utf8", "File name encoding UTF-8" },
	{ "pwd", "Password" },
};

using CmdLlineArguments = std::map<std::string, std::string>;

void ParseCmdLlineArguments(int argc, char* argv[], CmdLlineArguments &arguments)
{
	std::list<std::string> arg_list;
	for (int i = 0; i < argc; i++) {
		arg_list.emplace_back(argv[i]);
	}

	std::string prefix = "--";
	std::string separator = "=";
	for (auto &arg : arg_list) {		
		if (arg.find(prefix) == 0) {
			std::string arg_name = "";
			std::string arg_value = "";
			size_t separator_pos = arg.find(separator);
			if (separator_pos != std::string::npos) {
				arg_name = std::string(arg, prefix.length(), separator_pos - prefix.length());
				arg_value = std::string(arg, separator_pos + 1, arg.length() - (separator_pos + 1));
			} else {				
				arg_name = std::string(arg, prefix.length(), arg.length() - prefix.length());
			}
			arguments.emplace(std::make_pair(arg_name, arg_value));			
		}
	}
}

bool InitializeParameters(ZipRecParameters &params, CmdLlineArguments &arguments)
{
	bool unk_arguments = false;
	params.Clear();
	for (auto &arg : arguments) {
		if (arg.first == "file") {
			params.file_path = arg.second;		
		} else if (arg.first == "out_dir") {
			params.out_dir = arg.second;
		} else if (arg.first == "offset") {
			try {
				params.offset = std::stoull(arg.second);
			} catch (...) {				
				std::cout << "Invalid argument value: " << arg.second;
				return false;
			}
		} else if (arg.first == "force_utf8") {
			params.force_utf8 = true;
		} else if (arg.first == "pwd") {
			params.password == arg.second;
		} else {
			std::cout << "Skip unknown argument: {" << arg.first << "}" << std::endl;
			unk_arguments = true;
		}	
	}
	if (unk_arguments) {
		std::cout << "For resume press any key";
		::getchar();
	}

	return true;
}

bool IsValidParameters(ZipRecParameters &params)
{
	return params.file_path.size() && params.out_dir.size();
}

void PrintSupportedArguments(std::map<std::string, std::string> &args)
{
	for (auto &arg : args) {
		std::cout << "  " << arg.first << " - " << arg.second << std::endl;
	}
}

void PrintRecoveryParameters(const ZipRecParameters &param)
{
	con.Print("\nRecovery parameters:\n", ConsoleColour::kYelow | ConsoleColour::kIntensity);
	con.Print(" file       = ", ConsoleColour::kWhite | ConsoleColour::kIntensity); con.Print(param.file_path.c_str()); con.Print("\n");
	con.Print(" out_dir    = ", ConsoleColour::kWhite | ConsoleColour::kIntensity); con.Print(param.out_dir.c_str()); con.Print("\n");
	con.Print(" offset     = ", ConsoleColour::kWhite | ConsoleColour::kIntensity); con.Print(std::to_string(param.offset).c_str()); con.Print("\n");
	con.Print(" force_utf8 = ", ConsoleColour::kWhite | ConsoleColour::kIntensity); con.Print(param.force_utf8 ? "true" : "false"); con.Print("\n");
	//con.Print("pwd="); con.Print(param.password.c_str(), ConsoleColour::kWhite | ConsoleColour::kIntensity);
}

int ZipRec_Main(int argc, _TCHAR* argv[])
{
	con.Print("**********************************\n");
	con.Print("*** ---=== ");
	con.Print("Zip Recovery", ConsoleColour::kGreen|ConsoleColour::kIntensity);
	con.Print(" ===--- ***\n");
	con.Print("**********************************\n");

	CmdLlineArguments args;
	ParseCmdLlineArguments(argc, argv, args);

	ZipRecParameters param;
	if (InitializeParameters(param, args) && IsValidParameters(param)) {
		PrintRecoveryParameters(param);
		FileEx archive(argv[1]);
		StartRecovery(param);
	} else {
		PrintSupportedArguments(supported_args);
	}

	::system("pause");

	return 0;
}

int PrepareAndExtract(FileEx &archive, ZipRecParameters &param)
{
	archive.SetPointer(0x00);

	DWORD rw = 0;
	LONGLONG descr_offs = 0;

	BYTE *h_buff = new BYTE[2048];
	LOCAL_FILE_HEADER *header = (LOCAL_FILE_HEADER *)h_buff;
	memset(h_buff, 0x00, 2048);

	if(sizeof(LOCAL_FILE_HEADER) != archive.Read(h_buff, sizeof(LOCAL_FILE_HEADER)))
		return 0;

	archive.Read(&h_buff[sizeof(LOCAL_FILE_HEADER)], header->name_len + header->extra_field_len - 1);

	if ( (header->DataDescriptorPresent()) )
	{
		bool data_descriptor_found = false;
		DWORD descr_sign = DATA_DESCRIPTOR_SIGN;
		while ( (descr_offs = archive.Find((BYTE *)&descr_sign, sizeof(descr_sign))) != -1 )
		{
			DATA_DESCRIPTOR_32 data_descr = {0};
			archive.Read(&data_descr, sizeof(DATA_DESCRIPTOR_32));
				
			if ( descr_offs == (LOCAL_FILE_HEADER_SIZE + header->name_len + header->extra_field_len + data_descr.compr_size ) ) {
				data_descriptor_found = true;
				header->crc32 = data_descr.crc32;
				header->compr_size = data_descr.compr_size;
				header->uncompr_size = data_descr.uncompr_size;
				header->flag &= ~(uint16_t)(1 << 3);
				break;
			} else {
				archive.SetPointer(descr_offs + 1);
			}
		}

		if (!data_descriptor_found) {
			return 0;
		}
	}

	BYTE *ch_buff = new BYTE[2048];
	memset(ch_buff, 0x00, 2048);
	CENTRAL_FILE_HEADER_32 *central_header = (CENTRAL_FILE_HEADER_32 *)ch_buff;

	central_header->signature = CENTRAL_FILE_HEADER_SIGN;
	central_header->ver_made = 0x1F;
	central_header->ver_needed = header->ver_needed;
	central_header->flag = header->flag;
	central_header->compr_method = header->compr_method;
	central_header->time = header->time;
	central_header->date = header->date;
	central_header->crc32 = header->crc32;
	central_header->compr_size = header->compr_size;
	central_header->uncompr_size = header->uncompr_size;
	central_header->name_len = header->name_len;
	memcpy(central_header->file_name, header->file_name, header->name_len);
	DWORD ch_sz = sizeof(CENTRAL_FILE_HEADER_32) - 1 + central_header->comment_len + central_header->extra_field_len + central_header->name_len;

	BYTE *er_buff = new BYTE[2048];
	memset(er_buff, 0x00, 2048);
	END_OF_CDIRECTORY_RECORD_32 *end_record = (END_OF_CDIRECTORY_RECORD_32 *)er_buff;

	end_record->signature = END_CDIRECTORY_RECORD_SIGN;
	end_record->cdir_entries_count = 1;
	end_record->cdir_entries_total_count = 1;
	end_record->cdir_size = ch_sz;
	end_record->cdir_offset = LOCAL_FILE_HEADER_SIZE + header->name_len + header->extra_field_len + header->compr_size;
	end_record->comment_len = 0;

	if (param.force_utf8) {
		header->flag |= (uint16_t)(1 << 11);
	}

	archive.SetPointer(0x00);
	archive.Write(header, sizeof(LOCAL_FILE_HEADER));

	archive.SetPointer(end_record->cdir_offset);
	archive.Write(central_header, ch_sz);
	archive.Write(end_record, sizeof(END_OF_CDIRECTORY_RECORD_32) - 1);
	archive.Close();

	TCHAR cmd_str[1024] = {0};
	//_stprintf_s(cmd_str, 1024, _T("\"C:\\Program Files\\7-Zip\\7z.exe\" x -y -o%s %s"), out_dir, archive->GetName());

	if (header->UTF8Encoding()) {
		_stprintf_s(cmd_str, 1024, _T(".\\7z.exe x -mcu -y -o%s %s"), param.out_dir.c_str(), archive.GetName());
	} else {
		_stprintf_s(cmd_str, 1024, _T(".\\7z.exe x -y -o%s %s"), param.out_dir.c_str(), archive.GetName());
	}
	
	::system(cmd_str);

	delete h_buff;
	delete ch_buff;
	delete er_buff;
	return 0;
}

int PrepareAndExtract(FileEx &archive, const std::string &out_dir)
{


	return 0;
}

bool IsValidLocalFileHeader(LOCAL_FILE_HEADER &header)
{
	if (header.signature != LOCAL_FILE_HEADER_SIGNATURE)
		return false;

	if (!((header.compr_method >= 0) && (header.compr_method <= 19)) && 
		!(header.compr_method == 97) &&
		!(header.compr_method == 98) )
		return false;

	if (header.name_len == 0) {
		return false;
	}

	if (header.extra_field_len && (header.extra_field_len < 5)) {
		return false;
	}

	if (header.DataDescriptorPresent()) {
		if ((header.crc32 != 0) || (header.compr_size != 0) || (header.uncompr_size != 0))
			return false;
	}

	return true;	
}

bool IsValidExtraField(uint8_t *buff, size_t size)
{
	assert(buff);
	assert(size);

#pragma pack(push,1)
	struct ExtraFieldHeader {
		uint16_t header_id;
		uint16_t data_size;
	};
#pragma pack(pop)

	ExtraFieldHeader *hdr = (ExtraFieldHeader *)buff;
	size_t calculated_size = 0;

	while ((calculated_size + sizeof(ExtraFieldHeader)) < size) {
		hdr = (ExtraFieldHeader *)buff[calculated_size];
		if (hdr->header_id && hdr->data_size) {
			if ((calculated_size + sizeof(ExtraFieldHeader) + hdr->data_size) <= size) {
				calculated_size += sizeof(ExtraFieldHeader) + hdr->data_size;
				continue;
			}
		}
		return false;
	}
	return calculated_size == size;
}

LONGLONG FindNextLocalFile(FileEx &archive)
{
	LONGLONG offset = 0;
	DWORD file_sign = LOCAL_FILE_HEADER_SIGNATURE;

	while ((offset = archive.Find((BYTE *)&file_sign, sizeof(file_sign))) != -1)
	{
		LOCAL_FILE_HEADER header;
		archive.Read(&header, sizeof(LOCAL_FILE_HEADER));
		if (IsValidLocalFileHeader(header))
		{
			archive.SetPointer(offset);
			return offset;
		}
		else
			archive.SetPointer(offset+1);
		
	}
	return -1;
}

bool SaveFileFragmentToNewFile(FileEx &src_file, uint64_t offset, uint64_t size, FileEx &new_file)
{
	uint32_t io_block_size = 128 * 512;
	std::vector<uint8_t> buffer(io_block_size, 0x00);
	uint64_t remain = size;
	uint32_t to_read = 0;
	uint32_t readed = 0;

	src_file.SetPointer(offset);
	while (remain) {		
		to_read = static_cast<uint32_t>(remain > buffer.size() ? buffer.size() : remain);
		readed = src_file.Read(buffer.data(), to_read);
		remain -= readed;
		new_file.Write(buffer.data(), readed);
		if (readed == 0) {
			break;
		}				
	}	
	return remain == 0;
}

bool SaveLocalFile(FileEx &archive, uint64_t offset, FileEx &out_file)
{
	std::vector<uint8_t> buffer(LOCAL_FILE_HEADER_SIZE, 0x00);
	LOCAL_FILE_HEADER *file_header = (LOCAL_FILE_HEADER *)buffer.data();

	archive.SetPointer(offset);
	if (LOCAL_FILE_HEADER_SIZE != archive.Read(buffer.data(), LOCAL_FILE_HEADER_SIZE)) {
		return false;
	}

	if (!IsValidLocalFileHeader(*file_header)) {
		return false;	
	}

	size_t var_len = file_header->name_len + file_header->extra_field_len;
	buffer.resize(LOCAL_FILE_HEADER_SIZE + var_len);
	file_header = (LOCAL_FILE_HEADER *)buffer.data();

	if (var_len != archive.Read(&buffer[LOCAL_FILE_HEADER_SIZE], var_len)) {
		return false;
	}

	//if (file_header->extra_field_len) {
	//// Vrify extra_field 
	//
	//}



	return true;
}

int StartRecovery(ZipRecParameters &param)
{
	DWORD rw = 0;
	uint64_t prev_offs = 0;
	uint64_t curr_offs = 0;
	DWORD file_sign = LOCAL_FILE_HEADER_SIGNATURE;

	uint64_t local_file_size = 0;
	uint64_t local_file_offset = 0;
	std::string tmp_file_name = ".\\tmp.zip";

	FileEx archive(param.file_path.c_str());
	if (!archive.Open()) {
		con.Print("ERROR: ", ConsoleColour::kRed);
		con.Print("Couldn't open file \""); con.Print(param.file_path.c_str()); con.Print("\"");
		return -1;
	}

	archive.SetPointer(param.offset);

	if ( (prev_offs = FindNextLocalFile(archive)) != -1 ) {
		archive.SetPointer(prev_offs + 1);
		while ( (curr_offs = FindNextLocalFile(archive)) != -1 ) {
			local_file_size = curr_offs - prev_offs;
			local_file_offset = prev_offs;
			FileEx tmp_file(tmp_file_name.c_str());
			if (tmp_file.Create()) {
				std::cout << endl;
				con.Print("FILE_HEADER at: ", ConsoleColour::kWhite | ConsoleColour::kIntensity);
				con.Print(std::to_string(local_file_offset).c_str(), ConsoleColour::kWhite | ConsoleColour::kIntensity);
				std::cout << endl;

				if (SaveFileFragmentToNewFile(archive, local_file_offset, local_file_size, tmp_file)) {
					PrepareAndExtract(tmp_file, param);
				} else {
					con.Print("ERROR: ", ConsoleColour::kRed);
					con.Print("in SaveFileFragmentToNewFile()\n");
				}			

				prev_offs = curr_offs;
				archive.SetPointer(curr_offs + 1);
			}
		}
	}
	return 0;
}

int TestZipRec(void)
{
	SetConsoleOutputCP(65001);

	ZipRecParameters param = { 0 };
	param.file_path = "E:\\43881\\PASSPORT.zip";
	param.out_dir = "F:\\43881\\v2";
	param.offset = 46992060398UL;
	param.force_utf8 = true;

	return StartRecovery(param);
}
