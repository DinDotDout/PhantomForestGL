#ifndef _CAMERA_H_
#define _CAMERA_H_

class Camera
{
public:
	Camera() { Init(); }
	~Camera() {}

	void Init();
	void Refresh();
	void SetPos(float x, float y, float z);
	void GetPos(float &x, float &y, float &z);
	void GetDirectionVector(float &x, float &y, float &z);
	void SetYaw(float angle);
	void SetPitch(float angle);

	// Variables de navegacion
	void Move(float incr);
	void Strafe(float incr);
	void Fly(float incr);
	void RotateYaw(float angle);
	void RotatePitch(float angle);

private:
	float m_x, m_y, m_z;   // Posicion
	float m_lx, m_ly, m_lz; // Verctores de direccion
	float m_yaw, m_pitch; // Angulos de direccion
	float m_strafe_lx, m_strafe_lz; // Vectores de direccion a 90 grados
	float m_move_lx, m_move_lz;
};

#endif