#include "geometry.hpp"
#include <gtest/gtest.h>


#include <gtest/gtest.h>

#include "geometry.hpp"

#include <vector>

using namespace geometry;

namespace {
constexpr double kEps = 1e-9;

// Helper: compare two doubles with tolerance
void ExpectNear(double actual, double expected, double eps = kEps) { EXPECT_NEAR(actual, expected, eps); }
}  // namespace

// ============================================================
// Point2D
// ============================================================

TEST(Point2DTest, DefaultConstructor) {
    Point2D p;
    ExpectNear(p.x, 0.0);
    ExpectNear(p.y, 0.0);
}

TEST(Point2DTest, ParameterizedConstructor) {
    Point2D p{3.0, 4.0};
    ExpectNear(p.x, 3.0);
    ExpectNear(p.y, 4.0);
}

TEST(Point2DTest, ComparisonOperators) {
    Point2D a{1.0, 1.0};
    Point2D b{2.0, 2.0};
    Point2D c{1.0, 1.0};

    EXPECT_TRUE(a < b);
    EXPECT_FALSE(b < a);
    EXPECT_FALSE(a < c);
    EXPECT_TRUE(a == c);
    EXPECT_FALSE(a == b);
}

TEST(Point2DTest, ArithmeticOperators) {
    Point2D a{1.0, 2.0};
    Point2D b{3.0, 4.0};

    Point2D sum = a + b;
    ExpectNear(sum.x, 4.0);
    ExpectNear(sum.y, 6.0);

    Point2D diff = b - a;
    ExpectNear(diff.x, 2.0);
    ExpectNear(diff.y, 2.0);

    Point2D scaled = a * 2.0;
    ExpectNear(scaled.x, 2.0);
    ExpectNear(scaled.y, 4.0);

    Point2D divided = b / 2.0;
    ExpectNear(divided.x, 1.5);
    ExpectNear(divided.y, 2.0);
}

TEST(Point2DTest, DotAndCross) {
    Point2D a{1.0, 0.0};
    Point2D b{0.0, 1.0};

    ExpectNear(a.Dot(b), 0.0);
    ExpectNear(a.Cross(b), 1.0);
    ExpectNear(b.Cross(a), -1.0);

    Point2D c{2.0, 3.0};
    Point2D d{4.0, 5.0};
    ExpectNear(c.Dot(d), 2.0 * 4.0 + 3.0 * 5.0);
    ExpectNear(c.Cross(d), 2.0 * 5.0 - 3.0 * 4.0);
}

TEST(Point2DTest, LengthAndDistance) {
    Point2D p{3.0, 4.0};
    ExpectNear(p.Length(), 5.0);

    Point2D a{0.0, 0.0};
    Point2D b{3.0, 4.0};
    ExpectNear(a.DistanceTo(b), 5.0);
    ExpectNear(b.DistanceTo(a), 5.0);
}

TEST(Point2DTest, Normalize) {
    Point2D p{3.0, 4.0};
    Point2D n = p.Normalize();
    ExpectNear(n.x, 0.6);
    ExpectNear(n.y, 0.8);
    ExpectNear(n.Length(), 1.0);
}

TEST(Point2DTest, NormalizeZeroVector) {
    Point2D zero{0.0, 0.0};
    Point2D n = zero.Normalize();
    ExpectNear(n.x, 0.0);
    ExpectNear(n.y, 0.0);
}

// ============================================================
// BoundingBox
// ============================================================

TEST(BoundingBoxTest, Construction) {
    BoundingBox bb{0.0, 0.0, 10.0, 5.0};
    ExpectNear(bb.min_x, 0.0);
    ExpectNear(bb.min_y, 0.0);
    ExpectNear(bb.max_x, 10.0);
    ExpectNear(bb.max_y, 5.0);
}

TEST(BoundingBoxTest, WidthHeightCenter) {
    BoundingBox bb{0.0, 0.0, 10.0, 6.0};
    ExpectNear(bb.Width(), 10.0);
    ExpectNear(bb.Height(), 6.0);

    Point2D c = bb.Center();
    ExpectNear(c.x, 5.0);
    ExpectNear(c.y, 3.0);
}

