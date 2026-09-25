#pragma once
#include "geometry.hpp"
#include <cmath>
#include <optional>

namespace geometry::intersections {

/*
 * Класс для поиска пересечений между двумя фигурами
 *
 * Требуется организовать возможность нахождения пересечений только для следующих комбинаций фигур:
 *    - Line   & Line
 *    - Line   & Circle
 *    - Circle & Circle
 *
 * Для всех остальных требуется выбросить исключение std::logic_error
 */
class IntersectionVisitor {
public:
    using IntersectionResult = std::optional<Point2D>;

    static constexpr double eps = 1e-12;

    [[nodiscard]] IntersectionResult operator()(const Line &l1, const Line &l2) const {
        const Point2D d1 = l1.end - l1.start;
        const Point2D d2 = l2.end - l2.start;
        const Point2D diff = l2.start - l1.start;

        const double denom = d1.Cross(d2);

        // Parallel lines (including collinear)
        if (std::abs(denom) < eps) {
            return std::nullopt;
        }

        const double t = diff.Cross(d2) / denom;
        const double s = diff.Cross(d1) / denom;

        // Check if intersection is within both segments
        if (t < 0.0 || t > 1.0 || s < 0.0 || s > 1.0) {
            return std::nullopt;
        }

        return l1.start + d1 * t;
    }

    [[nodiscard]] IntersectionResult operator()(const Line &line, const Circle &circle) const {
        const Point2D d = line.end - line.start;
        const Point2D f = line.start - circle.center_p;

        const double a = d.Dot(d);
        const double b = 2.0 * f.Dot(d);
        const double c = f.Dot(f) - circle.radius * circle.radius;

        const double discriminant = b * b - 4.0 * a * c;

        if (discriminant < 0.0) {
            return std::nullopt;
        }

        const double sqrt_disc = std::sqrt(discriminant);
        const double t1 = (-b - sqrt_disc) / (2.0 * a);
        const double t2 = (-b + sqrt_disc) / (2.0 * a);

        // Prefer the smallest t in [0, 1]
        std::optional<double> best_t;
        for (double t : {t1, t2}) {
            if (t >= 0.0 && t <= 1.0) {
                if (!best_t.has_value() || t < *best_t) {
                    best_t = t;
                }
            }
        }

        if (!best_t.has_value()) {
            return std::nullopt;
        }

        return line.start + d * (*best_t);
    }

    [[nodiscard]] IntersectionResult operator()(const Circle &circle, const Line &line) const {
        return (*this)(line, circle);
    }

    [[nodiscard]] IntersectionResult operator()(const Circle &c1, const Circle &c2) const {
        const Point2D diff = c2.center_p - c1.center_p;
        const double d = diff.Length();

        // No intersection: separate, one inside the other, or coincident
        if (d < eps) {
            return std::nullopt;
        }
        if (d > c1.radius + c2.radius + eps) {
            return std::nullopt;
        }
        if (d < std::abs(c1.radius - c2.radius) - eps) {
            return std::nullopt;
        }

        // Distance from c1 center to the radical line
        const double a = (c1.radius * c1.radius - c2.radius * c2.radius + d * d) / (2.0 * d);

        // Height from the radical line to the intersection points
        const double h_sq = c1.radius * c1.radius - a * a;
        const double h = h_sq > 0.0 ? std::sqrt(h_sq) : 0.0;

        const Point2D unit = diff / d;
        const Point2D base = c1.center_p + unit * a;

        if (h < eps) {
            // Tangent circles - single intersection point
            return base;
        }

        // Choose the first intersection point (consistent ordering)
        const Point2D perp{-unit.y, unit.x};
        return base + perp * h;
    }

    // Fallback: unsupported combination
    template <typename T1, typename T2>
    [[nodiscard]] IntersectionResult operator()(const T1 &, const T2 &) const {
        throw std::logic_error("Intersection is not supported for this combination of shape types");
    }
};

inline std::optional<Point2D> GetIntersectPoint(const Shape &shape1, const Shape &shape2) {
    return std::visit(IntersectionVisitor{}, shape1, shape2);
}

}  // namespace geometry::intersections