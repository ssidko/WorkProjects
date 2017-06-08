#include "zap.h"
#include <assert.h>

bool MZapObjectToNVPairs(const std::vector<char> &mzap_obj, NVPairList &pairs)
{
	mzap_phys_t *mzap = (mzap_phys_t *)mzap_obj.data();
	mzap_ent_phys_t *entry = (mzap_ent_phys_t *)&mzap_obj[sizeof(mzap_phys_t)];

	if (mzap->mz_block_type != ZBT_MICRO) {
		return false;
	}

	size_t max_entries = (mzap_obj.size() - MZAP_HEADER_SIZE) / MZAP_ENT_LEN;

	for(int i = 0; i < max_entries; ++i) {
		entry = &mzap->mz_chunk[i];
		
		size_t name_len = std::strlen(entry->mze_name);
		if (name_len == 0 || name_len >= MZAP_NAME_LEN) {
			continue;
		}

		pairs.emplace(NVPair(entry->mze_value, std::string(entry->mze_name)));
	
	}
	return true;
}

void TraversingMicroZapEntries(const std::vector<char> &mzap_obj, std::function<void(const uint64_t&, const char*)> callback)
{
	mzap_phys_t *mzap = (mzap_phys_t *)mzap_obj.data();
	mzap_ent_phys_t *entry = (mzap_ent_phys_t *)&mzap_obj[sizeof(mzap_phys_t)];

	assert(mzap->mz_block_type == ZBT_MICRO);

	size_t max_entries = (mzap_obj.size() - MZAP_HEADER_SIZE) / MZAP_ENT_LEN;
	for (int i = 0; i < max_entries; ++i) {
		entry = &mzap->mz_chunk[i];

		size_t name_len = std::strlen(entry->mze_name);
		if (name_len == 0 || name_len >= MZAP_NAME_LEN) {
			continue;
		}

		callback(entry->mze_value, entry->mze_name);

	}

}