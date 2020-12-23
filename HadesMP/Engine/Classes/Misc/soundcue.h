#pragma once

namespace engine::misc
{
	class SoundCue
	{
		DWORD64 audio_channel;
		HashGuid name;
		bool valid;
	};
}