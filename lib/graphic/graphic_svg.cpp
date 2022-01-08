#include "graphic/graphic_svg.h"
#include <string>
#include <utility>
#include <vector>
#include <cmath>

using namespace std;

namespace tex {

class XMLNode {
private:
  string _tag;
  vector<pair<string, string>> _attrs;
  vector<XMLNode> _children;

public:
  explicit XMLNode(string tag) : _tag(move(tag)) {}

  XMLNode(const XMLNode& node) = default;

  XMLNode(XMLNode&& node) noexcept:
    _tag(move(node._tag)),
    _attrs(move(node._attrs)),
    _children(move(node._children)) {}

  XMLNode& operator=(const XMLNode& node) = default;

  XMLNode& operator=(XMLNode&& node) noexcept {
    _tag = std::move(node._tag);
    _attrs = std::move(node._attrs);
    _children = std::move(node._children);
    return *this;
  }

  XMLNode& attr(string key, string value) {
    _attrs.emplace_back(move(key), move(value));
    return *this;
  }

  XMLNode& child(XMLNode& node) {
    _children.push_back(node);
  }

  XMLNode& child(XMLNode&& node) {
    _children.push_back(move(node));
  }
};

/************************************************************************************/

class Matrix {
private:
  float m[9] = {1, 0, 0, 0, 1, 0, 0, 0, 1};

  void multiply(const float n[9]) {
    m[0] = m[0] * n[0] + m[1] * n[3] + m[2] * n[6];
    m[1] = m[0] * n[1] + m[1] * n[4] + m[2] * n[7];
    m[2] = m[0] * n[2] + m[1] * n[5] + m[2] * n[8];

    m[3] = m[3] * n[0] + m[4] * n[3] + m[5] * n[6];
    m[4] = m[3] * n[1] + m[4] * n[4] + m[5] * n[7];
    m[5] = m[3] * n[2] + m[4] * n[5] + m[5] * n[8];

    m[6] = m[6] * n[0] + m[7] * n[3] + m[8] * n[6];
    m[7] = m[6] * n[1] + m[7] * n[4] + m[8] * n[7];
    m[8] = m[6] * n[2] + m[7] * n[5] + m[8] * n[8];
  }

public:
  bool isIdentity() {
    return m[0] == 1 && m[1] == 0 && m[2] == 0
           && m[3] == 0 && m[4] == 1 && m[5] == 0
           && m[6] == 0 && m[7] == 0 && m[8] == 1;
  }

  void translate(float dx, float dy) {
    const float n[9] = {1, 0, dx, 0, 1, dy, 0, 0, 1};
    multiply(n);
  }

  void scale(float sx, float sy) {
    const float n[9] = {sx, 0, 0, 0, sy, 0, 0, 0, 1};
    multiply(n);
  }

  void rotate(float angle) {
    const float s = std::sin(angle);
    const float c = std::cos(angle);
    const float n[9] = {c, -s, 0, s, c, 0, 0, 0, 1};
    multiply(n);
  }
};

/************************************************************************************/

Graphics2D_svg::Graphics2D_svg() {
  _svg = new XMLNode("svg");
}

void Graphics2D_svg::setColor(color c) {
  _color = c;
}

color Graphics2D_svg::getColor() const {
  return _color;
}

void Graphics2D_svg::setStroke(const Stroke& s) {
  _stroke = s;
}

const Stroke& Graphics2D_svg::getStroke() const {
  return _stroke;
}

void Graphics2D_svg::setStrokeWidth(float w) {
  _stroke.lineWidth = w;
}

void Graphics2D_svg::setDash(const std::vector<float>& dash) {
  // todo
}

std::vector<float> Graphics2D_svg::getDash() {
  // todo
}

sptr<Font> Graphics2D_svg::getFont() const {
  // todo
}

void Graphics2D_svg::setFont(const sptr<Font>& font) {
  // todo
}

void Graphics2D_svg::setFontSize(float size) {
  // todo
}

float Graphics2D_svg::getFontSize() const {
  // todo
  return 0;
}

void Graphics2D_svg::translate(float dx, float dy) {
  auto g = XMLNode("g");
  g.attr("transform", "translate(" + std::to_string(dx) + " " + std::to_string(dy) + ")");
  _svg->child(move(g));
}


}
