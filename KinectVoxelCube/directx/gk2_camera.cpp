#include "gk2_camera.h"

using namespace gk2;
using namespace DirectX;

Camera::Camera(float minDistance, float maxDistance, float distance)
	: m_angleX(0.0f), m_angleY(0.0f), m_distance(distance)
{
	SetRange(minDistance, maxDistance);
}

void Camera::ClampDistance()
{
	if (m_distance < m_minDistance)
		m_distance = m_minDistance;
	if (m_distance > m_maxDistance)
		m_distance = m_maxDistance;
}

void Camera::SetRange(float minDistance, float maxDistance)
{
	if (minDistance < 0)
		minDistance = 0;
	if (maxDistance < minDistance)
		maxDistance = minDistance;
	m_minDistance = minDistance;
	m_maxDistance = maxDistance;
	ClampDistance();
}

void Camera::Zoom(float d)
{
	m_distance += d;
	ClampDistance();
}

void Camera::Rotate(float dx, float dy)
{
	m_angleX = XMScalarModAngle(m_angleX + dx);
	m_angleY = XMScalarModAngle(m_angleY + dy);
}

XMMATRIX Camera::GetViewMatrix() const
{
	XMMATRIX viewMtx;
	GetViewMatrix(viewMtx);
	return viewMtx;
}

void Camera::GetViewMatrix(XMMATRIX& viewMtx) const
{
	viewMtx = XMMatrixRotationY(-m_angleY) * XMMatrixRotationX(m_angleX)  * XMMatrixTranslation(0.0f, 0.0f, m_distance);
}

XMFLOAT4 Camera::GetPosition() const
{
	XMMATRIX viewMtx;
	GetViewMatrix(viewMtx);
	XMVECTOR det;
	viewMtx = XMMatrixInverse(&det, viewMtx);
	//auto alt = XMMatrixTranslation(0.0f, 0.0f, -m_distance) * XMMatrixRotationY(m_angleY) * XMMatrixRotationX(-m_angleX);
	XMFLOAT3 res(0.0f, 0.0f, 0.0f);
	auto transl = XMVector3TransformCoord(XMLoadFloat3(&res), viewMtx);
	XMStoreFloat3(&res, transl);
	return XMFLOAT4(res.x, res.y, res.z, 1.0f);
}
