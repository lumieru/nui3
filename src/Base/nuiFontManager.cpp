/*
  NUI3 - C++ cross-platform GUI framework for OpenGL based applications
  Copyright (C) 2002-2003 Sebastien Metrot

  licence: see nui3/LICENCE.TXT
*/

#include "nui.h"
#include "nuiFontManager.h"
#include "nglPath.h"
#include "nglIStream.h"

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_CACHE_H
#include FT_TRUETYPE_TABLES_H

//#define FONT_TEST_HACK

#ifndef FONT_TEST_HACK
  #ifdef _WIN32_
  #define _WIN32_FONTS_
  #endif

  #ifdef _CARBON_
  #define _CARBON_FONTS_
  #endif
#else
  #define _WIN32_FONTS_
#endif

static FT_Library gFTLibrary = NULL;  // Global FT library instance
nglTextEncoding nglGetCharMapEncoding (FT_CharMap CharMap);

std::multimap<nglString, nglString> nuiFontRequest::gFontsForGenericNames;
std::map<nglString, nglString> nuiFontRequest::gGenericNamesForFonts;
std::map<nglString, nglString> nuiFontRequest::gDefaultFontsForGenericNames;

void nuiFontRequest::AddGenericNameForFont(const nglString& rGenericName, const nglString& rFamilyName)
{
  nuiFontRequest::gFontsForGenericNames.insert(std::pair<nglString, nglString>(rGenericName, rFamilyName));
  nuiFontRequest::gGenericNamesForFonts[rFamilyName] = rGenericName;
}

nglString nuiFontRequest::GetGenericNameForFont(const nglString& rName)
{
  if (nuiFontRequest::gGenericNamesForFonts.find(rName) != nuiFontRequest::gGenericNamesForFonts.end())
  {
    return nuiFontRequest::gGenericNamesForFonts[rName];
  }
  else
  {
    return NULL;
  }
}

void nuiFontRequest::GetFontsForGenericName(const nglString& rName, std::vector<nglString>& rFonts)
{
  rFonts.clear();
  std::pair<std::multimap<nglString, nglString>::iterator, std::multimap<nglString, nglString>::iterator> its = nuiFontRequest::gFontsForGenericNames.equal_range(rName);
  std::multimap<nglString, nglString>::iterator end = its.second;
  for (std::multimap<nglString, nglString>::iterator it = its.first; it != end; ++it)
  {
    rFonts.push_back(it->second);
  }
}

