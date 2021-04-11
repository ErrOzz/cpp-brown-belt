#include "Common.h"
#include "optional"

using namespace std;

// Этот файл сдаётся на проверку
// Здесь напишите реализацию необходимых классов-потомков `IShape`

namespace Shape {

class Shape : public IShape {
  Point pos_ = {0, 0};
  Size size_ = {0, 0};
  shared_ptr<ITexture> texture_;

  virtual bool IsPointInShape(Point p) const = 0;
  virtual unique_ptr<IShape> NewShape() const = 0;

  optional<pair<Point, Point>> GetIntersect(const Image& image) const {
    const int img_x_sz = static_cast<int>(image.empty() ? 0 : image[0].size());
    const int img_y_sz = static_cast<int>(image.size());
    if (pos_.x >= img_x_sz ||
        pos_.y >= img_y_sz ||
        pos_.x + size_.width < 0 ||
        pos_.y + size_.height < 0) return nullopt;
    int left = max(pos_.x, 0);
    int right = min(pos_.x + size_.width, img_x_sz - 1);
    int top = max(pos_.y, 0);
    int bottom = min(pos_.y + size_.height, img_y_sz - 1);
    return make_pair<Point, Point>({left, top}, {right, bottom});
  }

  inline bool IsPointInTexture(Point p) const {
    if (!texture_) return false;
    return p.x < texture_->GetSize().width && p.y < texture_->GetSize().height;
  }
public:
  Shape() = default;
  virtual unique_ptr<IShape> Clone() const final {
    auto result = NewShape();
    result->SetPosition(pos_);
    result->SetSize(size_);
    result->SetTexture(texture_);
    return result;
  }
  virtual void SetPosition(Point position) final {
    pos_ = move(position);
  }
  virtual Point GetPosition() const final {
    return pos_;
  }
  virtual void SetSize(Size size) final {
    size_ = move(size);
  }
  virtual Size GetSize() const final {
    return size_;
  }
  virtual void SetTexture(shared_ptr<ITexture> texture) final {
    texture_ = move(texture);
  }
  virtual ITexture* GetTexture() const final {
    return texture_.get();
  }
  virtual void Draw(Image& image) const final {
    const auto draw_field = GetIntersect(image);
    if (!draw_field) return;
    for (int y = draw_field->first.y; y <= draw_field->second.y;++y) {
      const int shape_y = y - pos_.y;
      for (int x = draw_field->first.x; x <= draw_field->second.x; ++x) {
        const int shape_x = x - pos_.x;
        const Point shape_point = {shape_x, shape_y};
        if (IsPointInShape(shape_point)) {
          image[y][x] = IsPointInTexture(shape_point) ?
                        texture_->GetImage()[shape_y][shape_x] : '.';
        }
      }
    }
  }
};

class Rectangle : public Shape {
  virtual bool IsPointInShape(Point p) const override {
    return p.x < GetSize().width && p.y < GetSize().height;
  }
  virtual unique_ptr<IShape> NewShape() const override {
    return make_unique<Rectangle>();
  }
public:
  using Shape::Shape;
};

class Ellipse : public Shape {
  virtual bool IsPointInShape(Point p) const override {
    return IsPointInEllipse(p, GetSize());
  }
  virtual unique_ptr<IShape> NewShape() const override {
    return make_unique<Ellipse>();
  }
public:
  using Shape::Shape;
};

}

// Напишите реализацию функции
unique_ptr<IShape> MakeShape(ShapeType shape_type) {
  switch (shape_type) {
    case ShapeType::Rectangle:
    return make_unique<Shape::Rectangle>();
    case ShapeType::Ellipse:
    return make_unique<Shape::Ellipse>();
  }
  return nullptr;
}
