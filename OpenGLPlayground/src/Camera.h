#pragma once
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/constants.hpp>

using namespace glm;

class Camera
{
public:
    vec4			eye;
    vec4			lookat;

	mat4			mvMat;
    mat4			projMat;

private:
    float           fovy;              // view angle 
    float           aspect;            // the window ration equal to w/h
    float           near_plane;
    float           far_plane;

    vec4			axis_n;            //eye-lookat
    vec4		    axis_u;
    vec4	        axis_v;            //up axis
    vec4			world_up;

    vec2			mouse_pos;         // the cursor position of moving mouse
    vec2			mouse_pre_pos;
    unsigned short  mouse_button;

    vec2			key_pos;           // the translated step changed by keyboard operation in FP cam mode
    vec2			key_pre_pos;
    bool            m_altKey;          // detect if ALT pressed 

    
    int m_mode;
	
    // View Frustum
	vec4	     ntl;	          // the near-top-left
	vec4         ntr;	          // the near-top-right
	vec4         nbl;	          // the near-bottom-left
	vec4         nbr;	          // the near-bottom-right
	vec4	     ftl;	          // the far-top-left
	vec4	     fbr;	          // the far-bottom-right
	vec4         ftr;	          // the far-to-right
	vec4	     fbl;	          // the far-bottom-left

    // scale parameters for the camera control
	float M_ZOOM_PAR;
	float M_PAN_PAR;
	float M_PAN_PAR_FP;
	float M_ROTATE_PAR_FP;

public: 
	Camera();
	~Camera();
	void PrintProperty();
	void set(float eye_x, float eye_y, float eye_z, 
			 float lookat_x, float lookat_y, float lookat_z, 
             int winW, int winH,
			 float p_angle = 45.0f, float p_near =0.1f, float p_far=10000.0f);

    void setProj(int winW, int winH);
    void setModelView();
    void mouseClick(int button, int state, int x, int y, int winW ,int winH);
    void mouseMotion(int x, int y, int winW, int winH);
    void keyOperation(const unsigned char keyStates[], int winW, int winH);

	// cam mode
	void switchCamMode();
	bool isFocusMode();
	bool isFPMode();

	// third person model 
	void CameraRotate();
	void CameraZoom();
	void CameraPan();

	// first person model
	void CameraPan_fp();
	void CameraRotate_fp(int winW, int winH);

	void CameraAutoFocus(float new_lookat_x, float new_lookat_y, float new_lookat_z);

    void drawGrid();
    void drawCoordinate();
    void drawCoordinateOnScreen(int winW, int winH);

	void DrawCam(float pScaleX, float pScaleY, float pScaleZ);
	void drawFrustum();

	// function to see if camera is changed
	bool IsChanged();

private: 
    void horizontalRotate();    // for focus cam mode
	void verticalRotate();      // for focus cam mode
    void GetCamCS();
	void GetViewFrustum();
};