TEST(BoundingBoxTest, Overlaps) {
    BoundingBox a{0.0, 0.0, 10.0, 10.0};
    BoundingBox b{5.0, 5.0, 15.0, 15.0};
    BoundingBox c{20.0, 20.0, 30.0, 30.0};
    BoundingBox d{10.0, 0.0, 20.0, 10.0};  // touching edge

    EXPECT_TRUE(a.Overlaps(b));
    EXPECT_TRUE(b.Overlaps(a));
    EXPECT_FALSE(a.Overlaps(c));
    EXPECT_FALSE(c.Overlaps(a));
    EXPECT_TRUE(a.Overlaps(d));  // touching counts as overlap in this impl
}

// ============================================================
// Line
// ============================================================

TEST(LineTest, Construction) {
    Line l{{0.0, 0.0}, {3.0, 4.0}};
    ExpectNear(l.start.x, 0.0);
    ExpectNear(l.start.y, 0.0);
    ExpectNear(l.end.x, 3.0);
    ExpectNear(l.end.y, 4.0);
}

TEST(LineTest, Length) {
    Line l{{0.0, 0.0}, {3.0, 4.0}};
    ExpectNear(l.Length(), 5.0);
}

TEST(LineTest, Direction) {
    Line l{{0.0, 0.0}, {3.0, 4.0}};
    Point2D d = l.Direction();
    ExpectNear(d.x, 0.6);
    ExpectNear(d.y, 0.8);
}

TEST(LineTest, BoundBox) {
    Line l{{1.0, 5.0}, {3.0, 2.0}};
    BoundingBox bb = l.BoundBox();
    ExpectNear(bb.min_x, 1.0);
    ExpectNear(bb.min_y, 2.0);
    ExpectNear(bb.max_x, 3.0);
    ExpectNear(bb.max_y, 5.0);
}

TEST(LineTest, Height) {
    Line l{{0.0, 1.0}, {0.0, 7.0}};
    ExpectNear(l.Height(), 7.0);
}

TEST(LineTest, Center) {
    Line l{{0.0, 0.0}, {4.0, 2.0}};
    Point2D c = l.Center();
    ExpectNear(c.x, 2.0);
    ExpectNear(c.y, 1.0);
}

TEST(LineTest, Vertices) {
    Line l{{1.0, 2.0}, {3.0, 4.0}};
    auto verts = l.Vertices();
    ASSERT_EQ(verts.size(), 2u);
    EXPECT_TRUE(verts[0] == Point2D(1.0, 2.0));
    EXPECT_TRUE(verts[1] == Point2D(3.0, 4.0));
}

// ============================================================
// Triangle
// ============================================================

TEST(TriangleTest, Area) {
    Triangle t{{0.0, 0.0}, {4.0, 0.0}, {0.0, 3.0}};
    ExpectNear(t.Area(), 6.0);
}

TEST(TriangleTest, AreaDegenerate) {
    Triangle t{{0.0, 0.0}, {1.0, 1.0}, {2.0, 2.0}};
    ExpectNear(t.Area(), 0.0);
}

TEST(TriangleTest, BoundBox) {
    Triangle t{{1.0, 1.0}, {5.0, 3.0}, {2.0, 7.0}};
    BoundingBox bb = t.BoundBox();
    ExpectNear(bb.min_x, 1.0);
    ExpectNear(bb.min_y, 1.0);
    ExpectNear(bb.max_x, 5.0);
    ExpectNear(bb.max_y, 7.0);
}

TEST(TriangleTest, Height) {
    Triangle t{{0.0, 1.0}, {0.0, 5.0}, {0.0, 3.0}};
    ExpectNear(t.Height(), 5.0);
}

TEST(TriangleTest, Center) {
    Triangle t{{0.0, 0.0}, {3.0, 0.0}, {0.0, 3.0}};
    Point2D c = t.Center();
    ExpectNear(c.x, 1.0);
    ExpectNear(c.y, 1.0);
}

