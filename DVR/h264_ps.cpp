#include "h264_ps.h"
#include <cstring>

static const uint8_t 	ff_zigzag_direct[64] = {
	0,   1,  8, 16,  9,  2,  3, 10,
	17, 24, 32, 25, 18, 11,  4,  5,
	12, 19, 26, 33, 40, 48, 41, 34,
	27, 20, 13,  6,  7, 14, 21, 28,
	35, 42, 49, 56, 57, 50, 43, 36,
	29, 22, 15, 23, 30, 37, 44, 51,
	58, 59, 52, 45, 38, 31, 39, 46,
	53, 60, 61, 54, 47, 55, 62, 63
};

static const uint8_t 	ff_zigzag_scan[16 + 1] = {
	0 + 0 * 4, 1 + 0 * 4, 0 + 1 * 4, 0 + 2 * 4,
	1 + 1 * 4, 2 + 0 * 4, 3 + 0 * 4, 2 + 1 * 4,
	1 + 2 * 4, 0 + 3 * 4, 1 + 3 * 4, 2 + 2 * 4,
	3 + 1 * 4, 3 + 2 * 4, 2 + 3 * 4, 3 + 3 * 4,
};

static const uint8_t default_scaling4[2][16] = {
	{ 6, 13, 20, 28, 13, 20, 28, 32,
	20, 28, 32, 37, 28, 32, 37, 42 },
	{ 10, 14, 20, 24, 14, 20, 24, 27,
	20, 24, 27, 30, 24, 27, 30, 34 }
};

static const uint8_t default_scaling8[2][64] = {
	{ 6, 10, 13, 16, 18, 23, 25, 27,
	10, 11, 16, 18, 23, 25, 27, 29,
	13, 16, 18, 23, 25, 27, 29, 31,
	16, 18, 23, 25, 27, 29, 31, 33,
	18, 23, 25, 27, 29, 31, 33, 36,
	23, 25, 27, 29, 31, 33, 36, 38,
	25, 27, 29, 31, 33, 36, 38, 40,
	27, 29, 31, 33, 36, 38, 40, 42 },
	{ 9, 13, 15, 17, 19, 21, 22, 24,
	13, 13, 17, 19, 21, 22, 24, 25,
	15, 17, 19, 21, 22, 24, 25, 27,
	17, 19, 21, 22, 24, 25, 27, 28,
	19, 21, 22, 24, 25, 27, 28, 30,
	21, 22, 24, 25, 27, 28, 30, 32,
	22, 24, 25, 27, 28, 30, 32, 33,
	24, 25, 27, 28, 30, 32, 33, 35 }
};

void decode_scaling_list(bitstream_reader &bs, uint8_t *factors, int size,
	const uint8_t *jvt_list,
	const uint8_t *fallback_list)
{
	int i, last = 8, next = 8;
	const uint8_t *scan = size == 16 ? ff_zigzag_scan : ff_zigzag_direct;
	if (!bs.u(1)) /* matrix not written, we use the predicted one */
		memcpy(factors, fallback_list, size * sizeof(uint8_t));
	else
		for (i = 0; i < size; i++) {
			if (next)
				next = (last + bs.se()) & 0xff;
			if (!i && !next) { /* matrix not written, we use the preset one */
				memcpy(factors, jvt_list, size * sizeof(uint8_t));
				break;
			}
			last = factors[scan[i]] = next ? next : last;
		}
}

/* returns non zero if the provided SPS scaling matrix has been filled */
int decode_scaling_matrices(bitstream_reader &bs, const SPS &sps,
	uint8_t(*scaling_matrix4)[16],
	uint8_t(*scaling_matrix8)[64])
{
	bool is_sps = true;
	int fallback_sps = !is_sps && sps.scaling_matrix_present;
	const uint8_t *fallback[4] = {
		fallback_sps ? sps.scaling_matrix4[0] : default_scaling4[0],
		fallback_sps ? sps.scaling_matrix4[3] : default_scaling4[1],
		fallback_sps ? sps.scaling_matrix8[0] : default_scaling8[0],
		fallback_sps ? sps.scaling_matrix8[3] : default_scaling8[1]
	};
	int ret = 0;
	if (bs.u(1)) {
		ret = is_sps;
		decode_scaling_list(bs, scaling_matrix4[0], 16, default_scaling4[0], fallback[0]);        // Intra, Y
		decode_scaling_list(bs, scaling_matrix4[1], 16, default_scaling4[0], scaling_matrix4[0]); // Intra, Cr
		decode_scaling_list(bs, scaling_matrix4[2], 16, default_scaling4[0], scaling_matrix4[1]); // Intra, Cb
		decode_scaling_list(bs, scaling_matrix4[3], 16, default_scaling4[1], fallback[1]);        // Inter, Y
		decode_scaling_list(bs, scaling_matrix4[4], 16, default_scaling4[1], scaling_matrix4[3]); // Inter, Cr
		decode_scaling_list(bs, scaling_matrix4[5], 16, default_scaling4[1], scaling_matrix4[4]); // Inter, Cb

		decode_scaling_list(bs, scaling_matrix8[0], 64, default_scaling8[0], fallback[2]); // Intra, Y
		decode_scaling_list(bs, scaling_matrix8[3], 64, default_scaling8[1], fallback[3]); // Inter, Y
		if (sps.chroma_format_idc == 3) {
			decode_scaling_list(bs, scaling_matrix8[1], 64, default_scaling8[0], scaling_matrix8[0]); // Intra, Cr
			decode_scaling_list(bs, scaling_matrix8[4], 64, default_scaling8[1], scaling_matrix8[3]); // Inter, Cr
			decode_scaling_list(bs, scaling_matrix8[2], 64, default_scaling8[0], scaling_matrix8[1]); // Intra, Cb
			decode_scaling_list(bs, scaling_matrix8[5], 64, default_scaling8[1], scaling_matrix8[4]); // Inter, Cb
		}
	}

	return ret;
}