///! Font Request class:
nuiFontRequest::nuiFontRequest(nglFontBase* pOriginalFont, bool ForcePanoseOnlyFonts)
{
  if (SetObjectClass(_T("nuiFontRequest")))
    InitAttributes();
  
  if (nuiFontRequest::gGenericNamesForFonts.empty())
  {
    // Standard generic font family names from CSS specification
    // http://www.w3.org/TR/2002/WD-css3-fonts-20020802/#generic-font-families
    
    // latin serif
    nuiFontRequest::AddGenericNameForFont(_T("serif"), _T("Times"));
    nuiFontRequest::AddGenericNameForFont(_T("serif"), _T("Times New Roman"));
    nuiFontRequest::AddGenericNameForFont(_T("serif"), _T("Bodoni"));
    nuiFontRequest::AddGenericNameForFont(_T("serif"), _T("Garamond"));
    nuiFontRequest::AddGenericNameForFont(_T("serif"), _T("Minion Web"));
    nuiFontRequest::AddGenericNameForFont(_T("serif"), _T("ITC Stone Serif"));
    nuiFontRequest::AddGenericNameForFont(_T("serif"), _T("Georgia"));
    nuiFontRequest::AddGenericNameForFont(_T("serif"), _T("Bitstream Cyberbit"));
    // cyrillic serif
    nuiFontRequest::AddGenericNameForFont(_T("serif"), _T("Adobe Minion Cyrillic"));
    nuiFontRequest::AddGenericNameForFont(_T("serif"), _T("Excelcior Cyrillic Upright"));
    nuiFontRequest::AddGenericNameForFont(_T("serif"), _T("Monotype Albion 70"));
    nuiFontRequest::AddGenericNameForFont(_T("serif"), _T("ER Bukinst"));
    // hebrew serif
    nuiFontRequest::AddGenericNameForFont(_T("serif"), _T("New Peninim"));
    nuiFontRequest::AddGenericNameForFont(_T("serif"), _T("Raanana"));
    // japanese serif
    nuiFontRequest::AddGenericNameForFont(_T("serif"), _T("Ryumin Light-KL"));
    nuiFontRequest::AddGenericNameForFont(_T("serif"), _T("Kyokasho ICA"));
    nuiFontRequest::AddGenericNameForFont(_T("serif"), _T("Futo Min A101"));
    // cherokee serif
    nuiFontRequest::AddGenericNameForFont(_T("serif"), _T("Lo Cicero Cherokee"));
    
    // latin sans-serif
    nuiFontRequest::AddGenericNameForFont(_T("sans-serif"), _T("Trebuchet"));
    nuiFontRequest::AddGenericNameForFont(_T("sans-serif"), _T("ITC Avant Garde Gothic"));
    nuiFontRequest::AddGenericNameForFont(_T("sans-serif"), _T("Arial"));
    nuiFontRequest::AddGenericNameForFont(_T("sans-serif"), _T("Verdana"));
    nuiFontRequest::AddGenericNameForFont(_T("sans-serif"), _T("Univers"));
    nuiFontRequest::AddGenericNameForFont(_T("sans-serif"), _T("Futura"));
    nuiFontRequest::AddGenericNameForFont(_T("sans-serif"), _T("ITC Stone Sans"));
    nuiFontRequest::AddGenericNameForFont(_T("sans-serif"), _T("Gill Sans"));
    nuiFontRequest::AddGenericNameForFont(_T("sans-serif"), _T("Akzidenz Grotesk"));
    nuiFontRequest::AddGenericNameForFont(_T("sans-serif"), _T("Helvetica"));
    // greek sans-serif
    nuiFontRequest::AddGenericNameForFont(_T("sans-serif"), _T("Attika"));
    nuiFontRequest::AddGenericNameForFont(_T("sans-serif"), _T("Typiko New Era"));
    nuiFontRequest::AddGenericNameForFont(_T("sans-serif"), _T("Tahoma"));
    nuiFontRequest::AddGenericNameForFont(_T("sans-serif"), _T("Monotype Gill Sans 571"));
    nuiFontRequest::AddGenericNameForFont(_T("sans-serif"), _T("Helvetica Greek"));
    // cyrillic sans-serif
    nuiFontRequest::AddGenericNameForFont(_T("sans-serif"), _T("Helvetica Cyrillic"));
    nuiFontRequest::AddGenericNameForFont(_T("sans-serif"), _T("ER Univers"));
    nuiFontRequest::AddGenericNameForFont(_T("sans-serif"), _T("Lucida Sans Unicode"));
    nuiFontRequest::AddGenericNameForFont(_T("sans-serif"), _T("Bastion"));
    // hebrew sans-serif
    nuiFontRequest::AddGenericNameForFont(_T("sans-serif"), _T("Arial Hebrew"));
    // japanese sans-serif
    nuiFontRequest::AddGenericNameForFont(_T("sans-serif"), _T("Shin Go"));
    nuiFontRequest::AddGenericNameForFont(_T("sans-serif"), _T("Heisei Kaku Gothic W5"));
    
    // latin cursive
    nuiFontRequest::AddGenericNameForFont(_T("cursive"), _T("Caflisch Script"));
    nuiFontRequest::AddGenericNameForFont(_T("cursive"), _T("Comic Sans MS"));
    nuiFontRequest::AddGenericNameForFont(_T("cursive"), _T("Adobe Poetica"));
    nuiFontRequest::AddGenericNameForFont(_T("cursive"), _T("Sanvito"));
    nuiFontRequest::AddGenericNameForFont(_T("cursive"), _T("Ex Ponto"));
    nuiFontRequest::AddGenericNameForFont(_T("cursive"), _T("Snell Roundhand"));
    nuiFontRequest::AddGenericNameForFont(_T("cursive"), _T("Apple Chancery"));
    nuiFontRequest::AddGenericNameForFont(_T("cursive"), _T("Zapf-Chancery"));
    // cyrillic cursive
    nuiFontRequest::AddGenericNameForFont(_T("cursive"), _T("ER Architekt"));
    // hebrew cursive
    nuiFontRequest::AddGenericNameForFont(_T("cursive"), _T("Corsiva"));
    // arabic cursive
    nuiFontRequest::AddGenericNameForFont(_T("cursive"), _T("DecoType Naskh"));
    nuiFontRequest::AddGenericNameForFont(_T("cursive"), _T("Monotype Urdu 507"));
    
    // latin fantasy
    nuiFontRequest::AddGenericNameForFont(_T("fantasy"), _T("Alpha Geometrique"));
    nuiFontRequest::AddGenericNameForFont(_T("fantasy"), _T("Critter"));
    nuiFontRequest::AddGenericNameForFont(_T("fantasy"), _T("Cottonwood"));
    nuiFontRequest::AddGenericNameForFont(_T("fantasy"), _T("FB Reactor"));
    nuiFontRequest::AddGenericNameForFont(_T("fantasy"), _T("Studz"));
    nuiFontRequest::AddGenericNameForFont(_T("fantasy"), _T("Nina"));
    nuiFontRequest::AddGenericNameForFont(_T("fantasy"), _T("Papyrus"));
    
    // latin monospace
    nuiFontRequest::AddGenericNameForFont(_T("monospace"), _T("Courier"));
    nuiFontRequest::AddGenericNameForFont(_T("monospace"), _T("Courier New"));
    nuiFontRequest::AddGenericNameForFont(_T("monospace"), _T("Prestige"));
    nuiFontRequest::AddGenericNameForFont(_T("monospace"), _T("Everson Mono"));
    // cyrillic monospace
    nuiFontRequest::AddGenericNameForFont(_T("monospace"), _T("ER Kurier"));
    // japanese monospace
    nuiFontRequest::AddGenericNameForFont(_T("monospace"), _T("Osaka Monospaced"));
    
    // add default fonts for each system
#ifdef _WIN32_FONTS_
    nuiFontRequest::gDefaultFontsForGenericNames[_T("serif")] = _T("Times New Roman");
    nuiFontRequest::gDefaultFontsForGenericNames[_T("sans-serif")] = _T("Arial");
    nuiFontRequest::gDefaultFontsForGenericNames[_T("cursive")] = _T("Comic Sans MS");
    nuiFontRequest::gDefaultFontsForGenericNames[_T("fantasy")] = _T("Nina");
    nuiFontRequest::gDefaultFontsForGenericNames[_T("monospace")] = _T("Courier New");
#endif
#ifdef _CARBON_FONTS_
    nuiFontRequest::gDefaultFontsForGenericNames[_T("serif")] = _T("Times");
    nuiFontRequest::gDefaultFontsForGenericNames[_T("sans-serif")] = _T("Helvetica");
    nuiFontRequest::gDefaultFontsForGenericNames[_T("cursive")] = _T("Apple Chancery");
    nuiFontRequest::gDefaultFontsForGenericNames[_T("fantasy")] = _T("Papyrus");
    nuiFontRequest::gDefaultFontsForGenericNames[_T("monospace")] = _T("Courier");
#endif
  }
  
  /*mItalic.mScore = 0.05f;
  mBold.mScore = 0.05f;
  mMonospace.mScore = 0.05f;*/
  if (pOriginalFont)
  {
    if (pOriginalFont->HasPanoseInfo())
    {
      MustBeSimilar(pOriginalFont->GetPanoseBytes(), 50.0f);
    }
      
    if (!ForcePanoseOnlyFonts || !pOriginalFont->HasPanoseInfo())
    {
      SetName(pOriginalFont->GetFamilyName(), 3.0f);
      SetStyle(pOriginalFont->GetStyleName(), 1.5f);
      SetItalic(pOriginalFont->IsItalic(), 1.0f);
      SetBold(pOriginalFont->IsBold(), 1.0f);
      if (pOriginalFont->IsScalable())
        SetScalable(10.f, true);
      if (pOriginalFont->IsMonospace())
        SetMonospace(1.0f);
      MustHaveSize(pOriginalFont->GetSize(), 1.0f);
    }
  }
  
}

