/*
 NUI3 - C++ cross-platform GUI framework for OpenGL based applications
 Copyright (C) 2002-2003 Sebastien Metrot
 
 licence: see nui3/LICENCE.TXT
 */

#include "nui.h"
#include "nuiDrawContext.h"
#include "nuiImageDecoration.h"

nuiImageDecoration::nuiImageDecoration(const nglString& rName)
: nuiDecoration(rName),
  mpTexture(NULL),
  mPosition(nuiCenter),
  mColor(255,255,255,255),
  mRepeatX(false),
  mRepeatY(false)
{
  if (SetObjectClass(_T("nuiImageDecoration")))
    InitAttributes();
}

nuiImageDecoration::nuiImageDecoration(const nglString& rName, nuiTexture* pTexture, const nuiRect& rClientRect, nuiPosition position, const nuiColor& rColor)
: nuiDecoration(rName),
  mpTexture(pTexture),
  mPosition(position),
  mClientRect(rClientRect),
  mColor(rColor),
  mRepeatX(false),
  mRepeatY(false)
{
  if (SetObjectClass(_T("nuiImageDecoration")))
    InitAttributes();
}

nuiImageDecoration::nuiImageDecoration(const nglString& rName, const nglPath& rTexturePath, const nuiRect& rClientRect, nuiPosition position, const nuiColor& rColor)
: nuiDecoration(rName),
  mpTexture(NULL),
  mPosition(position),
  mClientRect(rClientRect),
  mColor(rColor),
  mRepeatX(false),
  mRepeatY(false)
{
  if (SetObjectClass(_T("nuiImageDecoration")))
    InitAttributes();
	
  mpTexture = nuiTexture::GetTexture(rTexturePath);
  if (mpTexture)
    SetProperty(_T("Texture"), rTexturePath.GetPathName());
}




void nuiImageDecoration::InitAttributes()
{
  AddAttribute(new nuiAttribute<const nuiRect&>
   (nglString(_T("ClientRect")), nuiUnitNone,
    nuiAttribute<const nuiRect&>::GetterDelegate(this, &nuiImageDecoration::GetSourceClientRect),
    nuiAttribute<const nuiRect&>::SetterDelegate(this, &nuiImageDecoration::SetSourceClientRect)));

  AddAttribute(new nuiAttribute<nglPath>
   (nglString(_T("Texture")), nuiUnitNone,
    nuiMakeDelegate(this, &nuiImageDecoration::GetTexturePath), 
    nuiMakeDelegate(this, &nuiImageDecoration::SetTexturePath)));

  AddAttribute(new nuiAttribute<nuiPosition>
   (nglString(_T("Position")), nuiUnitPosition,
    nuiMakeDelegate(this, &nuiImageDecoration::GetPosition), 
    nuiMakeDelegate(this, &nuiImageDecoration::SetPosition)));
  
  AddAttribute(new nuiAttribute<const nuiColor&>
               (nglString(_T("Color")), nuiUnitNone,
                nuiAttribute<const nuiColor&>::GetterDelegate(this, &nuiImageDecoration::GetColor), 
                nuiAttribute<const nuiColor&>::SetterDelegate(this, &nuiImageDecoration::SetColor)));
  
  AddAttribute(new nuiAttribute<bool>
               (nglString(_T("RepeatX")), nuiUnitBoolean,
                nuiAttribute<bool>::GetterDelegate(this, &nuiImageDecoration::GetRepeatX), 
                nuiAttribute<bool>::SetterDelegate(this, &nuiImageDecoration::SetRepeatX)));  
  
  AddAttribute(new nuiAttribute<bool>
               (nglString(_T("RepeatY")), nuiUnitBoolean,
                nuiAttribute<bool>::GetterDelegate(this, &nuiImageDecoration::GetRepeatY), 
                nuiAttribute<bool>::SetterDelegate(this, &nuiImageDecoration::SetRepeatY)));  
}



nuiImageDecoration::~nuiImageDecoration()
{
  mpTexture->Release();
}

bool nuiImageDecoration::Load(const nuiXMLNode* pNode)
{
  mClientRect.SetValue(nuiGetString(pNode, _T("ClientRect"), _T("{0,0,0,0}")));
  mpTexture = nuiTexture::GetTexture(nglPath(nuiGetString(pNode, _T("Texture"), nglString::Empty)));
  return true;
}

nuiXMLNode* nuiImageDecoration::Serialize(nuiXMLNode* pNode)
{
  pNode->SetName(_T("nuiImageDecoration"));
  pNode->SetAttribute(_T("ClientRect"), mClientRect.GetValue());
  
  pNode->SetAttribute(_T("Texture"), GetTexturePath());
  return pNode;
}

bool nuiImageDecoration::GetRepeatX() const
{
  return mRepeatX;
}


void nuiImageDecoration::SetRepeatX(bool set)
{
  mRepeatX = set;
  if (mpTexture && set)
    mpTexture->SetWrapS(GL_REPEAT);
  else if (mpTexture && !set)
    mpTexture->SetWrapS(GL_CLAMP);
}


