#include <stdint.h>
#include <stdio.h>
#ifdef _WINDOWS
	#include <windows.h>
#endif
#if defined(__MACOSX__)
	#include <OpenGL/gl.h>
#elif defined(__MACOS__)
	#include <gl.h>
#else
	#include <GL/gl.h>
#endif
#include <QApplication>
//#include <QtGui/QRgba64>
#include <system_error>

#include <sys/stat.h>
#include <fcntl.h>

#include "drawAPNG.hxx"

template<typename T> struct pngRGB_t
{
	T r;
	T g;
	T b;
};

template<typename T> struct pngRGBA_t final : public pngRGB_t<T>
	{ T a; };

using pngRGB8_t = pngRGB_t<uint8_t>;
using pngRGB16_t = pngRGB_t<uint16_t>;
using pngRGBA8_t = pngRGBA_t<uint8_t>;
using pngRGBA16_t = pngRGBA_t<uint16_t>;

constexpr uint8_t toByte(uint16_t val) noexcept { return uint8_t(val >> 8); }

QRgb copyRGB8(const pngRGB8_t colour) noexcept
	{ return qRgb(colour.r, colour.g, colour.b); }
QRgb copyRGB16(const pngRGB16_t colour) noexcept
	//{ return QColor(QRgba64::fromRgba64(colour.r, colour.g, colour.b, 0xFFFF)); }
	{ return qRgb(toByte(colour.r), toByte(colour.g), toByte(colour.b)); }
QRgb copyRGBA8(const pngRGBA8_t colour) noexcept
	{ return qRgba(colour.r, colour.g, colour.b, colour.a); }
QRgb copyRGBA16(const pngRGBA16_t colour) noexcept
	//{ return QColor(QRgba64::fromRgba64(colour.r, colour.g, colour.b, colour.a)); }
	{ return qRgba(toByte(colour.r), toByte(colour.g), toByte(colour.b), toByte(colour.a)); }

template<typename T, QRgb copyFunc(const T)> void copyFrame(const bitmap_t *const frame, QImage &dest) noexcept
{
	const T *const data = reinterpret_cast<const T *>(frame->data());
	const uint32_t height = frame->height();
	const uint32_t width = frame->width();
	for (uint32_t y = 0; y < height; ++y)
	{
		for (uint32_t x = 0; x < width; ++x)
			dest.setPixel(x, y, copyFunc(data[x + (y * width)]));
	}
}

drawAPNG_t::drawAPNG_t(QWidget *parent) noexcept : QMainWindow(parent)
{
	window.setupUi(this);

	animateThread = std::thread([this]() noexcept { animate(); });
}

drawAPNG_t::~drawAPNG_t() noexcept
{
	leave = true;
	animateThread.join();
}

void drawAPNG_t::image(std::unique_ptr<apng_t> &&image) noexcept
{
	if (!image)
		return;
	apng.swap(image);
	activeFrame = QImage(apng->width(), apng->height(), pixelFormat(apng->pixelFormat()));
	activeFrame.fill(QColor(0, 0, 0, 0));
	processFrame(apng->defaultFrame(), activeFrame);
	window.image->setPixmap(QPixmap::fromImage(activeFrame));
}

QImage::Format drawAPNG_t::pixelFormat(const pixelFormat_t format) const noexcept
{
	switch (format)
	{
		/*case pixelFormat_t::format8bppGrey:
			return QImage::Format_Grayscale8;*/
		case pixelFormat_t::format24bppRGB:
		case pixelFormat_t::format48bppRGB:
			return QImage::Format_RGB888;
		case pixelFormat_t::format32bppRGBA:
		case pixelFormat_t::format64bppRGBA:
			return QImage::Format_RGBA8888;
		default:
			return QImage::Format_Invalid;
	}
}

void drawAPNG_t::processFrame(const bitmap_t *const frame, QImage &dest) noexcept
{
	switch (frame->format())
	{
		case pixelFormat_t::format24bppRGB:
			return copyFrame<pngRGB8_t, copyRGB8>(frame, dest);
		case pixelFormat_t::format48bppRGB:
			return copyFrame<pngRGB16_t, copyRGB16>(frame, dest);
		case pixelFormat_t::format32bppRGBA:
			return copyFrame<pngRGBA8_t, copyRGBA8>(frame, dest);
		case pixelFormat_t::format64bppRGBA:
			return copyFrame<pngRGBA16_t, copyRGBA16>(frame, dest);
		default:
			break;
	}
}

void drawAPNG_t::animate() noexcept
{
	// TODO: Implement animation here.
}

int main(int argc, char **argv) noexcept
{
	QApplication app(argc, argv);
	drawAPNG_t window;
	try
	{
		fileStream_t file("loading_16.png", O_RDONLY | O_NOCTTY);
		std::unique_ptr<apng_t> pngFile(new apng_t(file));
		window.image(std::move(pngFile));
	}
	catch (invalidPNG_t &error)
	{
		printf("%s\n", error.what());
		return 1;
	}
	catch (std::system_error &error)
	{
		printf("%s\n", error.what());
		return 1;
	}

	window.show();
	return app.exec();
}