bool ReadSPS(bitstream_reader &bs, SPS &sps)
{
	int width = 0;
	int height = 0;

	sps.profile_idc = bs.u(8);

	sps.constraint_set_flags |= bs.u(1) << 0;
	sps.constraint_set_flags |= bs.u(1) << 1;
	sps.constraint_set_flags |= bs.u(1) << 2;
	sps.constraint_set_flags |= bs.u(1) << 3;
	sps.constraint_set_flags |= bs.u(1) << 4;
	sps.constraint_set_flags |= bs.u(1) << 5;
	bs.skip_bits(2);                             // reserved_zero_2bits

	sps.level_idc = bs.u(8);
	sps.sps_id = bs.ue();

	if (sps.sps_id >= MAX_SPS_COUNT) {
		return false;
	}

	memset(sps.scaling_matrix4, 16, sizeof(sps.scaling_matrix4));
	memset(sps.scaling_matrix8, 16, sizeof(sps.scaling_matrix8));
	sps.scaling_matrix_present = 0;

	if (sps.profile_idc == 100 ||  // High profile
		sps.profile_idc == 110 ||  // High10 profile
		sps.profile_idc == 122 ||  // High422 profile
		sps.profile_idc == 244 ||  // High444 Predictive profile
		sps.profile_idc == 44 ||  // Cavlc444 profile
		sps.profile_idc == 83 ||  // Scalable Constrained High profile (SVC)
		sps.profile_idc == 86 ||  // Scalable High Intra profile (SVC)
		sps.profile_idc == 118 ||  // Stereo High profile (MVC)
		sps.profile_idc == 128 ||  // Multiview High profile (MVC)
		sps.profile_idc == 138 ||  // Multiview Depth High profile (MVCD)
		sps.profile_idc == 144) // old High444 profile
	{
		sps.chroma_format_idc = bs.ue();
		if (sps.chroma_format_idc == 3) {
			sps.residual_color_transform_flag = bs.u(1); // separate_colour_plane_flag
		}

		sps.bit_depth_luma = bs.ue() + 8;
		sps.bit_depth_chroma = bs.ue() + 8;
		sps.transform_bypass = bs.u(1);
		sps.scaling_matrix_present |= decode_scaling_matrices(bs, sps, sps.scaling_matrix4, sps.scaling_matrix8);
	}
	else {
		sps.chroma_format_idc = 1;
		sps.bit_depth_luma = 8;
		sps.bit_depth_chroma = 8;
	}

	sps.log2_max_frame_num = bs.ue() + 4;

	sps.pic_order_cnt_type = bs.ue();
	if (sps.pic_order_cnt_type == 0) {
		sps.log2_max_pic_order_cnt_lsb = bs.ue() + 4;
	} else if (sps.pic_order_cnt_type == 1) {
	
		sps.delta_pic_order_always_zero_flag = bs.u(1);
		sps.offset_for_non_ref_pic = bs.se();
		sps.offset_for_top_to_bottom_field = bs.se();
		sps.num_ref_frames_in_pic_order_cnt_cycle = bs.ue();

		for (int i = 0; i < sps.num_ref_frames_in_pic_order_cnt_cycle; i++)
			sps.offset_for_ref_frame[i] = bs.se();
	}
	
	sps.max_num_ref_frames = bs.ue();

	sps.gaps_in_frame_num_allowed_flag = bs.u(1);
	sps.mb_width = (bs.ue() + 1) * 16;			// ???
	sps.mb_height = (bs.ue() + 1) * 16;			// ???

	sps.frame_mbs_only_flag = bs.u(1);
	if (!sps.frame_mbs_only_flag) {
		sps.mb_adaptive_frame_field_flag = bs.u(1);
	} else {
		sps.mb_adaptive_frame_field_flag = 0;
	}

	sps.direct_8x8_inference_flag = bs.u(1);

	sps.frame_cropping_flag = bs.u(1);
	if (sps.frame_cropping_flag) {	
		sps.crop_left = bs.ue();
		sps.crop_right = bs.ue();
		sps.crop_top = bs.ue();
		sps.crop_bottom = bs.ue();
	} else {	
		sps.crop_left = 0;
		sps.crop_right = 0;
		sps.crop_top = 0;
		sps.crop_bottom = 0;
	}

	sps.vui_parameters_present_flag = bs.u(1);

	return true;
}
