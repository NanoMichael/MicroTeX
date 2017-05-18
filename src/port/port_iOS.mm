#ifdef __APPLE_CC__
#if defined (__clang__)
    #include "port.h"
#elif defined (__GNUC__)
    #include "port/port.h"
#endif // defined
#include "common.h"
#include <sstream>

using namespace std;
using namespace tex;
using namespace tex::port;

/**************** font *************/


Font::~Font() {

}

Font::Font() : _size(0) {}

UIFont* Font::creatIFont(int style, float size) {
    UIFont *iFont = nil;
    switch (style) {
        case PLAIN:
            iFont = [UIFont systemFontOfSize:size];
            break;
            
        case BOLD:
            iFont = [UIFont boldSystemFontOfSize:size];
            break;
            
        case ITALIC:
            iFont = [UIFont italicSystemFontOfSize:size];
            break;
            
        case BOLDITALIC:
            iFont = [UIFont systemFontOfSize:size];
            break;
            
        default:
            iFont = [UIFont systemFontOfSize:size];
            NSLog(@"specified font style not available!");
            break;
    }
    return iFont;
}

Font::Font(const string& name, int style, float size) : _size(size) {
    UIFont *iFont = creatIFont(style, size);
    _style = style;
    _size = size;
    _iFont = iFont;
}

UIFont* Font::getFont() const {
    return _iFont;
}

Font::Font(UIFont *iFont, float size) {
    _iFont = iFont;
    _size = size;
}

float Font::getSize() const {
	return _size;
}

Font Font::deriveFont(int style) const {
    if (style == _style) return *this;

    UIFont *iFont = creatIFont(style, _size);
    if (!iFont) {
        NSLog(@"specified font style not available!");
    }
    Font f = Font(iFont, _size);
    f._style = style;
    return f;
}

bool Font::operator==(const Font& f) const {
    return _iFont == f._iFont;
}

bool Font::operator!=(const Font& f) const {
    return !(*this == f);
}

Font* Font::create(const string& file, float s) {
    NSString *fontPath = [NSString stringWithUTF8String:file.c_str()];
    NSURL *fontUrl = [NSURL fileURLWithPath:fontPath];
    
    CGDataProviderRef fontDataProvider = CGDataProviderCreateWithURL((__bridge CFURLRef)fontUrl);
    CGFontRef fontRef = CGFontCreateWithDataProvider(fontDataProvider);
    CGDataProviderRelease(fontDataProvider);
    
    CFErrorRef error;
    CTFontManagerRegisterGraphicsFont(fontRef, &error);
    
    NSString *fontName = CFBridgingRelease(CGFontCopyPostScriptName(fontRef));
    UIFont *iFont = [UIFont fontWithName:fontName size:s];
    
    CTFontManagerUnregisterGraphicsFont(fontRef, &error);
    CGFontRelease(fontRef);

    Font* f = new Font();
    f->_iFont = iFont;
    f->_size = s;
    return f;
}

/****************************************************
 *               Graphics2D implementation          *
 ****************************************************/

Graphics2D::Graphics2D(CGContextRef ref) {
    _t = new float[9]();
    _t[SX] = _t[SY] = 1;
    _color = black;
    _ref = ref;
}

Graphics2D::~Graphics2D() {

}

void Graphics2D::setColor(color color) {
    _color = color;
    _cR = ((CGFloat)((_color & 0xFF0000) >> 16)) / 255.0;
    _cG = ((CGFloat)((_color & 0xFF00) >> 8))/ 255.0;
    _cB= ((CGFloat)(_color & 0xFF)) / 255.0;
}

color Graphics2D::getColor() const {
    return _color;
}

