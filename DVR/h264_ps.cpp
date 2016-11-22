#include "h264_ps.h"
#include <cstring>

void decode_scaling_list(bitstream_reader &bs, int size)
{
	int i, last = 8, next = 8;
	//const uint8_t *scan = size == 16 ? ff_zigzag_scan : ff_zigzag_direct;
	if (!bs.u(1)) { /* matrix not written, we use the predicted one */	
		//memcpy(factors, fallback_list, size * sizeof(uint8_t));
		int x = 0;
	} else {
		for (i = 0; i < size; i++) {
			if (next) {
				next = (last + bs.se()) & 0xff;
			}
			if (!i && !next) { /* matrix not written, we use the preset one */
				//memcpy(factors, jvt_list, size * sizeof(uint8_t));
				break;
			}
			//last = factors[scan[i]] = next ? next : last;
		}
	}
}

int decode_scaling_matrices(bitstream_reader &bs, const SPS &sps)
{
	bool is_sps = true;
	int fallback_sps = !is_sps && sps.scaling_matrix_present;
	int ret = 0;
	if (bs.u(1)) {
		ret = is_sps;
		decode_scaling_list(bs, 16);        // Intra, Y
		decode_scaling_list(bs, 16);		// Intra, Cr
		decode_scaling_list(bs, 16);		// Intra, Cb
		decode_scaling_list(bs, 16);        // Inter, Y
		decode_scaling_list(bs, 16);		// Inter, Cr
		decode_scaling_list(bs, 16);		// Inter, Cb
		decode_scaling_list(bs, 64);		// Intra, Y
		decode_scaling_list(bs, 64);		// Inter, Y
		if (sps.chroma_format_idc == 3) {
			decode_scaling_list(bs, 64);	// Intra, Cr
			decode_scaling_list(bs, 64);	// Inter, Cr
			decode_scaling_list(bs, 64);	// Intra, Cb
			decode_scaling_list(bs, 64);	// Inter, Cb
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

	sps.scaling_matrix_present = 0;

	if (sps.profile_idc == 100 ||		// High profile
		sps.profile_idc == 110 ||		// High10 profile
		sps.profile_idc == 122 ||		// High422 profile
		sps.profile_idc == 244 ||		// High444 Predictive profile
		sps.profile_idc == 44 ||		// Cavlc444 profile
		sps.profile_idc == 83 ||		// Scalable Constrained High profile (SVC)
		sps.profile_idc == 86 ||		// Scalable High Intra profile (SVC)
		sps.profile_idc == 118 ||		// Stereo High profile (MVC)
		sps.profile_idc == 128 ||		// Multiview High profile (MVC)
		sps.profile_idc == 138 ||		// Multiview Depth High profile (MVCD)
		sps.profile_idc == 144)			// old High444 profile
	{
		sps.chroma_format_idc = bs.ue();
		if (sps.chroma_format_idc == 3) {
			sps.residual_color_transform_flag = bs.u(1); // separate_colour_plane_flag
		}

		sps.bit_depth_luma = bs.ue() + 8;
		sps.bit_depth_chroma = bs.ue() + 8;
		sps.transform_bypass = bs.u(1);
		sps.scaling_matrix_present |= decode_scaling_matrices(bs, sps);
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

		for (int i = 0; i < sps.num_ref_frames_in_pic_order_cnt_cycle; i++) {
			int32_t x = bs.se();
		}
	}
	
	sps.max_num_ref_frames = bs.ue();
	sps.gaps_in_frame_num_allowed_flag = bs.u(1);
	sps.mb_width = bs.ue() + 1;
	sps.mb_height = bs.ue() + 1;

	sps.frame_mbs_only_flag = bs.u(1);
	if (!sps.frame_mbs_only_flag) {
		sps.mb_adaptive_frame_field_flag = bs.u(1);
	} else {
		sps.mb_adaptive_frame_field_flag = 0;
	}

	sps.mb_height *= 2 - sps.frame_mbs_only_flag;

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

	sps.mb_width *= 16;
	sps.mb_height *= 16;

	return true;
}
