#include <stdio.h>
#include "math.h"
#include "pch.h"
#include <cmath>
#include <GL/glut.h>
#include "Camera.h"

void Camera::Init()
{	//Inclinacion inicial
	m_yaw = 0.0f;
	m_pitch = 0.0f;
	//Posicion inicial
	SetPos(0, 3, 0);
}

void Camera::Refresh()
{
	//Actualización pos de la camara
	m_lx = cos(m_yaw) * cos(m_pitch);
	m_ly = sin(m_pitch);
	m_lz = sin(m_yaw) * cos(m_pitch);

	//Obtenemos los valores para mover la camara lateralmente
	m_strafe_lx = cos(m_yaw - (acos(-1.0f) / 2.0f));
	m_strafe_lz = sin(m_yaw - (acos(-1.0f) / 2.0f));

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(m_z, m_y, m_x, m_z + m_lz, m_y + m_ly, m_x + m_lx, 0.0, 1.0, 0.0);
}

void Camera::SetPos(float x, float y, float z)
{
	m_x = x;
	m_y = y;
	m_z = z;

	Refresh();
}

void Camera::GetPos(float &x, float &y, float &z)
{
	x = m_x;
	y = m_y;
	z = m_z;
}

void Camera::GetDirectionVector(float &x, float &y, float &z)
{
	x = m_lx;
	y = m_ly;
	z = m_lz;
}

//Desplazamiento frontal de la camara
void Camera::Move(float incr)
{
	float lx = cos(m_yaw);
	float lz = sin(m_yaw);

	m_x = m_x + incr * lx;
	m_z = m_z + incr * lz;

	Refresh();
}

//Desplazamiento lateral de la camara
void Camera::Strafe(float incr)
{
	m_x = m_x + incr * m_strafe_lx;
	m_z = m_z + incr * m_strafe_lz;

	Refresh();
}

//Desplazamiento vertical de la camara
void Camera::Fly(float incr)
{
	m_y = m_y + incr;

	Refresh();
}

//Rotacion horizontal de la camara
void Camera::RotateYaw(float angle)
{
	m_yaw += angle;

	Refresh();
}

//Rotación vertical de la camara, hay un límite en 70 grados
void Camera::RotatePitch(float angle)
{
	const float limit = 60.0f * (acos(-1.0f)) / 180.0f;

	m_pitch += angle;

	if (m_pitch < -limit)
		m_pitch = -limit;

	if (m_pitch > limit)
		m_pitch = limit;

	Refresh();
}

void Camera::SetYaw(float angle)
{
	m_yaw = angle;

	Refresh();
}

void Camera::SetPitch(float angle)
{
	m_pitch = angle;

	Refresh();
}