void nuiFontRequest::InitAttributes()
{
  // Name
  AddAttribute(new nuiAttribute<const nglString&>
               (nglString(_T("Name")), nuiUnitName,
                nuiFastDelegate::FastDelegate0<const nglString&>(),
                nuiFastDelegate::MakeDelegate(this, &nuiFontRequest::_SetName)));
  
  // GenericName
  AddAttribute(new nuiAttribute<const nglString&>
               (nglString(_T("GenericName")), nuiUnitName,
                nuiFastDelegate::FastDelegate0<const nglString&>(),
                nuiFastDelegate::MakeDelegate(this, &nuiFontRequest::_SetGenericName)));
    
  // Style
  AddAttribute(new nuiAttribute<const nglString&>
               (nglString(_T("Style")), nuiUnitName,
                nuiFastDelegate::FastDelegate0<const nglString&>(),
                nuiFastDelegate::MakeDelegate(this, &nuiFontRequest::_SetStyle)));
  
  // Face
  AddAttribute(new nuiAttribute<int32>
               (nglString(_T("Face")), nuiUnitNone,
                nuiFastDelegate::FastDelegate0<int32>(),
                nuiFastDelegate::MakeDelegate(this, &nuiFontRequest::_SetFace)));
  
  // Size
  AddAttribute(new nuiAttribute<int32>
               (nglString(_T("Size")), nuiUnitPixels,
                nuiFastDelegate::FastDelegate0<int32>(),
                nuiFastDelegate::MakeDelegate(this, &nuiFontRequest::_SetSize)));
  
  // Italic
  AddAttribute(new nuiAttribute<bool>
               (nglString(_T("Italic")), nuiUnitName,
                nuiFastDelegate::FastDelegate0<bool>(),
                nuiFastDelegate::MakeDelegate(this, &nuiFontRequest::_SetItalic)));
  
  // Bold
  AddAttribute(new nuiAttribute<bool>
               (nglString(_T("Bold")), nuiUnitName,
                nuiFastDelegate::FastDelegate0<bool>(),
                nuiFastDelegate::MakeDelegate(this, &nuiFontRequest::_SetBold)));
  
  // Proportionnal
  AddAttribute(new nuiAttribute<bool>
               (nglString(_T("Proportionnal")), nuiUnitName,
                nuiFastDelegate::FastDelegate0<bool>(),
                nuiFastDelegate::MakeDelegate(this, &nuiFontRequest::_SetProportionnal)));
  
  // Monospace
  AddAttribute(new nuiAttribute<bool>
               (nglString(_T("Monospace")), nuiUnitName,
                nuiFastDelegate::FastDelegate0<bool>(),
                nuiFastDelegate::MakeDelegate(this, &nuiFontRequest::_SetMonospace)));
  
  // Scalable
  AddAttribute(new nuiAttribute<bool>
               (nglString(_T("Scalable")), nuiUnitName,
                nuiFastDelegate::FastDelegate0<bool>(),
                nuiFastDelegate::MakeDelegate(this, &nuiFontRequest::_SetScalable)));
  
}

nuiFontRequest::~nuiFontRequest()
{
  
}

void nuiFontRequest::SetName(const nglString& rName, float Score, bool Strict)
{
  mName.mElement = rName;
  mName.mScore = Score;  
  mName.mStrict = Strict;  
}

void nuiFontRequest::SetGenericName(const nglString& rName, float Score, bool Strict)
{
  mGenericName.mElement = rName;
  mGenericName.mScore = Score;  
  mGenericName.mStrict = Strict;  
}

void nuiFontRequest::SetStyle(const nglString& rStyle, float Score, bool Strict)
{
  mStyle.mElement = rStyle;
  mStyle.mScore = Score;  
  mStyle.mStrict = Strict;  
}

void nuiFontRequest::SetFace(int32 Face, float Score, bool Strict)
{
  mFace.mElement = Face;
  mFace.mScore = Score;
  mFace.mStrict = Strict;  
}

void nuiFontRequest::SetItalic(bool Italic, float Score, bool Strict)
{
  mItalic.mElement = Italic;
  mItalic.mScore = Score;
  mItalic.mStrict = Strict;  
}

void nuiFontRequest::SetBold(bool Bold, float Score, bool Strict)
{
  mBold.mElement = Bold;
  mBold.mScore = Score;  
  mBold.mStrict = Strict;  
}

void nuiFontRequest::MustHaveGlyphs(const std::set<nglChar>& rGlyphs, float Score, bool Strict)
{
  mMustHaveGlyphs.mElement = rGlyphs;
  mMustHaveGlyphs.mScore = Score;
  mMustHaveGlyphs.mStrict = Strict;  
}

