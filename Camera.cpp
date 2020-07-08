//
//  Camera.cpp
//  Lab5
//
//  Created by CGIS on 28/10/2016.
//  Copyright © 2016 CGIS. All rights reserved.
//

#include "Camera.hpp"

namespace gps {
    
    Camera::Camera(glm::vec3 cameraPosition, float moveSensitivity, float rotationSensitivity)
    {
        this->cameraPosition = cameraPosition;
		this->yaw = -90.0f;
		this->pitch = 0.0f;
		this->moveSensitivity = moveSensitivity;
		this->rotSensitivity = rotationSensitivity;

		updateVectors();
    }
    
    glm::mat4 Camera::getViewMatrix()
    {
        return glm::lookAt(cameraPosition, cameraPosition + cameraForward , glm::vec3(0.0f, 1.0f, 0.0f));
    }
    
    void Camera::move(MOVE_DIRECTION direction, double deltaTime)
    {
        switch (direction) {
            case MOVE_FORWARD:
                cameraPosition += cameraForward * (float)(moveSensitivity * deltaTime);
                break;
                
            case MOVE_BACKWARD:
                cameraPosition -= cameraForward * (float)(moveSensitivity * deltaTime);
                break;
        }
    }
    
    void Camera::rotate(float dx, float dy)
    {
		yaw += dx * rotSensitivity;
		pitch += dy * rotSensitivity;

		if (pitch > 89.0f)
			pitch = 89.0f;
		if (pitch < -89.0f)
			pitch = -89.0f;

		updateVectors();
    }
    
	void Camera::updateVectors()
	{
		glm::vec3 front;
		front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		front.y = sin(glm::radians(pitch));
		front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		cameraForward = glm::normalize(front);
		// Also re-calculate the Right and Up vector
		cameraRight = glm::normalize(glm::cross(cameraForward, glm::vec3(0.0f, 1.0f, 0.0f)));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
		cameraUp = glm::normalize(glm::cross(cameraRight, cameraForward));
	}
}
