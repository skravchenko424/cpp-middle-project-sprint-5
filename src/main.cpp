#include "convex_hull.hpp"
#include "geometry.hpp"
#include "intersections.hpp"
#include "queries.hpp"
#include "shape_utils.hpp"
#include "triangulation.hpp"
#include "visualization.hpp"

#include <algorithm>
#include <print>
#include <ranges>

using namespace geometry;

namespace rng = std::ranges;
namespace views = std::ranges::views;

void PrintAllIntersections(const Shape &shape, std::span<const Shape> others) {
    std::println("\n=== Intersections ===");

    /*
     * Используйте ranges чтобы оставить только фигуры,
     * поддерживающие возможность находить пересечения между собой
     *
     * Затем примените монадический интерфейс для обработки результатов:
     *     - Пересечение найдено в точке A между фигурами B и C
     *     - Фигуры B и C не пересекаются
     */

    auto isSupported = queries::Multilambda{[](const Line &) { return true; }, [](const Circle &) { return true; },
                                            [](const auto &) { return false; }};

    if (std::visit(isSupported, shape)) {
        rng::for_each(
            others | views::filter([&isSupported](const auto &shape) { return std::visit(isSupported, shape); }),
            [&shape](const auto &other) {
                auto res = intersections::GetIntersectPoint(shape, other);
                if (res.has_value()) {
                    std::println("Пересечение найдено в точке {} между фигурами {} и {}", res.value(), shape, other);
                } else {
                    std::println("Фигуры {} и {} не пересекаются", shape, other);
                }
            });
    }
}

void PrintDistancesFromPointToShapes(Point2D p, std::span<const Shape> shapes) {
    std::println("\n=== Distance from Point Test ===");

    /*
     * Используйте ranges чтобы выбрать любые 5 фигур из списка.
     * Затем найдите расстояния от заданной точки до всех выбранных фигур.
     * Выведите результат в формате "Расстояние от точки P до фигуры S равно D"
     */

    rng::for_each(shapes | views::take(5), [&p](const auto &shape) {
        std::println("Расстояние от точки {} до фигуры {} равно {}", p, shape, queries::DistanceToPoint(shape, p));
    });
}

void PerformShapeAnalysis(std::span<const Shape> shapes) {
    std::println("\n=== Shape Analysis ===");

    /*
     * Используйте ranges и созданные классы чтобы:
     *     - Найти все пересечения между фигурами используя метод Bounding Box
     *     - Найти самую высокую фигуру (чья высота наибольшая)expected
     *     - Вывести расстояние между любыми двумя фигурами, которые поддерживают данную функциональность
     */

    for (const auto &[shape1, shape2] : utils::FindAllCollisions(shapes)) {
        std::println(" Фигуры {} и {} пересекаются", shape1, shape2);
    }

    if (auto index = utils::FindHighestShape(shapes)) {
        std::println("Самая высокая фигура (индекс {}): {}, высота = {:.2f}", *index, shapes[*index],
                     queries::GetHeight(shapes[*index]));
    }

    if (shapes.size() >= 2) {
        if (auto dist = queries::DistanceBetweenShapes(shapes[0], shapes[1])) {
            std::println("Расстояние между фигурами {} и {}: {:.2f}", shapes[0], shapes[1], *dist);
        } else {
            std::println("Расстояние между фигурами не поддерживается");
        }
    }
}

void PerformExtraShapeAnalysis(std::span<const Shape> shapes) {
    std::println("\n=== Shape Extra Analysis ===");

    /*
     * Используйте ranges и созданные классы чтобы:
     *     - Вывести 3 любые фигуры, которые находятся выше 50.0
     *     - Вывести фигуры с наименьшей и с наибольшей высотами
     */

    auto shapesAbove50 =
        shapes | views::filter([](const Shape &s) { return queries::GetHeight(s) > 50.0; }) | views::take(3);

    std::println("Фигуры, которые находятся выше 50.0:");
    for (const auto &shape : shapesAbove50) {
        std::println("{}, высота = {:.2f}", shape, queries::GetHeight(shape));
    }

    if (shapesAbove50.empty()) {
        std::println("Нет таких фигур");
    }

    if (!shapes.empty()) {
        auto min_it = rng::min_element(
            shapes, [](const Shape &a, const Shape &b) { return queries::GetHeight(a) < queries::GetHeight(b); });
        auto max_it = rng::max_element(
            shapes, [](const Shape &a, const Shape &b) { return queries::GetHeight(a) < queries::GetHeight(b); });

        std::println("Фигура наименьшей высоты: {}, высота {:.2f}", *min_it, queries::GetHeight(*min_it));
        std::println("Фигура наимольшей высоты: {}, высота {:.2f}", *max_it, queries::GetHeight(*max_it));
    }
}

int main() {
    std::vector<Shape> shapes = utils::ParseShapes("circle 0 0 1.5; line 1 2 3 4; polygon 0 0 2 5; triangle 0 0 1 0 "
                                                   "0.5 1; polygon 0 0 1 2; badshape; circle 0 0 -1");
    std::println("Parsed {} shapes", shapes.size());

    for (auto [i, shape] : shapes | views::enumerate) {
        std::println("  [{}] {} — высота {:.4f}", i, shape, queries::GetHeight(shape));
    }

    //
    // Вызываем разработанные функции
    //
    PrintAllIntersections(shapes[0], shapes);

    PrintDistancesFromPointToShapes(Point2D{10.0, 10.0}, shapes);

    PerformShapeAnalysis(shapes);

    PerformExtraShapeAnalysis(shapes);

    //
    // Рисуем все фигуры
    //
    // Важно: после изучения графика - нажмите Enter чтобы продолжить выполнение и построить 2ой график
    //
    geometry::visualization::Draw(shapes);

    //
    // Формируем список из вершин всех фигур
    //
    std::vector<Point2D> points;
    for (const auto &shape : shapes) {
        std::visit(
            [&points](const auto &s) {
                auto vertices = s.Vertices();
                if constexpr (std::is_same_v<decltype(vertices), std::vector<Point2D>>) {
                    points.insert(points.end(), vertices.begin(), vertices.end());
                } else if constexpr (std::is_same_v<decltype(vertices), std::span<const Point2D>>) {
                    points.insert(points.end(), vertices.begin(), vertices.end());
                } else {
                    points.insert(points.end(), vertices.begin(), vertices.end());
                }
            },
            shape);
    }

    //
    // Находим список точек, для построения выпуклой оболочки - convex hull - алгоритмом Грэхема
    // Создаём из них объект класса `Polygon` и добавляем его в список shapes
    // Рисуем все фигуры
    //

    auto convexHull = convex_hull::GrahamScan(points);
    shapes.push_back(Polygon{std::move(*convexHull)});
    geometry::visualization::Draw(shapes);

    //
    // после изучения графика - нажмите Enter чтобы продолжить выполнение и построить 3ий график
    //

    {
        std::vector<Point2D> points = {{0, 0}, {10, 0}, {5, 8}, {15, 5}, {2, 12}};

        //
        // Используйте список точек points или свой, чтобы
        // выполнить алгоритм триангуляции Делоне алгоритмом Боуэра-Ватсона
        //
        // После успешного завершения алгоритма - выведите результат для проверки
        // используя geometry::visualization::Draw
        //

        auto triangulation = triangulation::DelaunayTriangulation(points);
        std::vector<Shape> triangulation_shapes;
        for (const auto &point : *triangulation) {
            triangulation_shapes.push_back(Triangle{point.a, point.b, point.c});
        }
        geometry::visualization::Draw(triangulation_shapes);
    }
    return 0;
}