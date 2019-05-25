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
	m_TextureShader = 0;
	m_Bitmap = 0;
	m_Text = 0;
}

GraphicsClass::GraphicsClass(const GraphicsClass& other)
{
}


GraphicsClass::~GraphicsClass()
{
}

bool GraphicsClass::Initialize(int screenWidth, int screenHeight, HWND hwnd, InputClass* input)
{
	bool result;
	D3DXMATRIX baseViewMatrix;

	m_Input = input;

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
		"../Engine/data/human.obj",
		"../Engine/data/N916MU.obj",
		"../Engine/data/M8 Ape Marauder.obj"
	};

	WCHAR* textures[] = {
		L"../Engine/data/human.dds",
		L"../Engine/data/N916MU.dds",
		L"../Engine/data/M8 Ape Marauder.dds"
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
	m_Light->SetSpecularPower(32.0f);	// Create the texture shader object.
	m_TextureShader = new TextureShaderClass;
	if (!m_TextureShader)
	{
		return false;
	}
	// Initialize the texture shader object.
	result = m_TextureShader->Initialize(m_D3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the texture shader object.", L"Error", MB_OK);
		return false;
	}	// Create the bitmap object.
	m_Bitmap = new BitmapClass;
	if (!m_Bitmap)
	{
		return false;
	}
	// Initialize the bitmap object.
	result = m_Bitmap->Initialize(m_D3D->GetDevice(), screenWidth, screenHeight,
		L"../Engine/data/background.dds", screenWidth, screenHeight);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the bitmap object.", L"Error", MB_OK);
		return false;
	}	// Initialize a base view matrix with the camera for 2D user interface rendering.
	m_Camera->Render();
	m_Camera->GetViewMatrix(baseViewMatrix);
	
	// Create the text object.
	m_Text = new TextClass;
	if (!m_Text)
	{
		return false;
	}
	// Initialize the text object.
	result = m_Text->Initialize(m_D3D->GetDevice(), m_D3D->GetDeviceContext(), hwnd, screenWidth,
		screenHeight, baseViewMatrix);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the text object.", L"Error", MB_OK);
		return false;
	}
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

	// Release the bitmap object.
	if (m_Bitmap)
	{
		m_Bitmap->Shutdown();
		delete m_Bitmap;
		m_Bitmap = 0;
	}
	// Release the texture shader object.
	if (m_TextureShader)
	{
		m_TextureShader->Shutdown();
		delete m_TextureShader;
		m_TextureShader = 0;
	}

	// Release the text object.
	if (m_Text)
	{
		m_Text->Shutdown();
		delete m_Text;
		m_Text = 0;
	}

	return;
}

bool GraphicsClass::Frame(int fps, float frameTime, int cpu, int screenWidth, int screenHeight)
{
	bool result;
	const float cameraSpeed = 20.0f;

	int deltaX, deltaY;
	m_Input->GetMouseDeltaPosition(deltaX, deltaY);

	m_Camera->Yaw(deltaX * frameTime * 0.0018f);
	m_Camera->Pitch(deltaY * frameTime * 0.0018f);

	if (m_Input->GetKey(KeyCode::W)) m_Camera->MoveForward(cameraSpeed * frameTime);
	if (m_Input->GetKey(KeyCode::A)) m_Camera->Strafe(-cameraSpeed * frameTime);
	if (m_Input->GetKey(KeyCode::S)) m_Camera->MoveForward(-cameraSpeed * frameTime);
	if (m_Input->GetKey(KeyCode::D)) m_Camera->Strafe(cameraSpeed * frameTime);

	if (m_Input->GetKeyDown(KeyCode::F1)) m_Text->TurnOnOffRenderInfo();

	// Set the frames per second.
	result = m_Text->SetFPS(fps, m_D3D->GetDeviceContext());
	if (!result)
	{
		return false;
	}
	// Set the cpu usage.
	result = m_Text->SetCPU(cpu, m_D3D->GetDeviceContext());
	if (!result)
	{
		return false;
	}

	result = m_Text->SetNumOfObjects(m_Models.size(), m_D3D->GetDeviceContext());
	if (!result)
	{
		return false;
	}

	int numOfPolygons = 0;
	for (auto model : m_Models) numOfPolygons += model->GetPolygonsCount();

	result = m_Text->SetNumOfPolygons(numOfPolygons, m_D3D->GetDeviceContext());
	if (!result)
	{
		return false;
	}

	result = m_Text->SetScreenSize(screenWidth, screenHeight, m_D3D->GetDeviceContext());
	if (!result)
	{
		return false;
	}

	// Render the graphics scene.
	result = Render();
	if(!result)
	{
		return false;
	}

	return true;
}

bool GraphicsClass::Render()
{
	bool result;
	const size_t NumOfObject = m_Models.size();
	D3DXMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoMatrix;
	vector<D3DXMATRIX> objectMatrices(m_Models.size());
	vector<D3DXVECTOR3> positions({
		{ 1.0f, 1.0f, 1.0f},
		{ 400.0f, 1.0f, 1.0f},
		{ 1000.0f, 1.0f, 0.0f},
	});
		
	// Clear the buffers to begin the scene.
	m_D3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// Generate the view matrix based on the camera's position.
	m_Camera->Render();

	// Get the world, view, and projection matrices from the camera and d3d objects.
	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetWorldMatrix(worldMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);
	m_D3D->GetOrthoMatrix(orthoMatrix);

	// Turn off the Z buffer to begin all 2D rendering.
	m_D3D->TurnZBufferOff();

	// Put the bitmap vertex and index buffers on the graphics pipeline to prepare them for drawing.
	//result = m_Bitmap->Render(m_D3D->GetDeviceContext(), 0, 0);
	//if (!result)
	//{
	//	return false;
	//}

	// Render the bitmap with the texture shader.
	result = m_TextureShader->Render(m_D3D->GetDeviceContext(), m_Bitmap->GetIndexCount(),
		worldMatrix, viewMatrix, orthoMatrix, m_Bitmap->GetTexture());
	if (!result)
	{
		return false;
	}

	// Turn on the alpha blending before rendering the text.
	m_D3D->TurnOnAlphaBlending();

	// Render the text strings.
	result = m_Text->Render(m_D3D->GetDeviceContext(), worldMatrix, orthoMatrix);
	if (!result)
	{
		return false;
	}

	// Turn off alpha blending after rendering the text.
	m_D3D->TurnOffAlphaBlending();

	// Turn the Z buffer back on now that all 2D rendering has completed.
	m_D3D->TurnZBufferOn();

	// Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing.
	for (int i= 0; i < m_Models.size(); ++i)
	{
		objectMatrices[i] = worldMatrix;
		D3DXMatrixTranslation(&objectMatrices[i],
			positions[i].x, positions[i].y, positions[i].z);

		m_Models[i]->Render(m_D3D->GetDeviceContext());
		// Render the model using the light shader.
		result = m_LightShader->Render(m_D3D->GetDeviceContext(), m_Models[i]->GetIndexCount(),
			objectMatrices[i], viewMatrix, projectionMatrix,
			m_Models[i]->GetTexture(), m_Light->GetDirection(),
			m_Light->GetAmbientColor(), m_Light->GetDiffuseColor(), m_Camera->GetPosition(),
			m_Light->GetSpecularColor(), m_Light->GetSpecularPower());
		if (!result) return false;
	}

	// Present the rendered scene to the screen.
	m_D3D->EndScene();

	return true;
}