#include "componentefisico.h"

componentefisico::componentefisico(tranformacion *t)
    : m_transform(t),
    m_velocity(0, 0),
    m_direction(0),
    m_speed(0.0f)
{}

void componentefisico::setSpeed(float pxPerSecond)
{
    m_speed = pxPerSecond;
}
componentefisico::componentefisico(const componentefisico &o)
    : m_transform(o.m_transform)
    , m_velocity(o.m_velocity)
    , m_speed(o.m_speed)
    , m_direction(o.m_direction)
{}

QPointF componentefisico::velocity() const
{
    return m_velocity;
}

void componentefisico::setVelocity(float vx, float vy)
{
    m_velocity.setX(vx);
    m_velocity.setY(vy);
    m_speed = std::hypot(vx, vy);
    if (m_speed > 0.0f) {
        float angRad = std::atan2(vy, vx);
        m_direction = int(angRad * 180.0f / M_PI);
    }
}

void componentefisico::setDirection(int dir)
{
    m_direction = dir;
    float rad = m_direction * M_PI / 180.0f;
    m_velocity.setX(m_speed * std::cos(rad));
    m_velocity.setY(m_speed * std::sin(rad));
}

void componentefisico::actualizar(float dt)
{
    if (m_transform) {
        QPointF pos = m_transform->getPosition();
        pos += m_velocity * dt;
        m_transform->setPosition(pos.x(), pos.y());
    }
}

