// MediaDecoder.h: Media decoding base class.
// 
//   Copyright (C) 2007 Free Software Foundation, Inc.
// 
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

// $Id:

#ifndef __MEDIADECODER_H__
#define __MEDIADECODER_H__

#include <boost/thread/thread.hpp>
#include <boost/bind.hpp> 
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>
#include <vector> 

#include "MediaBuffer.h"
#include "MediaParser.h"
#include "FLVParser.h"
#include "log.h"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

namespace gnash {


/// \brief
/// The MediaDecoder class decodes media data and puts it on a given buffer.
///
/// We need to be able to handle event stuff, so we store the events that
/// need to be handled, and then NetStream can ask for it on ::advance()
/// or whenever it is appropriate. Eventhandling is different from AS2 to AS3,
/// but in this first draft only AS2 will be supported.
///
class MediaDecoder
{

public:
	/// Status codes used for NetStream onStatus notifications
	// Copied from NetSteam.h, so it should be updated it the orginal
	// is changed.
	enum StatusCode {
	
		// Internal status, not a valid ActionScript value
		invalidStatus,

		/// NetStream.Buffer.Empty (level: status)
		bufferEmpty,

		/// NetStream.Buffer.Full (level: status)
		bufferFull,

		/// NetStream.Buffer.Flush (level: status)
		bufferFlush,

		/// NetStream.Play.Start (level: status)
		playStart,

		/// NetStream.Play.Stop  (level: status)
		playStop,

		/// NetStream.Seek.Notify  (level: status)
		seekNotify,

		/// NetStream.Play.StreamNotFound (level: error)
		streamNotFound,

		/// NetStream.Seek.InvalidTime (level: error)
		invalidTime
	};

	/// This is copied from the render and should be changed if the original is.
	enum videoOutputFormat
	{
		NONE,
		YUV,
		RGB
	};

	/// Internal error codes
	enum MediaDecoderErrorCode {
	
		/// All is fine
		noError,

		/// Stream/connection error
		streamError,

		/// Error while decoding
		decodingError,

		/// Error while parsing
		parsingError
	};

	/// \brief
	/// Create a MediaDecoder reading input from
	/// the given tu_file
	//
	/// @param stream
	/// 	tu_file to use for input.
	/// 	Ownership left to the caller.
	///
	/// @param buffer
	/// 	The buffer we will use.
	/// 	Ownership left to the caller.
	///
	MediaDecoder(tu_file* stream, MediaBuffer* buffer, uint16_t swfVersion, int format)
		:
	_buffer(buffer),
	_stream(stream),
	_swfVersion(swfVersion),
	_videoFrameFormat(format),
	_parser(NULL),
	_lastConfirmedPosition(0),
	_streamSize(0),
	_error(noError),
	_isFLV(true),
	_inputPos(0),
	_audio(true),
	_video(true)
	{
	}

	/// Destroys the Decoder
	~MediaDecoder() {}

	/// Pause decoding (needed ?)
	virtual void pause() {}

	/// Resume/start decoding (needed ?)
	virtual void decode() {}

	/// Seeks to pos, returns the new position
	virtual uint32_t seek(uint32_t /*pos*/) { return 0;}

	virtual std::pair<uint32_t, uint32_t> getWidthAndHeight() { return std::pair<uint32_t, uint32_t>(0,0); }

	/// Returns the size of the file being loaded, in bytes
	uint32_t getBytesTotal()
	{
		return _streamSize;
	}

	/// Returns the amount of bytes of the current file that has been loaded.
	uint32_t getBytesLoaded() {
		return _lastConfirmedPosition;
	}

	/// Returns a vector with the waiting onStatus events (AS2)
	std::vector<StatusCode> getOnStatusEvents() {
		boost::mutex::scoped_lock lock(_onStatusMutex);

		const std::vector<StatusCode> statusQueue(_onStatusQueue);
		_onStatusQueue.clear();
		return statusQueue;
	}

	/// Returns whether we got audio
	bool gotAudio() {
		return _audio;
	}

	/// Returns whether we got video
	bool gotVideo() {
		return _video;
	}

protected:
	/// Push an event to the onStatus event queue (AS2)
	void pushOnStatus(StatusCode code) {
		boost::mutex::scoped_lock lock(_onStatusMutex);
		_onStatusQueue.push_back(code);	
	}

	/// The media buffer
	MediaBuffer* _buffer;

	/// The stream we decode
	tu_file* _stream;

	/// Version of the SWF playing
	uint16_t _swfVersion;
	
	/// The output format
	int _videoFrameFormat;

	/// The parser used
	std::auto_ptr<MediaParser> _parser;

	/// The last confirmed size of the downloaded file
	uint32_t _lastConfirmedPosition;

	/// total size of the file being downloaded
	uint32_t _streamSize;

	/// Is everything ok?
	MediaDecoderErrorCode _error;

	/// Waiting NetStream onStatus events (AS2)
	std::vector<StatusCode> _onStatusQueue;

	/// Mutex protecting _onStatusQueue
	boost::mutex _onStatusMutex;

	/// Are we decoding a FLV?
	bool _isFLV;

	/// The position in the inputfile, only used when not playing a FLV
	long _inputPos;

	/// Do we have audio ?
	bool _audio;

	/// Do we have video ?
	bool _video;

};


} // namespace gnash

#endif // __MEDIADECODER_H__