void Graphics2D::setStroke(const Stroke& s) {
    _stroke = s;

    CGContextSetLineWidth(_ref, s.lineWidth);
    
    CGLineCap cap;
    switch (s.cap) {
        case CAP_BUTT:
            cap = kCGLineCapButt;
            break;
        case CAP_ROUND:
            cap = kCGLineCapRound;
            break;
        case CAP_SQUARE:
            cap = kCGLineCapSquare;
            break;
    }
    CGContextSetLineCap(_ref, cap);
    
    CGLineJoin join;
    switch (s.join) {
        case JOIN_BEVEL:
            join = kCGLineJoinBevel;
            break;
        case JOIN_ROUND:
            join = kCGLineJoinRound;
            break;
        case JOIN_MITER:
            join = kCGLineJoinMiter;
            break;
    }
    CGContextSetLineJoin(_ref, join);
}

const Stroke& Graphics2D::getStroke() const {
    return _stroke;
}

void Graphics2D::setStrokeWidth(float w) {
    _stroke.lineWidth = w;
    CGContextSetLineWidth(_ref, w);
}

const Font* Graphics2D::getFont() const {
    return _font;
}

void Graphics2D::setFont(const Font* font) {
    _font = font;
}

void Graphics2D::translate(float dx, float dy) {
    _t[TX] += _t[SX] * dx;
    _t[TY] += _t[SY] * dy;
    CGContextTranslateCTM(_ref, dx, dy);
}

void Graphics2D::scale(float sx, float sy) {
    _t[SX] *= sx;
    _t[SY] *= sy;
    CGContextScaleCTM(_ref, sx, sy);
}

void Graphics2D::rotate(float angle) {
    float r = (float) (angle / PI * 180);
    _t[R] += r;
    CGContextRotateCTM(_ref, angle);
}

void Graphics2D::rotate(float angle, float px, float py) {
    float r = (float) (angle / PI * 180);
    _t[R] += r;
    _t[PX] = px * _t[SX] + _t[TX];
    _t[PY] = py * _t[SY] + _t[TY];
    CGContextTranslateCTM(_ref, px, py);
    CGContextRotateCTM(_ref, angle);
    CGContextTranslateCTM(_ref, px, py);
}

void Graphics2D::reset() {
    memset(_t, 0, sizeof(float) * 9);
    _t[SX] = _t[SY] = 1;
//    CGAffineTransform tf0 = CGContextGetCTM(_ref);
//    CGAffineTransform tf1 = CGAffineTransformInvert(tf0);
//    CGContextConcatCTM(_ref, tf1);
//    CGContextSetTextMatrix(_ref, CGAffineTransformIdentity);
////    CGContextTranslateCTM(_ref, 0, self.bounds.size.height);
//    CGContextScaleCTM(_ref, 1.0, -1.0);
}

float Graphics2D::sx() const {
    return _t[SX];
}

float Graphics2D::sy() const {
    return _t[SY];
}

float Graphics2D::tx() const {
    return _t[TX];
}

float Graphics2D::ty() const {
    return _t[TY];
}

float Graphics2D::r() const {
    return _t[R];
}

float Graphics2D::px() const {
    return _t[PX];
}

float Graphics2D::py() const {
    return _t[PY];
}

void Graphics2D::drawChar(wchar_t c, float x, float y) {
    wchar_t str[] { c, L'\0' };
    string cStr;
    wide2utf8(str, cStr);
    NSString *iStr = [NSString stringWithUTF8String:cStr.c_str()];
    [iStr drawAtPoint:CGPointMake(x, y - _font->_iFont.ascender) withAttributes:@{NSFontAttributeName : _font->_iFont,
                                                                                  NSForegroundColorAttributeName : [UIColor colorWithRed:_cR green:_cG blue:_cB alpha:1.0]}];
}

void Graphics2D::drawText(const wstring& c, float x, float y) {
    string str = wide2utf8(c.c_str());
    NSString *iStr = [NSString stringWithUTF8String:str.c_str()];
    [iStr drawAtPoint:CGPointMake(x, y - _font->_iFont.ascender) withAttributes:@{NSFontAttributeName : _font->_iFont,
                                                                                  NSForegroundColorAttributeName : [UIColor colorWithRed:_cR green:_cG blue:_cB alpha:1.0]}];
}

void Graphics2D::drawLine(float x1, float y1, float x2, float y2) {
    CGContextMoveToPoint(_ref, x1, y1);
    CGContextAddLineToPoint(_ref, x2, y2);
    CGContextSetRGBStrokeColor(_ref, _cR, _cG, _cB, 1.0);
    CGContextStrokePath(_ref);
}

