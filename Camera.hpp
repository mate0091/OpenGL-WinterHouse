//
//  Camera.hpp
//
//  Created by CGIS on 28/10/2016.
//  Copyright © 2016 CGIS. All rights reserved.
//

#ifndef Camera_hpp
#define Camera_hpp

#include <stdio.h>
#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"

namespace gps {
    
    enum MOVE_DIRECTION {MOVE_FORWARD, MOVE_BACKWARD, MOVE_RIGHT, MOVE_LEFT};
    
    class Camera
    {
    public:
        Camera(glm::vec3 cameraPosition, float moveSensitivity, float rotationSensitivity);
        glm::mat4 getViewMatrix();
		void move(MOVE_DIRECTION direction, double deltaTime);
        void rotate(float dx, float dy);		
		glm::vec3 getPosition()
		{
			return this->cameraPosition;
		}

		glm::vec3 getRotation()
		{
			return this->cameraForward;
		}
    private:
        glm::vec3 cameraPosition;
		glm::vec3 cameraUp;
        glm::vec3 cameraForward;
        glm::vec3 cameraRight;
		float yaw;
		float pitch;
		float moveSensitivity;
		float rotSensitivity;
		void updateVectors();
		
    };
    
}

#endif /* Camera_hpp */
