#ifndef SCREEN_H
#define SCREEN_H

#include <Windows.h>
#include <cstddef>
#include <memory>
#include <vector>
#include <array>

class Screen {

public:
	Screen(std::size_t width, std::size_t height, HDC d) :
		width_(width),
		height_(height),
		device(d),
		screen_buffer_(1) {

		auto row_stride = width * channels_;

		// Init pixel buffers
		buffers_[0] = std::make_unique<std::vector<uint8_t>>();
		buffers_[1] = std::make_unique<std::vector<uint8_t>>();

		buffers_[0]->resize(row_stride * height);
		buffers_[1]->resize(row_stride * height);
		std::fill(buffers_[0]->begin(), buffers_[0]->end(), 0);
		std::fill(buffers_[1]->begin(), buffers_[1]->end(), 0);

		// Setting BMP header values
		SetBMPHeaderValues();
	}


	void ClearScreenColor(uint8_t b = 255, uint8_t g = 255, uint8_t r = 255) {
		for (auto i = 0; i < height_; ++i) {
			for (auto j = 0; j < width_; ++j) {
				StorePixel(i, j, b, g, r);
			}
		}
	}

	void StorePixel(std::size_t x, std::size_t y,
		uint8_t b = 255, uint8_t g = 255, uint8_t r = 255) {
		auto curr_buf = (screen_buffer_ + 1) % buffers_.size();

		(*buffers_[curr_buf])[channels_ * (y * width_ + x) + 0] = b;
		(*buffers_[curr_buf])[channels_ * (y * width_ + x) + 1] = g;
		(*buffers_[curr_buf])[channels_ * (y * width_ + x) + 2] = r;
	}

	uint8_t* GetScreenData() {
		return buffers_[screen_buffer_]->data();
	}



	void DrawScreen() {
		Swap();
		SetDIBitsToDevice(device,
			0, 0, Width(), Height(),
			0, 0, 0, Height(),
			buffers_[screen_buffer_]->data(),
			&bmi_,
			DIB_RGB_COLORS);
	}

	std::size_t Width() { return width_; }
	std::size_t Height() { return height_; }

private:

	void Swap() {
		std::fill(buffers_[screen_buffer_]->begin(), buffers_[screen_buffer_]->end(), 0);
		screen_buffer_ = (screen_buffer_ + 1) % buffers_.size();
	}

	HDC device;
	void SetBMPHeaderValues() {
		ZeroMemory(&bmi_, sizeof(bmi_));
		BITMAPINFOHEADER& h = bmi_.bmiHeader;
		h.biSize = sizeof(BITMAPINFOHEADER);
		h.biWidth = Width();
		h.biHeight = Height();
		h.biPlanes = 1;
		h.biBitCount = 24;
		h.biCompression = BI_RGB;
		h.biSizeImage = Width() * Height();
	}

	std::size_t screen_buffer_;
	std::array<std::unique_ptr<std::vector<uint8_t>>, 2> buffers_;

	std::size_t width_;
	std::size_t height_;

	const unsigned int channels_ = 3; //Supporting RGB channels only

	BITMAPINFO bmi_;
};


#endif // !SCREEN_H