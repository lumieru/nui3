/*
 NUI3 - C++ cross-platform GUI framework for OpenGL based applications
 Copyright (C) 2002-2003 Sebastien Metrot
 
 licence: see nui3/LICENCE.TXT
 */

#pragma once
#include "nui.h"
#include "nuiSampleReader.h"
#include "nuiFileSound.h"


class nuiFileVoice : nuiVoice
{
public:
  friend class nuiFileSound;
  
  nuiFileVoice(const nuiFileVoice& rVoice);
  nuiFileVoice& operator=(const nuiFileVoice& rVoice);
  
  virtual bool IsValid() const;
  virtual uint32 GetChannels() const;
  
  uint32 GetSampleFrames() const;
  
protected:
  virtual uint32 ReadSamples(const std::vector<float*>& rOutput, int64 position, uint32 SampleFrames);
  
private:
  nuiFileVoice(nuiFileSound* pSound = NULL);
  virtual ~nuiFileVoice();
  
  bool Init();
  
  nuiFileSound* mpFileSound;
  
  nglIStream* mpStream;
  nuiSampleReader* mpReader;
  nuiSampleInfo mInfo;
};