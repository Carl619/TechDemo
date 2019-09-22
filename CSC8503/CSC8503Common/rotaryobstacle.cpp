#include "rotaryobstacle.h"

void RotaryObstacle::update(float dt)
{
	rotation += speed * dt;
	if (rotation >= 360)
		rotation = 0;
	if(Axis==RotationAxis::X)
		transform.SetLocalOrientation(Quaternion::AxisAngleToQuaterion(Vector3(1,0,0), rotation));
	else if (Axis == RotationAxis::Y)
		transform.SetLocalOrientation(Quaternion::AxisAngleToQuaterion(Vector3(0, 1, 0), rotation));

	else if (Axis == RotationAxis::Z)
		transform.SetLocalOrientation(Quaternion::AxisAngleToQuaterion(Vector3(0, 0, 1), rotation));


}