TEST(TriangleTest, Vertices) {
    Triangle t{{1.0, 2.0}, {3.0, 4.0}, {5.0, 6.0}};
    auto verts = t.Vertices();
    ASSERT_EQ(verts.size(), 3u);
    EXPECT_TRUE(verts[0] == Point2D(1.0, 2.0));
    EXPECT_TRUE(verts[1] == Point2D(3.0, 4.0));
    EXPECT_TRUE(verts[2] == Point2D(5.0, 6.0));
}

// ============================================================
// Rectangle
// ============================================================

TEST(RectangleTest, Construction) {
    Rectangle r{{1.0, 2.0}, 5.0, 3.0};
    ExpectNear(r.bottom_left.x, 1.0);
    ExpectNear(r.bottom_left.y, 2.0);
    ExpectNear(r.width, 5.0);
    ExpectNear(r.height, 3.0);
}

TEST(RectangleTest, TopRight) {
    Rectangle r{{1.0, 2.0}, 5.0, 3.0};
    Point2D tr = r.TopRight();
    ExpectNear(tr.x, 6.0);
    ExpectNear(tr.y, 5.0);
}

TEST(RectangleTest, BoundBox) {
    Rectangle r{{1.0, 2.0}, 5.0, 3.0};
    BoundingBox bb = r.BoundBox();
    ExpectNear(bb.min_x, 1.0);
    ExpectNear(bb.min_y, 2.0);
    ExpectNear(bb.max_x, 6.0);
    ExpectNear(bb.max_y, 5.0);
}

TEST(RectangleTest, Vertices) {
    Rectangle r{{0.0, 0.0}, 4.0, 3.0};
    auto verts = r.Vertices();
    ASSERT_EQ(verts.size(), 4u);
    EXPECT_TRUE(verts[0] == Point2D(0.0, 0.0));
    EXPECT_TRUE(verts[1] == Point2D(4.0, 0.0));
    EXPECT_TRUE(verts[2] == Point2D(4.0, 3.0));
    EXPECT_TRUE(verts[3] == Point2D(0.0, 3.0));
}

TEST(RectangleTest, Height) {
    Rectangle r{{0.0, 1.0}, 4.0, 3.0};
    ExpectNear(r.Height(), 4.0);  // bottom_left.y + height
}

TEST(RectangleTest, Center) {
    Rectangle r{{0.0, 0.0}, 4.0, 2.0};
    Point2D c = r.Center();
    ExpectNear(c.x, 2.0);
    ExpectNear(c.y, 1.0);
}

// ============================================================
// RegularPolygon
// ============================================================

TEST(RegularPolygonTest, VerticesCount) {
    RegularPolygon p{{0.0, 0.0}, 5.0, 6};
    auto verts = p.Vertices();
    EXPECT_EQ(verts.size(), 6u);
}

TEST(RegularPolygonTest, VerticesOnCircle) {
    RegularPolygon p{{0.0, 0.0}, 5.0, 8};
    for (const auto &v : p.Vertices()) {
        ExpectNear(v.Length(), 5.0);
    }
}

TEST(RegularPolygonTest, SquareVertices) {
    // A 4-sided regular polygon centered at origin with radius 1
    RegularPolygon p{{0.0, 0.0}, 1.0, 4};
    auto verts = p.Vertices();
    ASSERT_EQ(verts.size(), 4u);

    // First vertex at angle 0 -> (1, 0)
    ExpectNear(verts[0].x, 1.0);
    ExpectNear(verts[0].y, 0.0);

    // Second vertex at angle pi/2 -> (0, 1)
    ExpectNear(verts[1].x, 0.0);
    ExpectNear(verts[1].y, 1.0);
}

TEST(RegularPolygonTest, BoundBox) {
    RegularPolygon p{{1.0, 2.0}, 3.0, 5};
    BoundingBox bb = p.BoundBox();
    ExpectNear(bb.min_x, -2.0);
    ExpectNear(bb.min_y, -1.0);
    ExpectNear(bb.max_x, 4.0);
    ExpectNear(bb.max_y, 5.0);
}

TEST(RegularPolygonTest, Height) {
    RegularPolygon p{{0.0, 1.0}, 2.0, 6};
    ExpectNear(p.Height(), 3.0);
}