bool nuiImageDecoration::GetRepeatY() const
{
  return mRepeatY;  
}


void nuiImageDecoration::SetRepeatY(bool set)
{
  mRepeatY = set;  
  if (mpTexture && set)
    mpTexture->SetWrapT(GL_REPEAT);
  else if (mpTexture && !set)
    mpTexture->SetWrapT(GL_CLAMP);
}




nuiPosition nuiImageDecoration::GetPosition()
{
  return mPosition;
}


void nuiImageDecoration::SetPosition(nuiPosition pos)
{
  mPosition = pos;
  Changed();
}


nglPath nuiImageDecoration::GetTexturePath() const
{
  if (HasProperty(_T("Texture")))
    return GetProperty(_T("Texture"));
  
  return mpTexture->GetSource();
}

void nuiImageDecoration::SetTexturePath(nglPath path)
{
  SetProperty(_T("Texture"), path.GetPathName());
  nuiTexture* pOld = mpTexture;
  mpTexture = nuiTexture::GetTexture(path);
  if (!mpTexture || !mpTexture->IsValid())
  {
    NGL_OUT(_T("nuiImageDecoration::SetTexturePath warning : could not load graphic resource '%ls'\n"), path.GetChars());
    return;
  }
  
  if (GetSourceClientRect() == nuiRect(0,0,0,0))
    SetSourceClientRect(nuiRect(0, 0, mpTexture->GetWidth(), mpTexture->GetHeight()));
  if (pOld)
    pOld->Release();
  
  if (mRepeatX)
    mpTexture->SetWrapS(GL_REPEAT);
  else
    mpTexture->SetWrapS(GL_CLAMP);
  
  if (mRepeatY)
    mpTexture->SetWrapT(GL_REPEAT);
  else
    mpTexture->SetWrapT(GL_CLAMP);
  
  Changed();
}

const nuiColor& nuiImageDecoration::GetColor() const
{
  return mColor;
}

void nuiImageDecoration::SetColor(const nuiColor& rColor)
{
  mColor = rColor;
  Changed();
}

void nuiImageDecoration::Draw(nuiDrawContext* pContext, nuiWidget* pWidget, const nuiRect& rDestRect)
{
  if (!mpTexture || !mpTexture->GetImage() || !mpTexture->GetImage()->GetPixelSize())
    return;
  
  pContext->PushState();
  pContext->ResetState();
  
  nuiRect rect = mClientRect;
  
  rect.SetPosition(mPosition, rDestRect);
  rect.RoundToBelow();
  
  pContext->EnableTexturing(true);
  pContext->EnableBlending(true);
  pContext->SetBlendFunc(nuiBlendTransp);
  pContext->SetTexture(mpTexture);
  nuiColor col(mColor);
  if (mUseWidgetAlpha && pWidget)
    col.Alpha() *= pWidget->GetAlpha();
  
  pContext->SetFillColor(col);
  
  nuiRect SrcRect = rect;
  if (mRepeatX)
    SrcRect.SetWidth(rDestRect.GetWidth());
  if (mRepeatY)
    SrcRect.SetHeight(rDestRect.GetHeight());
  if (mRepeatX || mRepeatY)
    SrcRect.MoveTo(0, 0);
  
  pContext->DrawImage(rDestRect, SrcRect);
  
  pContext->PopState();
}





const nuiRect& nuiImageDecoration::GetSourceClientRect() const
{
  return mClientRect;
}

void nuiImageDecoration::SetSourceClientRect(const nuiRect& rRect)
{
  mClientRect = rRect;
  Changed();
}


nuiSize nuiImageDecoration::GetBorder(nuiPosition position, const nuiWidget* pWidget) const
{
  if (!mBorderEnabled)
    return 0;
  
  nuiSize w = 1.0, h = 1.0;
  mpTexture->TextureToImageCoord(w, h);
  switch (position)
  {
    case nuiLeft:
      return mClientRect.Left();
      break;
    case nuiRight:
      return w - mClientRect.Right();
      break;
    case nuiTop:
      return mClientRect.Top();
      break;
    case nuiBottom:
      return h - mClientRect.Bottom();
      break;
    case nuiFillHorizontal:
      return w - mClientRect.GetWidth();
      break;
    case nuiFillVertical:
      return h - mClientRect.GetHeight();
      break;
    case nuiNoPosition: break;
    case nuiTopLeft: break;
    case nuiTopRight: break;
    case nuiBottomLeft: break;
    case nuiBottomRight: break;
    case nuiCenter: break;
    case nuiTile: break;
    case nuiFill: break;
    case nuiFillLeft: break;
    case nuiFillRight: break;
    case nuiFillTop: break;
    case nuiFillBottom: break;
  }
  //we should'nt arrive here
  return NULL;
}


nuiTexture* nuiImageDecoration::GetTexture() const
{
  return mpTexture;
}



nuiRect nuiImageDecoration::GetIdealClientRect(const nuiWidget* pWidget) const
{
  return mClientRect.Size();
}