void nuiFontRequest::MustHaveGlyph(nglChar Glyph, float Score, bool Strict)
{
  mMustHaveGlyphs.mElement.insert(Glyph);
  mMustHaveGlyphs.mScore = Score;
  mMustHaveGlyphs.mStrict = Strict;  
}

void nuiFontRequest::SetProportionnal(float Score, bool Strict)
{
  mMonospace.mElement = false;
  mMonospace.mScore = Score;
  mMonospace.mStrict = Strict;  
}

void nuiFontRequest::SetMonospace(float Score, bool Strict)
{
  mMonospace.mElement = true;
  mMonospace.mScore = Score;
  mMonospace.mStrict = Strict;  
}

void nuiFontRequest::SetScalable(float Score, bool Strict)
{
  mScalable.mElement = true;
  mScalable.mScore = Score;
  mScalable.mStrict = Strict;  
}

void nuiFontRequest::MustHaveEncoding(nglTextEncoding Encoding, float Score, bool Strict)
{
  mMustHaveEncoding.mElement.insert(Encoding);
  mMustHaveEncoding.mScore = Score;
  mMustHaveEncoding.mStrict = Strict;  
}

void nuiFontRequest::MustHaveSize(int32 size, float Score, bool Strict)
{
  mMustHaveSizes.mElement.insert(size);
  mMustHaveSizes.mScore = Score;
  mMustHaveSizes.mStrict = Strict;  
}

void nuiFontRequest::MustBeSimilar(const nuiPanose& rPanose, float score, bool Strict)
{
  mPanose.mElement = rPanose;
  mPanose.mScore = score;
  mPanose.mStrict = Strict;
}

void nuiFontRequest::_SetName(const nglString& rName)
{
  SetName(rName, 1.0);
}

void nuiFontRequest::_SetGenericName(const nglString& rName)
{
  SetGenericName(rName, 1.0);
}

void nuiFontRequest::_SetStyle(const nglString& rStyle)
{
  SetStyle(rStyle, 1.0);
}

void nuiFontRequest::_SetFace(int32 Face)
{
  SetFace(Face, 1.0);
}

void nuiFontRequest::_SetItalic(bool Italic)
{
  SetItalic(Italic, 1.0);
}

void nuiFontRequest::_SetBold(bool Bold)
{
  SetBold(Bold, 1.0);
}

void nuiFontRequest::_SetProportionnal(bool set)
{
  if (set)
    SetProportionnal(1.0f);
  else
    SetMonospace(1.0f);
}

void nuiFontRequest::_SetMonospace(bool set)
{
  if (set)
    SetMonospace(1.0f);
  else
    SetProportionnal(1.0f);
}

void nuiFontRequest::_SetScalable(bool set)
{
  SetScalable(1.0f);
}

void nuiFontRequest::_SetSize(int32 size)
{
  MustHaveSize(size, 1.0f);
}


///! FontRequestResult:
nuiFontRequestResult::nuiFontRequestResult(const nglPath& rPath, int32 Face, float Score, nuiFontDesc* pFontDesc)
{
  mPath = rPath;
  mFace = Face;
  mScore = Score;
  mpFontDesc = pFontDesc;
}

nuiFontRequestResult::~nuiFontRequestResult()
{
}

const nglPath& nuiFontRequestResult::GetPath() const
{
  return mPath;
}

int32 nuiFontRequestResult::GetFace() const
{
  return mFace;
}

float nuiFontRequestResult::GetScore() const
{
  return mScore;
}

const nuiFontDesc* nuiFontRequestResult::GetFontDesc() const
{
  return mpFontDesc;
}


///! nuiFontDesc
//class nuiFontDesc
nuiFontDesc::nuiFontDesc(const nglPath& rPath, int32 Face)
{
  //nglFont* pFont = NULL;
  mValid = false;
  
  mPath = rPath;
  mFace = Face;
  
  //pFont = new nglFont(rPath, Face);
  
//  if (!pFont->GetError() && (pFont->GetFaceCount() > (uint32)Face))
//  {
//    mValid = true;
//    
//    mName = pFont->GetFamilyName();
//    mStyle = pFont->GetStyleName();
//    
//    mBold = pFont->IsBold();
//    mItalic = pFont->IsItalic();
//    mMonospace = pFont->IsMonospace();
//    mScalable = pFont->IsScalable();
//    pFont->GetEncodings(mEncodings);
//    pFont->GetGlyphs(mGlyphs);
//    pFont->GetSizes(mSizes);
//  }
//  delete pFont;

  nglIStream* pStream = rPath.OpenRead();
  
  if (!pStream)
  {
    printf("Error Scanning font '%ls' face %d\n", rPath.GetChars(), Face);
    return;
  }
  
  uint32 size = pStream->Available();
  FT_Byte* pBuffer = new FT_Byte[size];
  pStream->ReadUInt8(pBuffer, size);
  delete pStream;
  
  FT_Error error = 0;
  FT_Face pFace = NULL;
  
  error = FT_New_Memory_Face(gFTLibrary, pBuffer, size, Face, &pFace);
 
  if (error)
    return;
  NGL_OUT(_T("Scanning font '%ls' face %d\n"), rPath.GetChars(), Face);
  
  NGL_ASSERT(pFace->num_faces > Face);
  
  mValid = true;

  mName.Import (pFace->family_name);
  mStyle.Import (pFace->style_name);
  
  mBold = ((pFace->style_flags & FT_STYLE_FLAG_BOLD) != 0);
  mItalic = ((pFace->style_flags & FT_STYLE_FLAG_ITALIC) != 0);
  mMonospace = ((pFace->face_flags & FT_FACE_FLAG_FIXED_WIDTH) != 0);
  mScalable = FT_IS_SCALABLE(pFace);

  // Get Panose information from the TT OS/2 tables
  TT_OS2* pOS2 = (TT_OS2*)FT_Get_Sfnt_Table(pFace, ft_sfnt_os2);
  if (pOS2)
    memcpy(&mPanoseBytes, pOS2->panose, 10);
  else
  {
    NGL_OUT(_T("Warning: font '%ls' has no panose information.\n"), mPath.GetChars());
    memset(&mPanoseBytes, 0, 10);
  }

  // Get encodings:
  for (int32 i = 0; i < pFace->num_charmaps; i++)
  {
    FT_CharMap charmap = pFace->charmaps[i];
    mEncodings.insert(nglGetCharMapEncoding(charmap));
  }
  
  // Get sizes
  for (int32 i = 0; i < pFace->num_fixed_sizes; i++)
  {
    FT_Bitmap_Size& size = pFace->available_sizes[i];
    
    mSizes.insert(size.height);
  }
  
  // Get glyphs:
  FT_ULong  charcode = 0;
  FT_UInt   gindex = 0;
    
  charcode = FT_Get_First_Char(pFace, &gindex);
  while ( gindex != 0 )
  {
    mGlyphs.insert(charcode);
    charcode = FT_Get_Next_Char(pFace, charcode, &gindex);     
  }
  
  FT_Done_Face(pFace);
  delete pBuffer;
}

