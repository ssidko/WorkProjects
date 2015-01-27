#include "Fat.h"
#include <cstring>



bool fat::IsValidDirEntry(char *buff)
{
	DIR_ENTRY *entry = (DIR_ENTRY *)buff;
	DIR_ENTRY_LONG *entry_long = (DIR_ENTRY_LONG *)buff;

	// Записи с значением первого байта 0x00 или 0xE5 дальше не проверяются.
	// Считается что они пустые.
	unsigned char first_byte = buff[0];
	if ((first_byte == 0x00) || (first_byte == 0xE5)) {
		return true;
	}

	// Два старших бита DIR_ENTRY::attributes  должны быть равны "0".
	if (entry->attributes & (~kAttributesMask)) {
		return false;
	}

	// Если биты атрибута kLongName установлены то остальные биты должны быть сброшены.
	if ((entry->attributes & fat::kLongName) == fat::kLongName) {
		if (entry->attributes & ~fat::kLongName) {
			return false;
		}
	}

	if (entry->attributes == fat::kLongName) {
	//
	// Проверка валидности длинной записи.
	//

		// Валидными значениями нулевого байта является 0x4X или 0x0X (где X - любое число).
		if (!(((entry->name[0] >> 4) == 0x00) || ((entry->name[0] >> 4) == 0x04))) {
			return false;
		}

		return true;
	} else {
	//
	// Проверка валидности короткой записи.
	//

		// Если запись является меткой тома
		if (entry->attributes & fat::kVolumeID) {
			// Все остальные атрибуты должны быть сброшены.
			if ((entry->attributes & ~fat::kVolumeID)) {
				return false;
			}
			// Номер первого кластера должен быть равен - "0".
			if ((entry->first_cluster_lo != 0x00) || (entry->first_cluster_hi != 0x00)) {
				return false;
			}
		}

		// Если запись является каталогом
		if (entry->attributes & fat::kDirectory) {
			// Поле DIR_ENTRY::file_size должно равняться нулю
			if (entry->file_size != 0x00) {
				return false;
			}

			// Проверка на имена первіх двух записей каталога.
			char self_ref_name[11] = {0x2E,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20};
			if (memcmp(entry->name, self_ref_name, 11) == 0x00) {
				return true;
			}
			char parent_ref_name[11] = {0x2E,0x2E,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20};
			if (memcmp(entry->name, parent_ref_name, 11) == 0x00) {
				return true;
			}
		}

		// Проверка на валидность символов в имени (DIR_ENTRY::name).
		for (int x = 0; x < sizeof(entry->name); x++) {
			if (entry->name[x] < 0x20) {
				if ((x == 0x00) && (entry->name[x] == 0x05)) {
					continue;
				}
				return false;
			} else if (entry->name[x] == 0x20) {
				// Символ с кодом 0x20 (пробел) не может быть первым символом имени.
				if (x == 0x00) {
					return false;
				}
			} else {
				if (entry->name[x] == 0x22) {
					return false;
				} else if ((entry->name[x] >= 0x2A) && (entry->name[x] <= 0x2C)) {
					return false;
				} else if ((entry->name[x] >= 0x2E) && (entry->name[x] <= 0x2F)) {
					return false;
				} else if ((entry->name[x] >= 0x3A) && (entry->name[x] <= 0x3F)) {
					return false;
				} else if ((entry->name[x] >= 0x5B) && (entry->name[x] <= 0x5D)) {
					return false;
				} else if (entry->name[x] == 0x7C) {
					return false;
				}
			}
		}
		return true;
	}
}
