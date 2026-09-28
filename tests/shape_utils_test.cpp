#include "geometry.hpp"
#include "shape_utils.hpp"
#include <gtest/gtest.h>


#include <vector>

using namespace geometry;

TEST(FindAllCollisionsTest, EmptyInputReturnsNothing) {
    std::vector<Shape> shapes;
    auto result = utils::FindAllCollisions(shapes);
    EXPECT_TRUE(result.empty());
}

TEST(FindAllCollisionsTest, SingleShapeReturnsNothing) {
    std::vector<Shape> shapes = {Circle{{0.0, 0.0}, 1.0}};
    auto result = utils::FindAllCollisions(shapes);
    EXPECT_TRUE(result.empty());
}

TEST(FindAllCollisionsTest, NoOverlapReturnsNothing) {
    std::vector<Shape> shapes = {
        Circle{{0.0, 0.0}, 1.0},
        Circle{{10.0, 0.0}, 1.0},
        Circle{{20.0, 0.0}, 1.0},
    };
    auto result = utils::FindAllCollisions(shapes);
    EXPECT_TRUE(result.empty());
}

TEST(FindAllCollisionsTest, TwoOverlappingShapesFound) {
    std::vector<Shape> shapes = {
        Circle{{0.0, 0.0}, 2.0},
        Circle{{1.0, 0.0}, 2.0},
        Circle{{100.0, 100.0}, 1.0},
    };
    auto result = utils::FindAllCollisions(shapes);
    EXPECT_EQ(result.size(), 1u);
}

TEST(FindAllCollisionsTest, AllPairsOverlapNoDuplicates) {
    std::vector<Shape> shapes = {
        Circle{{0.0, 0.0}, 1.0},
        Circle{{0.0, 0.0}, 2.0},
        Circle{{0.0, 0.0}, 3.0},
        Circle{{0.0, 0.0}, 4.0},
    };
    auto result = utils::FindAllCollisions(shapes);
    EXPECT_EQ(result.size(), 6u);
}

TEST(FindAllCollisionsTest, LinePolygonCircleOverlaps) {
    std::vector<Shape> shapes = {
        Line{{0.0, 0.0}, {6.0, 6.0}},
        Circle{{2.0, 2.0}, 2.0},
        Polygon{{{1.0, 1.0}, {5.0, 1.0}, {5.0, 5.0}, {1.0, 5.0}}},
    };

    auto result = utils::FindAllCollisions(shapes);

    EXPECT_EQ(result.size(), 3u);
}
