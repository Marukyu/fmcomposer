#include "songEditor.hpp"

void SongEditor::handleChannelHeads()
{

	static unsigned paramChanged = 0;
	static int channelChanged = -1;

	for (unsigned ch = 0; ch<FM_ch; ++ch)
	{
		int mutedChanged = 0;
		int channelSwapped = -1;
		channelHead[ch].update(&paramChanged, &mutedChanged, &channelSwapped);

		/* Mute */
		if (mutedChanged)
		{
			updateMutedChannels();
		}
		/* Channel swapping */
		if (channelSwapped > -1)
		{
			fm_moveChannels(fm, ch, channelSwapped);
			for (unsigned ch2 = 0; ch2 < FM_ch; ++ch2)
			{
				channelHead[ch2].updateFromFM();
				updateChannelData(ch2);
			}
			channelHead[ch].selected = 0;
			channelHead[channelSwapped].selected = 1;
			songModified(1);
		}

		/* Save info if channel param changed */
		if (paramChanged)
		{
			/* Holding ctrl/shift changes all the values */
			if ((keyboard.ctrl || keyboard.shift) && mouse.cursor == CURSOR_NORMAL)
			{
				for (unsigned ch2 = 0; ch2 < FM_ch; ++ch2)
				{
					if (paramChanged == 1)
					{
						channelHead[ch2].pan.setValue(channelHead[ch].pan.value);
						fm_setChannelPanning(fm, ch2, channelHead[ch].pan.value);
					}
					else if (paramChanged == 2)
					{
						channelHead[ch2].vol.setValue(channelHead[ch].vol.value);
						fm_setChannelVolume(fm, ch2, channelHead[ch].vol.value);
					}
					else if (paramChanged == 3)
					{
						channelHead[ch2].rev.setValue(channelHead[ch].rev.value);
						fm_setChannelReverb(fm, ch2, channelHead[ch].rev.value);
					}
				}
			}
			songModified(1);
			paramChanged = 0;
		}
	}
}