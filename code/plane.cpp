#include <QVector>
#include <QPair>
#include "plane.h"
#include <fstream>
#include<iostream>
#include <cmath>

plane::plane(type t, QWidget *parent) : QWidget(parent)
{
    std::ifstream fin("C:/Users/Root/lab5_code/input.txt");
    int n;
    fin >> n;
    int x1, y1, x2, y2;
    QPointF point1, point2;
    for (int i = 0; i < n; i++)
    {
        fin >> x1 >> y1 >> x2 >> y2;
        point1.setX(x1);
        point1.setY(y1);
        point2.setX(x2);
        point2.setY(y2);
        segments.push_back(qMakePair(point1, point2));
    }
    fin >> Xmin >> Ymin >> Xmax >> Ymax;
    fin >> n;
    for (int i = 0; i < n; i++)
    {
        fin >> x1 >> y1 >> x2 >> y2;
        point1.setX(x1);
        point1.setY(y1);
        point2.setX(x2);
        point2.setY(y2);
        polygon.push_back(qMakePair(point1, point2));
    }
    this->t = t;
    if (t == type::Central)
    {
        clipSegments();
    }
    else
    {
        Cirus();
    }
}

void plane::clipSegments()
{
    QVector<QPair<QPointF, QPointF>> localSegments = this->segments;
    for (int i = 0; i < localSegments.size(); i++)
    {
        std::cout << i << std::endl;
        QPointF point1 = localSegments[i].first;
        QPointF point2 = localSegments[i].second;
        int codePoint1 = getCode(point1), codePoint2 = getCode(point2);

        // Проверка на короткий отрезок
        if (sqrt(pow(point1.x() - point2.x(), 2) + pow(point1.y() - point2.y(), 2)) <= 1)
        {
            return; // Короткий отрезок не требует отсечения
        }

        // Проверка видимости отрезка
        if ((codePoint1 | codePoint2) == 0)
        {
            // Отрезок полностью видим внутри окна
            clippedSegments.push_back(localSegments[i]);
        }
        else if ((codePoint1 & codePoint2) != 0)
        {
            // Отрезок полностью находится вне окна
            continue;
        }
        else
        {
            // Отрезок пересекает границу окна, производится отсечение Лианга-Барски
            if (codePoint1 == 0)
            {
                QPointF tmp = point1;
                point1 = point2;
                point2 = tmp;
            }
            localSegments[i] = qMakePair(point2, intersectionPoint(point1, point2));
            i--;
        }
    }
}



int plane::getCode(QPointF point)
{
    double x = point.x();
    double y = point.y();

    int code = 0;

    if (x < Xmin)
        code += 1;
    if (x > Xmax)
        code += 2;
    if (y < Ymin)
        code += 4;
    if (y > Ymax)
        code += 8;
    return code;
}

QPointF plane::intersectionPoint(QPointF P1, QPointF P2)
{
    int codeP1 = getCode(P1);

    QPointF intersectionPoint;
    if ((codeP1 & 8))
        intersectionPoint = QPointF(P1.x() + (P2.x() - P1.x()) * (Ymax - P1.y()) / (P2.y() - P1.y()), Ymax);
    else if ((codeP1 & 4))
        intersectionPoint = QPointF(P1.x() + (P2.x() - P1.x()) * (Ymin - P1.y()) / (P2.y() - P1.y()), Ymin);
    else if ((codeP1 & 2))
        intersectionPoint = QPointF(Xmax, P1.y() + (P2.y() - P1.y()) * (Xmax - P1.x()) / (P2.x() - P1.x()));
    else if ((codeP1 & 1))
        intersectionPoint = QPointF(Xmin, P1.y() + (P2.y() - P1.y()) * (Xmin - P1.x()) / (P2.x() - P1.x()));
    return intersectionPoint;
}

