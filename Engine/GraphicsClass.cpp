////////////////////////////////////////////////////////////////////////////////
// Filename: graphicsclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "graphicsclass.h"


GraphicsClass::GraphicsClass()
{
	m_D3D = 0;
	m_Camera = 0;
	m_LightShader = 0;
	m_Light = 0;
}


GraphicsClass::GraphicsClass(const GraphicsClass& other)
{
}


GraphicsClass::~GraphicsClass()
{
}


bool GraphicsClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	bool result;


	// Create the Direct3D object.
	m_D3D = new D3DClass;
	if (!m_D3D)
	{
		return false;
	}

	// Initialize the Direct3D object.
	result = m_D3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize Direct3D.", L"Error", MB_OK);
		return false;
	}

	// Create the camera object.
	m_Camera = new CameraClass;
	if (!m_Camera)
	{
		return false;
	}

	// Set the initial position of the camera.
	m_Camera->SetPosition(0.0f, 0.0f, -20.0f);

	char* fileNames[] = {
		"../Engine/data/sword.obj",
		"../Engine/data/doll.obj",
		"../Engine/data/M1911.obj"
	};

	WCHAR* textures[] = {
		L"../Engine/data/t_sword.dds",
		L"../Engine/data/t_doll.dds",
		L"../Engine/data/t_M1911.dds"
	};

	// Create the model object.
	for (int i = 0; i < 3; ++i)
	{
		ModelClass* newModel = new ModelClass;
		result = newModel->Initialize(m_D3D->GetDevice(), fileNames[i], textures[i]);
		if (!result)
		{
			MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
			return false;
		}

		m_Models.push_back(newModel);
	}

	// Create the light shader object.
	m_LightShader = new LightShaderClass;
	if(!m_LightShader)
	{
		return false;
	}

	// Initialize the light shader object.
	result = m_LightShader->Initialize(m_D3D->GetDevice(), hwnd);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the light shader object.", L"Error", MB_OK);
		return false;
	}

	// Create the light object.
	m_Light = new LightClass;
	if(!m_Light)
	{
		return false;
	}

	// Initialize the light object.
	m_Light->SetAmbientColor(0.15f, 0.15f, 0.15f, 1.0f);
	m_Light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetDirection(0.0f, 0.0f, 1.0f);
	m_Light->SetSpecularColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetSpecularPower(32.0f);
	return true;
}


void GraphicsClass::Shutdown()
{
	// Release the light object.
	if(m_Light)
	{
		delete m_Light;
		m_Light = 0;
	}

	// Release the light shader object.
	if(m_LightShader)
	{
		m_LightShader->Shutdown();
		delete m_LightShader;
		m_LightShader = 0;
	}

	// Release the model object.
	for (auto it = m_Models.begin(); it != m_Models.end(); )
	{
		if (*it)
		{
			(*it)->Shutdown();
			delete (*it);
			it = m_Models.erase(it);
		}
		else ++it;
	}

	// Release the camera object.
	if(m_Camera)
	{
		delete m_Camera;
		m_Camera = 0;
	}

	// Release the D3D object.
	if(m_D3D)
	{
		m_D3D->Shutdown();
		delete m_D3D;
		m_D3D = 0;
	}

	return;
}


bool GraphicsClass::Frame()
{
	bool result;
	static float rotation = 0.0f;

	// Update the rotation variable each frame.
	rotation += (float)D3DX_PI * 0.005f;
	if(rotation > 360.0f)
	{
		rotation -= 360.0f;
	}
	
	// Render the graphics scene.
	result = Render(rotation);
	if(!result)
	{
		return false;
	}

	return true;
}

bool GraphicsClass::Render(float rotation)
{
	D3DXMATRIX worldMatrix, viewMatrix, projectionMatrix;
	D3DXMATRIX objRotateMatrix[3];
	D3DXMATRIX rotationX, rotationY, rotationZ;
	bool result;

	// Clear the buffers to begin the scene.
	m_D3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// Generate the view matrix based on the camera's position.
	m_Camera->Render();

	// Get the world, view, and projection matrices from the camera and d3d objects.
	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetWorldMatrix(worldMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);

	for (int i = 0; i < 3; ++i) objRotateMatrix[i] = worldMatrix;
	D3DXMatrixTranslation(&objRotateMatrix[0], 0.0f, 0.0f, 0.0f);
	D3DXMatrixTranslation(&objRotateMatrix[1], -4.0f, -4.0f, 0.0f);
	D3DXMatrixTranslation(&objRotateMatrix[2], 4.0f, 0.0f, 0.0f);

	// Rotate the world matrix by the rotation value so that the triangle will spin.
	D3DXMatrixIdentity(&rotationX);
	D3DXMatrixIdentity(&rotationY);
	D3DXMatrixIdentity(&rotationZ);
	D3DXMatrixRotationY(&rotationX, rotation);
	D3DXMatrixRotationY(&rotationY, rotation);
	D3DXMatrixRotationY(&rotationZ, rotation);

	D3DXMatrixMultiply(&objRotateMatrix[0], &objRotateMatrix[0], &rotationX);
	D3DXMatrixMultiply(&objRotateMatrix[1], &objRotateMatrix[1], &rotationY);
	D3DXMatrixMultiply(&objRotateMatrix[2], &objRotateMatrix[2], &rotationZ);
	
	// Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing.
	for (int i= 0; i < m_Models.size(); ++i)
	{
		m_Models[i]->Render(m_D3D->GetDeviceContext());
		// Render the model using the light shader.
		result = m_LightShader->Render(m_D3D->GetDeviceContext(), m_Models[i]->GetIndexCount(),
			objRotateMatrix[i], viewMatrix, projectionMatrix,
			m_Models[i]->GetTexture(), m_Light->GetDirection(),
			m_Light->GetAmbientColor(), m_Light->GetDiffuseColor(), m_Camera->GetPosition(),
			m_Light->GetSpecularColor(), m_Light->GetSpecularPower());
		if (!result) return false;
	}

	// Present the rendered scene to the screen.
	m_D3D->EndScene();

	return true;
}