#ifndef MICROTEX_QT_TEX_RENDER_H
#define MICROTEX_QT_TEX_RENDER_H

class TeXRender {
public:
  virtual float getTextSize() = 0;
  virtual void setTextSize(float size) = 0;
  virtual void setLaTeX(const std::string& latex) = 0;
  virtual bool isRenderDisplayed() = 0;
  virtual int getRenderWidth() = 0;
  virtual int getRenderHeight() = 0;
  virtual void saveSVG(const char* path) = 0;
};

#endif  // MICROTEX_QT_TEX_RENDER_H
