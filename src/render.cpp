#include "render.h"
#include "atom/atom.h"
#include "core/core.h"
#include "core/formula.h"

using namespace tex;

const color TeXRender::_defaultcolor = black;
float TeXRender::_defaultSize = -1;
float TeXRender::_magFactor = 0;

TeXRender::TeXRender(const shared_ptr<Box> b, float textSize, bool trueValues) {
    _box = b;
    if (_defaultSize != -1) {
        _textSize = _defaultSize;
    }
    if (_magFactor != 0) {
        _textSize = textSize * abs(_magFactor);
    } else {
        _textSize = textSize;
    }
    if (!trueValues) {
        _insets.top += (int)(0.18f * textSize);
        _insets.bottom += (int)(0.18f * textSize);
        _insets.left += (int)(0.18f * textSize);
        _insets.right += (int)(0.18f * textSize);
    }
}

float TeXRender::getTextSize() const {
    return _textSize;
}

int TeXRender::getHeight() const {
    return (int)(_box->_height * _textSize + 0.99f +
                 _insets.top + _box->_depth * _textSize + 0.99f +
                 _insets.bottom);
}

int TeXRender::getDepth() const {
    return (int)(_box->_depth * _textSize + 0.99f + _insets.bottom);
}

int TeXRender::getWidth() const {
    return (int)(_box->_width * _textSize + 0.99f + _insets.left + _insets.right);
}

float TeXRender::getBaseline() const {
    return ((_box->_height * _textSize + 0.99f + _insets.top) /
            ((_box->_height + _box->_depth) * _textSize + 0.99f + _insets.top + _insets.bottom));
}

void TeXRender::setTextSize(float textSize) {
    _textSize = textSize;
}

void TeXRender::setForeground(color fg) {
    _fg = fg;
}

Insets TeXRender::getInsets() {
    return _insets;
}

void TeXRender::setInsets(const Insets& insets, bool trueval) {
    _insets = insets;
    if (!trueval) {
        _insets.top += (int)(0.18f * _textSize);
        _insets.bottom += (int)(0.18f * _textSize);
        _insets.left += (int)(0.18f * _textSize);
        _insets.right += (int)(0.18f * _textSize);
    }
}

void TeXRender::setWidth(int w, int align) {
    float diff = w - getWidth();
    // FIXME
    // only care if new width larger than old
    if (diff > 0) {
        _box = shared_ptr<Box>(new HorizontalBox(_box, (float)w, align));
    }
}

void TeXRender::setHeight(int h, int align) {
    float diff = h - getHeight();
    // FIXME
    // only care if new height larger than old
    if (diff > 0) {
        _box = shared_ptr<Box>(new VerticalBox(_box, diff, align));
    }
}

void TeXRender::draw(_out_ Graphics2D& g2, int x, int y) {
    color old = g2.getColor();
    g2.scale(_textSize, _textSize);
    if (!istrans(_fg)) {
        g2.setColor(_fg);
    } else {
        g2.setColor(_defaultcolor);
    }

    // draw formula box
    _box->draw(g2, (x + _insets.left) / _textSize, (y + _insets.top) / _textSize + _box->_height);

    // restore
    g2.reset();
    g2.setColor(old);
}

DefaultTeXFont* TeXRenderBuilder::createFont(float s, int type) {
    DefaultTeXFont* tf = new DefaultTeXFont(s);
    if (type == 0) tf->setSs(false);
    if ((type & ROMAN) != 0) tf->setRoman(true);
    if ((type & TYPEWRITER) != 0) tf->setTt(true);
    if ((type & SANSSERIF) != 0) tf->setSs(true);
    if ((type & ITALIC) != 0) tf->setIt(true);
    if ((type & BOLD) != 0) tf->setBold(true);
    return tf;
}

TeXRender* TeXRenderBuilder::build(TeXFormula& f) {
    return build(f._root);
}

TeXRender* TeXRenderBuilder::build(const shared_ptr<Atom>& fc) {
    shared_ptr<Atom> f = fc;
    if (f == nullptr) {
        f = shared_ptr<Atom>(new EmptyAtom());
    }
    if (_style == -1) {
        throw ex_invalid_state("A style is required, call function setStyle before build");
    }
    if (_textSize == -1) {
        throw ex_invalid_state("A size is required, call function setSize before build");
    }

    DefaultTeXFont* font = (_type == -1) ? new DefaultTeXFont(_textSize) : createFont(_textSize, _type);
    shared_ptr<TeXFont> tf(font);
    TeXEnvironment* te = nullptr;
    if (_widthUnit != -1 && _textWidth != 0) {
        te = new TeXEnvironment(_style, tf, _widthUnit, _textWidth);
    } else {
        te = new TeXEnvironment(_style, tf);
    }

    if (_lineSpaceUnit != -1) {
        te->setInterline(_lineSpaceUnit, _lineSpace);
    }

    auto box = f->createBox(*te);
    TeXRender* ti = nullptr;
    if (_widthUnit != -1 && _textWidth != 0) {
        HorizontalBox* hb = nullptr;
        if (_lineSpaceUnit != -1 && _lineSpace != 0) {
            float il = _lineSpace * SpaceAtom::getFactor(_lineSpaceUnit, *te);
            auto b = FormulaBreaker::split(box, te->getTextWidth(), il);
            hb = new HorizontalBox(b, _isMaxWidth ? b->_width : te->getTextWidth(), _align);
        } else {
            hb = new HorizontalBox(box, _isMaxWidth ? box->_width : te->getTextWidth(), _align);
        }
        ti = new TeXRender(shared_ptr<Box>(hb), _textSize, _trueValues);
    } else {
        ti = new TeXRender(box, _textSize, _trueValues);
    }
    if (!istrans(_fg)) {
        ti->setForeground(_fg);
    }
    ti->_iscolored = te->_isColored;

    delete te;
    return ti;
}