nuiFontDesc::nuiFontDesc(nglIStream& rStream)
{
  mValid = Load(rStream);
}

nuiFontDesc::~nuiFontDesc()
{
}

bool nuiFontDesc::IsValid() const
{
  return mValid;
}

const nglPath& nuiFontDesc::GetPath() const
{
  return mPath;
}

const nglString& nuiFontDesc::GetName() const
{
  return mName;
}

const nglString& nuiFontDesc::GetStyle() const
{
  return mStyle;
}

int32 nuiFontDesc::GetFace() const
{
  return mFace;
}

bool nuiFontDesc::GetBold() const
{
  return mBold;
}

bool nuiFontDesc::GetItalic() const
{
  return mItalic;
}

bool nuiFontDesc::GetMonospace() const
{
  return mMonospace;
}

bool nuiFontDesc::GetScalable() const
{
  return mScalable;
}

bool nuiFontDesc::HasEncoding(nglTextEncoding Encoding) const
{
  std::set<nglTextEncoding>::const_iterator it = mEncodings.find(Encoding);
  return (it != mEncodings.end());
}

bool nuiFontDesc::HasGlyph(nglChar Glyph) const
{
  std::set<nglChar>::const_iterator it = mGlyphs.find(Glyph);
  return (it != mGlyphs.end());
}

bool nuiFontDesc::HasSize(int32 Size) const
{
  std::set<int32>::const_iterator it = mSizes.find(Size);
  return (it != mSizes.end());
}

const std::set<nglTextEncoding>& nuiFontDesc::GetEncodings() const
{
  return mEncodings;
}

const std::set<nglChar>& nuiFontDesc::GetGlyphs() const
{
  return mGlyphs;
}

const std::set<int32>& nuiFontDesc::GetSizes() const
{
  return mSizes;
}

const nuiFontPanoseBytes& nuiFontDesc::GetPanoseBytes() const
{
  return mPanoseBytes;
}

bool nuiFontDesc::Save(nglOStream& rStream)
{
  uint32 s = 0;
  const char* pPath = mPath.GetPathName().Export(eUTF8);
  const char* pName = mName.Export(eUTF8);
  const char* pStyle = mStyle.Export(eUTF8);
  
  // Write the path of the font:
  s = strlen(pPath);
  rStream.WriteUInt32(&s);
  rStream.Write(pPath, s, 1);
  
  // Write the name of the font face:
  s = strlen(pName);
  rStream.WriteUInt32(&s);
  rStream.Write(pName, s, 1);
  
  // Write the style of the font face:
  s = strlen(pStyle);
  rStream.WriteUInt32(&s);
  rStream.Write(pStyle, s, 1);
  
  
  rStream.WriteInt32(&mFace);
  
  uint32 boolean = 0;
  
  boolean = mBold ? 1 : 0;
  rStream.WriteUInt32(&boolean);
  boolean = mItalic ? 1 : 0;
  rStream.WriteUInt32(&boolean);
  boolean = mMonospace ? 1 : 0;
  rStream.WriteUInt32(&boolean);
  boolean = mScalable ? 1 : 0;
  rStream.WriteUInt32(&boolean);
  
  s = mEncodings.size();
  rStream.WriteUInt32(&s);
  if (s)
  {
    std::vector<uint32> Encodings(mEncodings.begin(), mEncodings.end());
    rStream.WriteUInt32(&Encodings[0], s);
  }
  
  s = mGlyphs.size();
  rStream.WriteUInt32(&s);
  if (s)
  {
    std::vector<uint16> Glyphs(mGlyphs.begin(), mGlyphs.end());
    rStream.WriteUInt16(&Glyphs[0], s);
  }
  
  s = mSizes.size();
  rStream.WriteUInt32(&s);
  if (s)
  {
    std::vector<uint32> Sizes(mSizes.begin(), mSizes.end());  
    rStream.WriteUInt32(&Sizes[0], s);
  }
 
  // Write the panose bytes for this font:
  rStream.Write(&mPanoseBytes, 10, 1);
  
  delete[] pPath;
  delete[] pName;
  delete[] pStyle;

  
  
  return true;
}