void plane::Cirus()
{
    for (int i = 0; i < segments.size(); i++)
    {
        ClipByCirus(segments[i]);
        if (t_1 < 0 || t_1 > 1 || t_2 < 0 || t_2 > 1)
        {
            continue;
        }
        else
        {
            QPointF begin = segments[i].first;
            QPointF end = segments[i].second;
            QPointF p1;
            p1.setX(begin.x() + t_1 * (end.x() - begin.x()));
            p1.setY(begin.y() + t_1 * (end.y() - begin.y()));
            QPointF p2;
            p2.setX(begin.x() + t_2 * (end.x() - begin.x()));
            p2.setY(begin.y() + t_2 * (end.y() - begin.y()));
            clippedSegments.push_back(qMakePair(p1, p2));
        }
    }
}

float plane::getT(QPair<QPointF, QPointF> edge, QPair<QPointF, QPointF> segment, bool *onLine)
{
    bool a;
    float ks = (segment.second.y() - segment.first.y()) / (segment.second.x() - segment.first.x());
    float ke = (edge.second.y() - edge.first.y()) / (edge.second.x() - edge.first.x());
    float bs = segment.first.y() - ks * segment.first.x();
    float be = edge.first.y() - ke * edge.first.x();
    float x = (be - bs) / (ks - ke);
    if ((x - edge.first.x()) / (edge.second.x() - edge.first.x()) <= 0 || (x - edge.first.x()) / (edge.second.x() - edge.first.x()) >= 1)
    {
        return -1;
    }
    if ((segment.second.x() - segment.first.x()) == 0 && ke == ks && be == bs)
    {
        *onLine = true;
        return -1;
    }
    else
    {
        float te = (x - segment.first.x()) / (segment.second.x() - segment.first.x());
        return te;
    }
}


float ScalarMultiply(QPair<QPointF,QPointF> v1, QPair<QPointF,QPointF> v2){
    float v1x1 = v1.first.y();
    float v1x2 = v1.second.y();
    float v1y1 = v1.first.x();
    float v1y2 = v1.second.x();
    float v2x1 = v2.first.x();
    float v2x2 = v2.second.x();
    float v2y1 = v2.first.y();
    float v2y2 = v2.second.y();
    return(-(v1x2-v1x1)*(v2x2-v2x1) + (v1y2-v1y1)*(v2y2-v2y1));
}

float getParameterOfPoint(QPointF p, QPair<QPointF,QPointF> segment)
{
    return (p.x() - segment.first.x()) / (segment.second.x() - segment.first.x());
}

void plane::ClipByCirus(QPair<QPointF, QPointF> segment){
    QVector<float> T_enter;
    QVector<float> T_outer;
    float t, S;
    bool onLine = false;
    // Проходим по всем граням многоугольника для отсечения Цируса-Бека
    for(int i = 0; i<polygon.size();i++){
        t = getT(polygon[i], segment, &onLine);
         // Если отрезок лежит на грани многоугольника
        if(onLine){
            T_enter.push_back(getParameterOfPoint(polygon[i].first, segment));
            T_outer.push_back(getParameterOfPoint(polygon[i].second, segment));
            T_outer.push_back(getParameterOfPoint(polygon[i].first, segment));
            T_enter.push_back(getParameterOfPoint(polygon[i].second, segment));
            onLine = false;
            continue;
        }
        S=-ScalarMultiply(polygon[i], segment);
        // Выполняем отсечение отрезка многоугольником по алгоритму Цируса-Бека
        if(t>=0 && t<=1){
            if(S>0){
                T_enter.push_back(t);
            }else if(S<0){
                T_outer.push_back(t);
            }else{
                T_enter.push_back(t);
                T_outer.push_back(t);
            }
        }
    }
    // Если отрезок полностью видим или полностью невидим
    if(T_outer.size() == 0 && T_enter.size() == 0){
        t_1 = -1;
        t_2 = -1;
        return;
    }

    float t_enter = 0;
    float t_outer = 1;
    for (int i = 0; i < T_enter.size(); i++){
        if (t_enter < T_enter[i])
        {
            t_enter = T_enter[i];
        }
    }
    for (int i = 0; i < T_outer.size(); i++){
        if (t_outer > T_outer[i])
        {
            t_outer = T_outer[i];
        }
    }
    t_1 = t_enter;
    t_2 = t_outer;
}