TEST(RegularPolygonTest, Center) {
    RegularPolygon p{{3.0, 4.0}, 2.0, 6};
    Point2D c = p.Center();
    ExpectNear(c.x, 3.0);
    ExpectNear(c.y, 4.0);
}

// ============================================================
// Circle
// ============================================================

TEST(CircleTest, Construction) {
    Circle c{{1.0, 2.0}, 5.0};
    ExpectNear(c.center_p.x, 1.0);
    ExpectNear(c.center_p.y, 2.0);
    ExpectNear(c.radius, 5.0);
}

TEST(CircleTest, BoundBox) {
    Circle c{{0.0, 0.0}, 3.0};
    BoundingBox bb = c.BoundBox();
    ExpectNear(bb.min_x, -3.0);
    ExpectNear(bb.min_y, -3.0);
    ExpectNear(bb.max_x, 3.0);
    ExpectNear(bb.max_y, 3.0);
}

TEST(CircleTest, Height) {
    Circle c{{0.0, 2.0}, 5.0};
    ExpectNear(c.Height(), 7.0);
}

TEST(CircleTest, Center) {
    Circle c{{4.0, 5.0}, 3.0};
    Point2D ctr = c.Center();
    ExpectNear(ctr.x, 4.0);
    ExpectNear(ctr.y, 5.0);
}

TEST(CircleTest, VerticesDefaultCount) {
    Circle c{{0.0, 0.0}, 1.0};
    auto verts = c.Vertices();
    EXPECT_EQ(verts.size(), 30u);
    for (const auto &v : verts) {
        ExpectNear(v.Length(), 1.0);
    }
}

TEST(CircleTest, VerticesCustomCount) {
    Circle c{{0.0, 0.0}, 2.0};
    auto verts = c.Vertices(8);
    EXPECT_EQ(verts.size(), 8u);
    for (const auto &v : verts) {
        ExpectNear(v.Length(), 2.0);
    }
}

// ============================================================
// Polygon
// ============================================================

TEST(PolygonTest, ConstructionAndVertices) {
    std::vector<Point2D> pts = {{0.0, 0.0}, {4.0, 0.0}, {4.0, 3.0}, {0.0, 3.0}};
    Polygon poly{pts};

    auto verts = poly.Vertices();
    ASSERT_EQ(verts.size(), 4u);
    EXPECT_TRUE(verts[0] == Point2D(0.0, 0.0));
    EXPECT_TRUE(verts[1] == Point2D(4.0, 0.0));
    EXPECT_TRUE(verts[2] == Point2D(4.0, 3.0));
    EXPECT_TRUE(verts[3] == Point2D(0.0, 3.0));
}

TEST(PolygonTest, BoundBox) {
    std::vector<Point2D> pts = {{1.0, 1.0}, {5.0, 2.0}, {3.0, 7.0}, {-1.0, 4.0}};
    Polygon poly{pts};
    BoundingBox bb = poly.BoundBox();
    ExpectNear(bb.min_x, -1.0);
    ExpectNear(bb.min_y, 1.0);
    ExpectNear(bb.max_x, 5.0);
    ExpectNear(bb.max_y, 7.0);
}

TEST(PolygonTest, Height) {
    std::vector<Point2D> pts = {{0.0, 0.0}, {4.0, 0.0}, {4.0, 3.0}, {0.0, 3.0}};
    Polygon poly{pts};
    ExpectNear(poly.Height(), 3.0);
}

TEST(PolygonTest, Center) {
    std::vector<Point2D> pts = {{0.0, 0.0}, {4.0, 0.0}, {4.0, 3.0}, {0.0, 3.0}};
    Polygon poly{pts};
    Point2D c = poly.Center();
    ExpectNear(c.x, 2.0);
    ExpectNear(c.y, 1.5);
}

