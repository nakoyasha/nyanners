#pragma once

#include "Instance.h"
#include "raylib.h"

namespace Nyanners {
namespace Instances {
    class CameraInstance : public Instance {
    private:
        bool isReversed = false;

    public:
        Camera rCamera = { 0 };
        Vector3 position;
        float fieldOfView;

        CameraInstance()
            : Instance("Camera")
        {
            this->m_className = "Camera";
            this->m_name = "Camera";
        }

        void setPosition(Vector3 newPos) {
            rCamera.position = newPos;
            position = newPos;
        }

        void setFOV(float FOV) {
            rCamera.fovy = FOV;
            fieldOfView = FOV;
        }

        void update()
        {
            UpdateCamera(&rCamera, CAMERA_FREE);
            position = rCamera.position;
            fieldOfView = rCamera.fovy;
        }
    };
}
}