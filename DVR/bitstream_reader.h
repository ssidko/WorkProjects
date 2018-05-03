#pragma once

#include <stdint.h>
#include <stdexcept> 

class bitstream_reader
{
public:

	bitstream_reader(const uint8_t* data, size_t size)
		: data_(data)
		, size_(size)
		, current_data_(data)
		, last_byte_(data + size - 1)
		, current_bit_(0)
	{
	}

	bitstream_reader(const bitstream_reader& other)
		: data_(other.data_)
		, size_(other.size_)
		, current_data_(other.current_data_)
		, last_byte_(other.last_byte_)
		, current_bit_(other.current_bit_)
	{
	}

	size_t bits_available()
	{
		return (size_ - (size_t)(current_data_ - data_)) * 8 - current_bit_;
	}

	size_t current_bit_in_byte()
	{
		return current_bit_;
	}

	uint32_t next_bits(size_t n)
	{
		if (n + current_bit_ <= 8)
			return ((*current_data_) >> (8 - current_bit_ - n)) & ((1 << n) - 1);
		else if (n + current_bit_ <= 16)
			return ((256 * current_data_[0] + current_data_[1]) >> (16 - current_bit_ - n)) & ((1 << n) - 1);
		else if (n + current_bit_ <= 24)
			return ((256 * 256 * current_data_[0] + 256 * current_data_[1] + current_data_[2]) >> (24 - current_bit_ - n)) & ((1 << n) - 1);
		else if (n + current_bit_ <= 32)
			return ((256 * 256 * 256 * current_data_[0] + 256 * 256 * current_data_[1] + 256 * current_data_[2] + current_data_[3]) >> (32 - current_bit_ - n)) & ((1 << n) - 1);
		else
			return ((256LL * 256 * 256 * 256 * current_data_[0] + 256 * 256 * 256 * current_data_[1] + 
				256 * 256 * current_data_[2] + 256 * current_data_[3] + current_data_[4]) >> (40 - current_bit_ - n)) & ((1 << n) - 1);
	}

	uint32_t read_bits(size_t n)
	{
		uint32_t res = 0;
		for (; n > 0; --n)
		{
			res |= (read_bit() << (n - 1));
		}
		return res;
	}

	void write_bits(uint32_t value, size_t n)
	{
		for (; n > 0; --n)
		{
			write_bit((value >> (n - 1)) & 1);
		}
	}

	uint32_t f(size_t n)
	{
		return read_bits(n);
	}

	uint32_t u(size_t n)
	{
		return read_bits(n);
	}

	uint32_t ue()
	{
		uint32_t leadingZeroBits = 0;
		while (read_bits(1) == 0)
		{
			++leadingZeroBits;
		}

		if (leadingZeroBits == 0) return 0;

		return (uint32_t(1) << (leadingZeroBits)) - 1 + read_bits(leadingZeroBits);
	}

	int32_t se()
	{
		uint32_t codeNum = ue();
		int32_t sign = (codeNum % 2 == 0) ? (-1) : (1);
		int32_t val = (codeNum + 1) / 2;
		return sign*val;
	}

	bool byte_aligned()
	{
		return current_bit_ == 0;
	}

	void skip_bits(size_t n)
	{
		current_data_ += n / 8;
		for (size_t i = 0; i<n % 8; i++)
		{
			skip_bit();
		}
	}

	size_t bytes_read()
	{
		return (size_t(current_data_ - data_) + (current_bit_ ? 1 : 0));
	}

	private:
	uint32_t read_bit()
	{
		uint32_t res = get_bit();
		skip_bit();
		return res;
	}

	void write_bit(uint32_t val)
	{
		set_bit(val);
		skip_bit();
	}

	uint32_t get_bit()
	{
		if (current_data_ > last_byte_) {
			throw std::out_of_range("");
		}
		return ((*current_data_) >> (7 - current_bit_)) & 1;
	}

	void set_bit(uint32_t val)
	{
		uint8_t* data = const_cast<uint8_t*>(current_data_);

		(*data) &= (~(uint8_t(1) << (7 - current_bit_)));
		(*data) |= ((val & 1) << (7 - current_bit_));
	}

	void skip_bit()
	{
		current_bit_ += 1;
		if (current_bit_ > 7)
		{
			current_bit_ = 0;
			current_data_ += 1;
		}
	}

private:
	const uint8_t* data_;
	size_t size_;
	
	const uint8_t* current_data_;
	const uint8_t* last_byte_;
	size_t current_bit_;
};