TEST(PolygonTest, LinesClosesLoop) {
    std::vector<Point2D> pts = {{0.0, 0.0}, {4.0, 0.0}, {4.0, 3.0}, {0.0, 3.0}};
    Polygon poly{pts};
    auto lines = poly.Lines();

    ASSERT_EQ(lines.x.size(), 5u);
    ASSERT_EQ(lines.y.size(), 5u);

    // First point equals last (closed loop)
    ExpectNear(lines.x.front(), lines.x.back());
    ExpectNear(lines.y.front(), lines.y.back());
}

// ============================================================
// Shape variant dispatch
// ============================================================

TEST(ShapeVariantTest, HoldsCorrectAlternative) {
    Shape s = Line{{0.0, 0.0}, {1.0, 1.0}};
    EXPECT_TRUE(std::holds_alternative<Line>(s));

    s = Circle{{0.0, 0.0}, 1.0};
    EXPECT_TRUE(std::holds_alternative<Circle>(s));
}

TEST(ShapeVariantTest, VisitHeight) {
    std::vector<Shape> shapes = {
        Line{{0.0, 1.0}, {0.0, 5.0}},
        Circle{{0.0, 0.0}, 2.0},
        Triangle{{0.0, 0.0}, {0.0, 3.0}, {0.0, 1.0}},
    };

    std::vector<double> heights;
    for (const auto &s : shapes) {
        heights.push_back(std::visit([](const auto &shape) { return shape.Height(); }, s));
    }

    ExpectNear(heights[0], 5.0);
    ExpectNear(heights[1], 2.0);
    ExpectNear(heights[2], 3.0);
}

// ============================================================
// Formatters
// ============================================================

TEST(FormatterTest, Point2D) {
    Point2D p{1.5, 2.25};
    auto s = std::format("{}", p);
    EXPECT_EQ(s, "(1.50, 2.25)");
}

TEST(FormatterTest, Line) {
    Line l{{0.0, 0.0}, {1.0, 1.0}};
    auto s = std::format("{}", l);
    EXPECT_EQ(s, "Line((0.00, 0.00), (1.00, 1.00))");
}

TEST(FormatterTest, Circle) {
    Circle c{{0.0, 0.0}, 5.0};
    auto s = std::format("{}", c);
    EXPECT_EQ(s, "Circle(center=(0.00, 0.00), r=5.00)");
}

TEST(FormatterTest, Triangle) {
    Triangle t{{0.0, 0.0}, {1.0, 0.0}, {0.0, 1.0}};
    auto s = std::format("{}", t);
    EXPECT_EQ(s, "Triangle((0.00, 0.00), (1.00, 0.00), (0.00, 1.00))");
}

TEST(FormatterTest, Rectangle) {
    Rectangle r{{0.0, 0.0}, 4.0, 3.0};
    auto s = std::format("{}", r);
    EXPECT_EQ(s, "Rectangle(bottom_left=(0.00, 0.00), w=4.00, h=3.00)");
}

TEST(FormatterTest, RegularPolygon) {
    RegularPolygon p{{0.0, 0.0}, 2.0, 5};
    auto s = std::format("{}", p);
    EXPECT_EQ(s, "RegularPolygon(center=(0.00, 0.00), r=2.00, sides=5)");
}

TEST(FormatterTest, ShapeVariant) {
    Shape s = Circle{{0.0, 0.0}, 5.0};
    auto str = std::format("{}", s);
    EXPECT_EQ(str, "Circle(center=(0.00, 0.00), r=5.00)");
}

TEST(FormatterTest, PointVectorOneLine) {
    std::vector<Point2D> pts = {{0.0, 0.0}, {1.0, 1.0}};
    auto s = std::format("{}", pts);
    EXPECT_EQ(s, "[(0.00, 0.00), (1.00, 1.00)]");
}

TEST(FormatterTest, PointVectorNewLine) {
    std::vector<Point2D> pts = {{0.0, 0.0}, {1.0, 1.0}};
    auto s = std::format("{:new_line}", pts);
    EXPECT_EQ(s, "\t(0.00, 0.00)\n\t(1.00, 1.00)");
}

TEST(FormatterTest, PointVectorEmpty) {
    std::vector<Point2D> pts;
    auto s = std::format("{}", pts);
    EXPECT_EQ(s, "[]");
}