bool nuiFontDesc::Load(nglIStream& rStream)
{
  uint32 s = 0;
  std::vector<char> chars;
  
  // Write the path of the font:
  rStream.ReadUInt32(&s);
  chars.resize(s);
  rStream.Read(&chars[0], s, 1);
  mPath = nglString(&chars[0], s, eUTF8);
  
  // Read the name of the font face:
  rStream.ReadUInt32(&s);
  chars.resize(s);
  rStream.Read(&chars[0], s, 1);
  mName = nglString(&chars[0], s, eUTF8);
  
  // Read the style of the font face:
  rStream.ReadUInt32(&s);
  chars.resize(s);
  rStream.Read(&chars[0], s, 1);
  mStyle = nglString(&chars[0], s, eUTF8);
  
  
  rStream.ReadInt32(&mFace);
  
  uint32 boolean = 0;
  
  rStream.ReadUInt32(&boolean);
  mBold = boolean != 0;
  rStream.ReadUInt32(&boolean);
  mItalic = boolean != 0;
  rStream.ReadUInt32(&boolean);
  mMonospace = boolean != 0;
  rStream.ReadUInt32(&boolean);
  mScalable = boolean != 0;
  
  rStream.ReadUInt32(&s);
  std::vector<uint32> Encodings;
  Encodings.resize(s);
  if (s)
  {
    rStream.ReadUInt32(&Encodings[0], s);
    mEncodings.clear();
    for (uint i = 0; i < Encodings.size(); i++)
      mEncodings.insert((nglTextEncoding)Encodings[i]);
  }
  
  rStream.ReadUInt32(&s);
  std::vector<uint16> Glyphs;
  Glyphs.resize(s);
  if (s)
  {
    rStream.ReadUInt16(&Glyphs[0], s);
    mGlyphs.clear();
    mGlyphs.insert(Glyphs.begin(), Glyphs.end());
  }
  
  rStream.ReadUInt32(&s);
  std::vector<uint32> Sizes;
  Sizes.resize(s);
  if (s)
  {
    rStream.ReadUInt32(&Sizes[0], s);
    mSizes.clear();
    mSizes.insert(Sizes.begin(), Sizes.end());
  }

  // Write the panose bytes for this font:
  rStream.Read(&mPanoseBytes, 10, 1);
  
  return true;
}


///! Font Manager class:
nuiFontManager::nuiFontManager()
{
  //std::map<nglString, nglPath> mFontFolders;
}

nuiFontManager::~nuiFontManager()
{
  Clear();
}

void nuiFontManager::AddFolder(const nglString& rId, const nglPath& rPath)
{
  mFontFolders[rId] = rPath;
}

void nuiFontManager::AddSystemFolders()
{
  GetSystemFolders(mFontFolders);
}

void nuiFontManager::GetSystemFolders(std::map<nglString, nglPath>& rFolders)
{
#ifdef FONT_TEST_HACK
  rFolders[_T("System0")] = _T("/Users/meeloo/Desktop/Fonts/");
#else
  #ifdef _CARBON_
    rFolders[_T("System0")] = _T("/System/Library/Fonts/");
    rFolders[_T("System1")] = _T("/Library/Fonts/");
  #endif
  #ifdef _WIN32_
    nglChar p[MAX_PATH];
    HRESULT hr = SHGetFolderPath(NULL, CSIDL_FONTS, NULL, 0, p);
    if (hr == S_OK)
    {
      rFolders[_T("System0")] = p;
      
      NGL_OUT(_T("Adding System0 font folder: '%ls'\n"), p);
    }
    else
    {
      rFolders[_T("System0")] = _T("/c:/windows/fonts/");
    }
  #endif
  #ifdef _LINUX_
    int count = 0;
    Display* pDisplay = XOpenDisplay(NULL);
    char** pPathes = XGetFontPath(pDisplay, &count);
    for (uint i = 0; i < count; i++)
    {
      nglString str;
      str.CFormat(_T("System%d"), i);
      rFolders[str] = nglString(pPathes[i]);
    }
    XFreeFontPath(pPathes);
    XCloseDisplay(pDisplay);
  #endif
#endif
}

void nuiFontManager::DelFolder(const nglString& rId)
{
  std::map<nglString, nglPath>::iterator it = mFontFolders.find(rId);
  if (it != mFontFolders.end())
    mFontFolders.erase(it);
}

void nuiFontManager::DelFolder(const nglPath& rPath)
{
  std::map<nglString, nglPath>::iterator it = mFontFolders.begin();
  std::map<nglString, nglPath>::iterator end = mFontFolders.end();
  while (it != mFontFolders.end())
  {
    if (it->second == rPath)
      mFontFolders.erase(it++);
    else
      ++it;
  }
}

const nglPath& nuiFontManager::GetFolder(const nglString& rId) const
{
  std::map<nglString, nglPath>::const_iterator it = mFontFolders.find(rId);
  NGL_ASSERT(it != mFontFolders.end());
  return it->second;
}

void nuiFontManager::ScanFolders(bool rescanAllFolders /* = false */)
{
  NGL_OUT(_T("Scan system fonts....\n"));
  nglTime start_time;
  
  mpFonts.clear();
  
  // Scan each path:
  std::map<nglString, nglPath>::const_iterator it = mFontFolders.begin();
  std::map<nglString, nglPath>::const_iterator end = mFontFolders.end();
  
  if (rescanAllFolders)
  {
    mScannedFolders.clear();
  }

  NGL_ASSERT(gFTLibrary == NULL);
  FT_Error error;
  error = FT_Init_FreeType(&gFTLibrary);
  
  nglString tr = _T("BOOKOS.ttf");
  
  while (it != end)
  {
    nglPath path(it->second);
    
    if (mScannedFolders.find(path) == mScannedFolders.end())
    {
      mScannedFolders.insert(path);
      
      std::list<nglPath> children;
      path.GetChildren(&children);
      
      std::list<nglPath>::const_iterator cit = children.begin();
      std::list<nglPath>::const_iterator cend = children.end();
      
      if (path.GetNodeName().Compare(tr, false) != 0)
      {
        while (cit != cend)
        {
          // enumerate all the faces of all the fonts:
          const nglPath& rPath(*cit);
          
          bool cont = true;
          int32 face = 0;
          while (cont)
          {
            nuiFontDesc* pFontDesc = new nuiFontDesc(rPath, face);
            if (pFontDesc->IsValid())
            {
              mpFonts.push_back(pFontDesc);
            }
            else
            {
              delete pFontDesc;
              cont = false;
            }
            face++;
          }
          
          ++cit;
        }
      }
    }
    
    ++it;
  }
  
  FT_Done_FreeType(gFTLibrary);
  gFTLibrary = NULL;

  nglTime end_time;
  
  double t = end_time - start_time;
  printf("Scanning the system fonts took %f seconds\n", t);
}