void Graphics2D::drawRect(float x, float y, float w, float h) {
    CGContextAddRect(_ref, CGRectMake(x, y, w, h));
    CGContextSetRGBStrokeColor(_ref, _cR, _cG, _cB, 1.0);
    CGContextStrokePath(_ref);
}

void Graphics2D::fillRect(float x, float y, float w, float h) {
    CGContextAddRect(_ref, CGRectMake(x, y, w, h));
    CGContextSetRGBFillColor(_ref, _cR, _cG, _cB, 1.0);
    CGContextFillPath(_ref);
}

void Graphics2D::drawRoundRect(float x, float y, float w, float h, float rx, float ry) {
    CGFloat radius = (w + h) * 0.05;
    CGContextMoveToPoint(_ref, (x + radius), y);
    
    CGContextAddLineToPoint(_ref, (x + w - radius), y);
    CGContextAddArc(_ref, (x + w - radius), (y + radius), radius, -0.5 * M_PI, 0.0, 0);
    
    CGContextAddLineToPoint(_ref, (x + w), (y + h - radius));
    CGContextAddArc(_ref, (x + w - radius), (y + h - radius), radius, 0.0, 0.5 * M_PI, 0);
    
    CGContextAddLineToPoint(_ref, (x + radius), (y + h));
    CGContextAddArc(_ref, (x + radius), (y + h - radius), radius, 0.5 * M_PI, M_PI, 0);
    
    CGContextAddLineToPoint(_ref, x, (y + radius));
    CGContextAddArc(_ref, (x + radius), (y + radius), radius, M_PI, 1.5 * M_PI, 0);
    
    CGContextClosePath(_ref);
    
    CGContextSetRGBStrokeColor(_ref, _cR, _cG, _cB, 1.0);
    CGContextDrawPath(_ref, kCGPathStroke);
}

void Graphics2D::fillRoundRect(float x, float y, float w, float h, float rx, float ry) {
    CGFloat radius = (w + h) * 0.05;
    CGContextMoveToPoint(_ref, (x + radius), y);
    
    CGContextAddLineToPoint(_ref, (x + w - radius), y);
    CGContextAddArc(_ref, (x + w - radius), (y + radius), radius, -0.5 * M_PI, 0.0, 0);
    
    CGContextAddLineToPoint(_ref, (x + w), (y + h - radius));
    CGContextAddArc(_ref, (x + w - radius), (y + h - radius), radius, 0.0, 0.5 * M_PI, 0);
    
    CGContextAddLineToPoint(_ref, (x + radius), (y + h));
    CGContextAddArc(_ref, (x + radius), (y + h - radius), radius, 0.5 * M_PI, M_PI, 0);
    
    CGContextAddLineToPoint(_ref, x, (y + radius));
    CGContextAddArc(_ref, (x + radius), (y + radius), radius, M_PI, 1.5 * M_PI, 0);
    
    CGContextClosePath(_ref);
    
    CGContextSetRGBFillColor(_ref, _cR, _cG, _cB, 1.0);
    CGContextDrawPath(_ref, kCGPathFill);
}

TextLayout::~TextLayout() {}

TextLayout::TextLayout() {}

TextLayout::TextLayout(const wstring& src, const Font& font) : _txt(src), _font(font) {

}

port::Rect TextLayout::getBounds() {
    NSString *str = [[NSString alloc] initWithBytes:_txt.data() length:_txt.length() * sizeof(wchar_t) encoding:NSUTF32LittleEndianStringEncoding];
    CGSize iSize = [str sizeWithAttributes:@{NSFontAttributeName : _font._iFont}];
    Rect r = Rect(0, -_font._iFont.ascender, iSize.width, iSize.height);
    return r;
}

void TextLayout::draw(Graphics2D& g2, float x, float y) {
    const Font* prev = g2.getFont();
    g2.setFont(&_font);
    g2.drawText(_txt, x, y);
    g2.setFont(prev);
}

#endif
