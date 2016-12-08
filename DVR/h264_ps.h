#pragma once
/*
* This file is part of FFmpeg.
*
* FFmpeg is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public
* License as published by the Free Software Foundation; either
* version 2.1 of the License, or (at your option) any later version.
*
* FFmpeg is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public
* License along with FFmpeg; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
*/

#ifndef AVCODEC_H264_PS_H
#define AVCODEC_H264_PS_H

#include <stdint.h>
#include "bitstream_reader.h"

typedef struct SPS {
	int profile_idc;
	int constraint_set_flags;					///< constraint_set[0-3]_flag
	int level_idc;
	unsigned int sps_id;
	int chroma_format_idc;
	int transform_bypass;						///< qpprime_y_zero_transform_bypass_flag
	int log2_max_frame_num;						///< log2_max_frame_num_minus4 + 4
	int pic_order_cnt_type;						///< pic_order_cnt_type
	int log2_max_pic_order_cnt_lsb;				///< log2_max_pic_order_cnt_lsb_minus4
	int delta_pic_order_always_zero_flag;
	int offset_for_non_ref_pic;
	int offset_for_top_to_bottom_field;
	int num_ref_frames_in_pic_order_cnt_cycle;	///< num_ref_frames_in_pic_order_cnt_cycle
	int max_num_ref_frames;						///< num_ref_frames
	int gaps_in_frame_num_allowed_flag;
	int mb_width;								///< pic_width_in_mbs_minus1 + 1
	int mb_height;								///< (pic_height_in_map_units_minus1 + 1) * (2 - frame_mbs_only_flag)
	int frame_mbs_only_flag;
	int mb_adaptive_frame_field_flag;           ///< mb_adaptive_frame_field_flag
	int direct_8x8_inference_flag;
	int frame_cropping_flag;                    ///< frame_cropping_flag
	unsigned int crop_left;						///< frame_cropping_rect_left_offset
	unsigned int crop_right;					///< frame_cropping_rect_right_offset
	unsigned int crop_top;						///< frame_cropping_rect_top_offset
	unsigned int crop_bottom;					///< frame_cropping_rect_bottom_offset
	int vui_parameters_present_flag;
	int scaling_matrix_present;
	int bit_depth_luma;							///< bit_depth_luma_minus8 + 8
	int bit_depth_chroma;						///< bit_depth_chroma_minus8 + 8
	int residual_color_transform_flag;			///< residual_colour_transform_flag
} SPS;

bool ReadSPS(bitstream_reader &bs, SPS &sps);
void GetResolution(bitstream_reader &bs, int &width, int &height);
void h264_GetWidthHeight(bitstream_reader &bs, size_t &width, size_t &height);

#endif /* AVCODEC_H264_PS_H */