void nuiFontManager::GetFolderList(std::vector<nglString>& rList) const
{
  rList.clear();
  std::map<nglString, nglPath>::const_iterator it = mFontFolders.begin();
  std::map<nglString, nglPath>::const_iterator end = mFontFolders.end();
  while (it != mFontFolders.end())
  {
    rList.push_back(it->first);
    ++it;
  }
}

void nuiFontManager::GetFonts(std::vector<nuiFontDesc*>& rList) const
{
  rList = mpFonts;
}

template <class T> float intersection_score(const std::set<T>& rRequest, const std::set<T>& rReference)
{
  float f = 1.0f / (float)rRequest.size();
  float score = 0.f;
  
  typename std::set<T>::const_iterator it = rRequest.begin();
  typename std::set<T>::const_iterator end = rRequest.end();
  
  while (it != end)
  {
    typename std::set<T>::const_iterator found = rReference.find(*it);
    if (found != rReference.end())
      score += f;
    ++it;
  }
  
  return score;
}

static bool greater_score(const nuiFontRequestResult& rLeft, const nuiFontRequestResult& rRight)
{
  return rLeft.GetScore() > rRight.GetScore();
}

static void SetScore(float& score, float& sscore, float request_score, bool strict, bool condition, bool ref)
{
  if (strict)
  {
    if (condition)
      sscore *= request_score;
    else
      sscore = 0.0f;
  }
  else
  {
    if (condition)
      score += request_score;
    else if (request_score == 0.0f)
      sscore *= .95f; // Lower the sscore if the feature was not particularly requested
  }
}

#define SET_SCORE(X) SetScore(score, sscore, rRequest.m##X.mScore, rRequest.m##X.mStrict, pFontDesc->Get##X() == rRequest.m##X.mElement, pFontDesc->Get##X());
#define SET_SCORE2(X) SetScore(score, sscore, rRequest.m##X.mScore, rRequest.m##X.mStrict, pFontDesc->Get##X().Compare(rRequest.m##X.mElement, false) == 0, false);


void nuiFontManager::RequestFont(nuiFontRequest& rRequest, std::list<nuiFontRequestResult>& rFoundFonts) const
{
  rFoundFonts.clear();
  
  if (nuiFontRequest::gFontsForGenericNames.find(rRequest.mName.mElement) != nuiFontRequest::gFontsForGenericNames.end())
    // given name is actually a generic name
  {
    rRequest.mGenericName = rRequest.mName;
    rRequest.mName.mElement = nuiFontRequest::gDefaultFontsForGenericNames[rRequest.mGenericName.mElement];
  }
  
  std::vector<nuiFontDesc*>::const_iterator it = mpFonts.begin();
  std::vector<nuiFontDesc*>::const_iterator end = mpFonts.end();
  
  nglString dump;
  while (it != end)
  {
    float score = 1.f;
    float sscore = 1.f;
    nuiFontDesc* pFontDesc = *it;
    //pFontDesc->GetPath()
    
    SET_SCORE2(Name);
    SET_SCORE2(Style);
    SET_SCORE(Face);
    SET_SCORE(Bold);
    SET_SCORE(Italic);
    SET_SCORE(Scalable);
    SET_SCORE(Monospace);
    
    {
      nglString genName = nuiFontRequest::GetGenericNameForFont(pFontDesc->GetName());
      if (!genName.IsNull())
      {
        if(rRequest.mGenericName.mStrict)
        {
          if (genName.Compare(rRequest.mGenericName.mElement) == 0)
          {
            sscore *= rRequest.mGenericName.mScore;
          }
          else
          {
            sscore *= 0.f;
          }
        }
        else
        {
          if (genName.Compare(rRequest.mGenericName.mElement) == 0)
          {
            score += rRequest.mGenericName.mScore;
          }
        }
      }
    }
    
    {
      const std::set<nglTextEncoding>& rEncodings(pFontDesc->GetEncodings());
      float _s = rRequest.mMustHaveEncoding.mScore * intersection_score(rRequest.mMustHaveEncoding.mElement, rEncodings);
      if (rRequest.mMustHaveEncoding.mStrict)
      {
        if (_s > 0.f)
          sscore *= _s;
        else
          sscore = 0.f;
      }
      else
        score += _s;
    }
    
    {
      const std::set<nglChar>& rGlyphs(pFontDesc->GetGlyphs());
      float _s = rRequest.mMustHaveGlyphs.mScore * intersection_score(rRequest.mMustHaveGlyphs.mElement, rGlyphs);
      if (rRequest.mMustHaveGlyphs.mStrict)
      {
        if (_s > 0.f)
          sscore *= _s;
        else
          sscore = 0.f;
      }
      else
        score += _s;
    }
    
    SetScore(score, sscore, rRequest.mPanose.mScore, rRequest.mPanose.mStrict, rRequest.mPanose.mElement.GetDistance(pFontDesc->GetPanoseBytes()), false);
    
    if (!pFontDesc->GetScalable())
    {
      if (rRequest.mMustHaveSizes.mStrict)
      {
        sscore *= rRequest.mMustHaveSizes.mScore;
      }
      else
      {
        score += rRequest.mMustHaveSizes.mScore;
      }
    }
    else
    {
      const std::set<int32>& rSizes(pFontDesc->GetSizes());
      float _s = rRequest.mMustHaveSizes.mScore * intersection_score(rRequest.mMustHaveSizes.mElement, rSizes);
      if (rRequest.mMustHaveSizes.mStrict)
      {
        if (_s > 0.f)
          sscore *= _s;
        else
          sscore = 0.f;
      }
      else
        score += _s;
    }
    
    score *= sscore;
    
    //#FIXME: hacky-hack to avoid bitmap fonts
    if (score > 0.f)
      if (pFontDesc->GetScalable())
        rFoundFonts.push_back(nuiFontRequestResult(pFontDesc->GetPath(), pFontDesc->GetFace(), score, pFontDesc));
    
    ++it;
  }
  
  rFoundFonts.sort(greater_score);
}


