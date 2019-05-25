////////////////////////////////////////////////////////////////////////////////
// Filename: textclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _TEXTCLASS_H_
#define _TEXTCLASS_H_

#include <vector>

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "fontclass.h"
#include "fontshaderclass.h"


////////////////////////////////////////////////////////////////////////////////
// Class name: TextClass
////////////////////////////////////////////////////////////////////////////////
class TextClass
{
private:
	struct SentenceType
	{
		ID3D11Buffer *vertexBuffer, *indexBuffer;
		int vertexCount, indexCount, maxLength;
		int x, y;
		float red, green, blue;
	};

	struct VertexType
	{
		D3DXVECTOR3 position;
	    D3DXVECTOR2 texture;
	};

	struct SentenceData {
		string msg;
		int posX, posY;
		float r, g, b;
	};

public:
	TextClass();
	TextClass(const TextClass&);
	~TextClass();

	bool Initialize(ID3D11Device*, ID3D11DeviceContext*, HWND, int, int, D3DXMATRIX);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, D3DXMATRIX, D3DXMATRIX);

	bool SetMousePosition(int, int, ID3D11DeviceContext*);
	bool SetFPS(int, ID3D11DeviceContext*);
	bool SetCPU(int, ID3D11DeviceContext*);
	bool SetNumOfObjects(int, ID3D11DeviceContext*);
	bool SetNumOfPolygons(int, ID3D11DeviceContext*);
	bool SetScreenSize(int, int, ID3D11DeviceContext*);

	void TurnOnOffRenderInfo();
	
private:
	bool InitializeSentence(SentenceType**, int, ID3D11Device*);
	bool UpdateSentence(SentenceType*, const char*, int, int, float, float, float, ID3D11DeviceContext*);
	void ReleaseSentence(SentenceType**);
	bool RenderSentence(ID3D11DeviceContext*, SentenceType*, D3DXMATRIX, D3DXMATRIX);

private:
	D3DXMATRIX					m_baseViewMatrix;
	FontClass*					m_Font;
	FontShaderClass*			m_FontShader;
	std::vector<SentenceType*>  m_renderInfo;
	int							m_screenWidth, m_screenHeight;
	bool						isEnableRenderInfo;
};

#endif