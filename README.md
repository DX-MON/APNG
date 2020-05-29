# APNG
## An Animated PNG reading library

The goal of this library is to provide a very simple API for reading and using frames from an APNG file.

[![Build Status](https://travis-ci.org/DX-MON/APNG.svg?branch=master)](https://travis-ci.org/DX-MON/APNG)
[![FOSSA Status](https://app.fossa.com/api/projects/git%2Bgithub.com%2FDX-MON%2FAPNG.svg?type=shield)](https://app.fossa.com/projects/git%2Bgithub.com%2FDX-MON%2FAPNG?ref=badge_shield)

## Building the library

This library can be built by running:
`make && make install`

## Testing your build

This library's tests depend on [crunch](https://github.com/DX-MON/crunch) being installed and in your PATH.
Provided this is met, the tests can be built by running `make test`.
To run the tests (and build them if they aren't), run `make check`.

## The API

The main type in the library is apng_t, which allows loading and interogating an APNG file.
There are several ways to present the APNG data to apng_t - using any of the built in stream_t types, or your own.

The three available built-in stream_t types are:
* fileStream_t, which takes the file to open and the mode to open it with using open()'s constants
* memoryStream_t, which takes a buffer and the length of that buffer
* zlibStream_t, which takes some other stream_t that represents a ZLib stream, and whether the stream should be used in inflate or deflate mode

Using fileStream_t as an example, here's a typical way to open an APNG file and have the library read it in:
`apng_t pngFile(fileStream_t("myAPNG.png", O_RDONLY));`
At this point, the frame data will be available by calling pngFile.frames(), with other properties such as the width and the height of the display area available by calling pngFile.width() and pngFile.height().

To free all resources consumed by this operation, simply let apng_t go out of scope, or if you used new to allocate your instance, just call delete on the instance, though you should have used std::unique_ptr<>.
*DO NOTE*: all frame data returned by frames() will be invalidated and you must stop using the pointers, after allowing apng_t to go out of scope.


## License
[![FOSSA Status](https://app.fossa.com/api/projects/git%2Bgithub.com%2FDX-MON%2FAPNG.svg?type=large)](https://app.fossa.com/projects/git%2Bgithub.com%2FDX-MON%2FAPNG?ref=badge_large)