nuiFontManager nuiFontManager::gManager;

nuiFontManager& nuiFontManager::GetManager(bool InitIfNeeded)
{
  if (InitIfNeeded && gManager.mpFonts.empty() && gManager.mFontFolders.empty())
  {
    gManager.AddSystemFolders();
    gManager.ScanFolders();
  }
  
  return gManager;
}

nuiFontManager& nuiFontManager::LoadManager(nglIStream& rStream)
{
  if (gManager.mFontFolders.empty())
  {
    gManager.AddSystemFolders();
  }
  
  gManager.Clear();
  gManager.Load(rStream);
  
  return gManager;
}


#define NUI_FONTDB_MARKER "nuiFontDatabase3"

bool nuiFontManager::Save(nglOStream& rStream)
{
  rStream.SetEndian(eEndianLittle);
  
  int s = strlen(NUI_FONTDB_MARKER) + 1;
  if (s != rStream.Write(NUI_FONTDB_MARKER, s, 1))
    return false;
  
  std::vector<nuiFontDesc*>::iterator it = mpFonts.begin(); 
  std::vector<nuiFontDesc*>::iterator end = mpFonts.end(); 
  
  while (it != end)
  {
    nuiFontDesc* pFontDesc = *it;
    
    pFontDesc->Save(rStream);    
    
    ++it;
  }
  
  
  return true;
}

bool nuiFontManager::Load(nglIStream& rStream)
{
  rStream.SetEndian(eEndianLittle);
  
  int s = strlen(NUI_FONTDB_MARKER) + 1;
  char* marker = new char[s];
  if (s != rStream.Read(marker, s, 1))
  {
    delete marker;
    
    return false;
  }
  
  if (strcmp(NUI_FONTDB_MARKER, marker))
  {
    delete marker;
    
    return false;
  }
  
  delete[] marker;
  
  Clear();
  
  while (rStream.GetState() == eStreamReady)
  {
    nuiFontDesc* pFontDesc = new nuiFontDesc(rStream);
    
    if (pFontDesc->IsValid() && pFontDesc->GetScalable())
    {
      mpFonts.push_back(pFontDesc);
      
#if 0 // #TEST this is a work in progress!
      nuiMiniTTFLoader loader(pFontDesc->GetPath());
      loader.Load();
#endif
    }
    else
      delete pFontDesc;
  }
  
  
  return true;
}

void nuiFontManager::Clear()
{
  std::vector<nuiFontDesc*>::iterator it = mpFonts.begin(); 
  std::vector<nuiFontDesc*>::iterator end = mpFonts.end(); 
  
  while (it != end)
  {
    nuiFontDesc* pFontDesc = *it;
    
    delete pFontDesc;
    
    ++it;
  }
  
  mpFonts.clear();
  
}

uint32 nuiFontManager::GetFontCount() const
{
  return mpFonts.size();
}


bool nuiFontManager::FindFontInFolders(const nglString& rFontFileName, nglPath& rResultFontPath) const
{
  std::map<nglString, nglPath>::const_iterator it = mFontFolders.begin();
  std::map<nglString, nglPath>::const_iterator end = mFontFolders.end();
  while (it != mFontFolders.end())
  {
    nglPath p(it->second);
    p += rFontFileName;
    if (p.Exists() && p.IsLeaf())
    {
      rResultFontPath = p;
      return true;
    }
    ++it;
  }
  
  return false;
}

bool nuiFontManager::FindFontInSystemFolders(const nglString& rFontFileName, nglPath& rResultFontPath)
{
  std::map<nglString, nglPath> folders;
  nuiFontManager::GetSystemFolders(folders);
  std::map<nglString, nglPath>::const_iterator it = folders.begin();
  std::map<nglString, nglPath>::const_iterator end = folders.end();
  while (it != end)
  {
    nglPath p(it->second);
    p += rFontFileName;
    if (p.Exists() && p.IsLeaf())
    {
      rResultFontPath = p;
      return true;
    }
    ++it;
  }
  
  return false;
}

nuiFont* nuiFontManager::GetFont(nuiFontRequest& rRequest, const nglString& rID) const
{
  std::list<nuiFontRequestResult> Fonts;
  RequestFont(rRequest, Fonts);

  float size = 14.f;
  if (!rRequest.mMustHaveSizes.mElement.empty())
    size = *(rRequest.mMustHaveSizes.mElement.begin());
  
  if (Fonts.empty())
  {
    return nuiFont::GetFont(size);
  }
  
  const nuiFontRequestResult& rFont(*(Fonts.begin()));
  return nuiFont::GetFont(rFont.GetPath(), size, rFont.GetFace(), rID);
}
