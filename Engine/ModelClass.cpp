////////////////////////////////////////////////////////////////////////////////
// Filename: modelclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "modelclass.h"


ModelClass::ModelClass()
{
	m_vertexBuffer = 0;
	m_indexBuffer = 0;
	m_Texture = 0;
	m_model = 0;
}


ModelClass::ModelClass(const ModelClass& other)
{
}


ModelClass::~ModelClass()
{
}


bool ModelClass::Initialize(ID3D11Device* device, char* modelFilename, WCHAR* textureFilename)
{
	bool result;

	// Load in the model data,
	result = LoadModel(modelFilename);
	if(!result)
	{
		return false;
	}

	// Initialize the vertex and index buffers.
	result = InitializeBuffers(device);
	if(!result)
	{
		return false;
	}

	// Load the texture for this model.
	result = LoadTexture(device, textureFilename);
	if(!result)
	{
		return false;
	}

	return true;
}


void ModelClass::Shutdown()
{
	// Release the model texture.
	ReleaseTexture();

	// Shutdown the vertex and index buffers.
	ShutdownBuffers();

	// Release the model data.
	ReleaseModel();

	return;
}


void ModelClass::Render(ID3D11DeviceContext* deviceContext)
{
	// Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.
	RenderBuffers(deviceContext);

	return;
}


int ModelClass::GetIndexCount()
{
	return m_indexCount;
}

int ModelClass::GetPolygonsCount()
{
	return m_polygonsCount;
}

ID3D11ShaderResourceView* ModelClass::GetTexture()
{
	return m_Texture->GetTexture();
}


bool ModelClass::InitializeBuffers(ID3D11Device* device)
{
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
    D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;
	int i;


	// Create the vertex array.
	vertices = new VertexType[m_vertexCount];
	if(!vertices)
	{
		return false;
	}

	// Create the index array.
	indices = new unsigned long[m_indexCount];
	if(!indices)
	{
		return false;
	}

	// Load the vertex array and index array with data.
	for(i=0; i<m_vertexCount; i++)
	{
		vertices[i].position = D3DXVECTOR3(m_model[i].x, m_model[i].y, m_model[i].z);
		vertices[i].texture = D3DXVECTOR2(m_model[i].tu, m_model[i].tv);
		vertices[i].normal = D3DXVECTOR3(m_model[i].nx, m_model[i].ny, m_model[i].nz);

		indices[i] = i;
	}

	// Set up the description of the static vertex buffer.
    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = 0;
    vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
    vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
    result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if(FAILED(result))
	{
		return false;
	}

	// Set up the description of the static index buffer.
    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufferDesc.CPUAccessFlags = 0;
    indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
    indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if(FAILED(result))
	{
		return false;
	}

	// Release the arrays now that the vertex and index buffers have been created and loaded.
	delete [] vertices;
	vertices = 0;

	delete [] indices;
	indices = 0;

	return true;
}


void ModelClass::ShutdownBuffers()
{
	// Release the index buffer.
	if(m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = 0;
	}

	// Release the vertex buffer.
	if(m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}

	return;
}


void ModelClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride;
	unsigned int offset;


	// Set vertex buffer stride and offset.
	stride = sizeof(VertexType); 
	offset = 0;
    
	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

    // Set the index buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

    // Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}


bool ModelClass::LoadTexture(ID3D11Device* device, WCHAR* filename)
{
	bool result;


	// Create the texture object.
	m_Texture = new TextureClass;
	if(!m_Texture)
	{
		return false;
	}

	// Initialize the texture object.
	result = m_Texture->Initialize(device, filename);
	if(!result)
	{
		return false;
	}

	return true;
}


void ModelClass::ReleaseTexture()
{
	// Release the texture object.
	if(m_Texture)
	{
		m_Texture->Shutdown();
		delete m_Texture;
		m_Texture = 0;
	}

	return;
}

bool ModelClass::ReadFileCounts(char* fileName, int& vertexCount, int& textureCount, int& normalCount, int& faceCount)
{
	ifstream fin;
	char input;

	// Initialize the counts.
	vertexCount = 0;
	textureCount = 0;
	normalCount = 0;
	faceCount = 0;

	// Open the file.
	fin.open(fileName);
	// Check if it was successful in opening the file.
	if (fin.fail() == true)
	{
		return false;
	}
	// Read from the file and continue to read until the end of the file is reached.
	fin.get(input);
	while (!fin.eof())
	{
		// If the line starts with 'v' then count either the vertex, the texture coordinates, or the normal vector.
		if (input == 'v')
		{
			fin.get(input);
			if (input == ' ') { ++vertexCount; }
			if (input == 't') { ++textureCount; }
			if (input == 'n') { ++normalCount; }
		}
		// If the line starts with 'f' then increment the face count.
		if (input == 'f')
		{
			fin.get(input);
			if (input == ' ') { ++faceCount; }
		}
		// Otherwise read in the remainder of the line.
		while (input != '\n')
		{
			fin.get(input);
		}
		// Start reading the beginning of the next line.
		fin.get(input);
	}
	// Close the file.
	fin.close();
	return true;
}

bool ModelClass::LoadModel(char* fileName)
{
	ifstream fin;
	D3DXVECTOR3 *vertices, *texcoords, *normals;
	FaceType *faces;
	int vertexCount, textureCount, normalCount, faceCount;
	int vertexIndex, texcoordIndex, normalIndex, faceIndex, vIndex, tIndex, nIndex, mIndex;
	char input, input2;

	// Set the number of indices to be the same as the vertex count.
	bool result = ReadFileCounts(fileName, vertexCount, textureCount, normalCount, faceCount);
	if (!result)
	{
		return false;
	}
	m_indexCount = m_vertexCount = faceCount * 3;
	m_polygonsCount = faceCount;

	// Initialize the four data structures.
	vertices = new D3DXVECTOR3[vertexCount];
	if (!vertices)
	{
		return false;
	}
	texcoords = new D3DXVECTOR3[textureCount];
	if (!texcoords)
	{
		return false;
	}
	normals = new D3DXVECTOR3[normalCount];
	if (!normals)
	{
		return false;
	}
	faces = new FaceType[faceCount];
	if (!faces)
	{
		return false;
	}
	m_model = new ModelType[m_vertexCount];
	if (!m_model)
	{
		return false;
	}

	// Initialize the indexes.
	vertexIndex = 0;
	texcoordIndex = 0;
	normalIndex = 0;
	faceIndex = 0;
	// Open the file.
	fin.open(fileName);
	// Check if it was successful in opening the file.
	if (fin.fail() == true)
	{
		return false;
	}
	// Read in the vertices, texture coordinates, and normals into the data structures.
	// Important: Also convert to left hand coordinate system since Maya uses right hand coordinate system.
	fin.get(input);
	while (!fin.eof())
	{
		if (input == 'v')
		{
			fin.get(input);
			// Read in the vertices.
			if (input == ' ')
			{
				fin >> vertices[vertexIndex].x >> vertices[vertexIndex].y >>
					vertices[vertexIndex].z;
				// Invert the Z vertex to change to left hand system.
				vertices[vertexIndex].z = vertices[vertexIndex].z * -1.0f;
				++vertexIndex;
			}
			// Read in the texture uv coordinates.
			if (input == 't')
			{
				fin >> texcoords[texcoordIndex].x >> texcoords[texcoordIndex].y;
				// Invert the V texture coordinates to left hand system.
				texcoords[texcoordIndex].y = 1.0f - texcoords[texcoordIndex].y;
				++texcoordIndex;
			}
			// Read in the normals.
			if (input == 'n')
			{
				fin >> normals[normalIndex].x >> normals[normalIndex].y >>
					normals[normalIndex].z;
				// Invert the Z normal to change to left hand system.
				normals[normalIndex].z = normals[normalIndex].z * -1.0f;
				++normalIndex;
			}
		}
		// Read in the faces.
		if (input == 'f')
		{
			fin.get(input);
			if (input == ' ')
			{
				// Read the face data in backwards to convert it to a left hand system from right hand system.
				fin >> faces[faceIndex].vIndex3 >> input2 >> faces[faceIndex].tIndex3 >>
					input2 >> faces[faceIndex].nIndex3 >> faces[faceIndex].vIndex2 >> input2
					>> faces[faceIndex].tIndex2 >> input2 >> faces[faceIndex].nIndex2 >>
					faces[faceIndex].vIndex1 >> input2 >> faces[faceIndex].tIndex1 >> input2 >>
					faces[faceIndex].nIndex1;
				++faceIndex;
			}
		}
		// Read in the remainder of the line.
		while (input != '\n')
		{
			fin.get(input);
		}
		// Start reading the beginning of the next line.
		fin.get(input);
	}
	// Close the file.
	fin.close();
	
	// Now loop through all the faces and output the three vertices for each face.
	for (int i = 0; i < faceIndex; ++i)
	{
		mIndex = i * 3;

		vIndex = faces[i].vIndex1 - 1;
		tIndex = faces[i].tIndex1 - 1;
		nIndex = faces[i].nIndex1 - 1;
		m_model[mIndex].x = vertices[vIndex].x;
		m_model[mIndex].y = vertices[vIndex].y;
		m_model[mIndex].z = vertices[vIndex].z;
		m_model[mIndex].tu = texcoords[tIndex].x;
		m_model[mIndex].tv = texcoords[tIndex].y;
		m_model[mIndex].nx = normals[nIndex].x;
		m_model[mIndex].ny = normals[nIndex].y;
		m_model[mIndex].nz = normals[nIndex].z;

		vIndex = faces[i].vIndex2 - 1;
		tIndex = faces[i].tIndex2 - 1;
		nIndex = faces[i].nIndex2 - 1;
		m_model[mIndex + 1].x = vertices[vIndex].x;
		m_model[mIndex + 1].y = vertices[vIndex].y;
		m_model[mIndex + 1].z = vertices[vIndex].z;
		m_model[mIndex + 1].tu = texcoords[tIndex].x;
		m_model[mIndex + 1].tv = texcoords[tIndex].y;
		m_model[mIndex + 1].nx = normals[nIndex].x;
		m_model[mIndex + 1].ny = normals[nIndex].y;
		m_model[mIndex + 1].nz = normals[nIndex].z;

		vIndex = faces[i].vIndex3 - 1;
		tIndex = faces[i].tIndex3 - 1;
		nIndex = faces[i].nIndex3 - 1;
		m_model[mIndex + 2].x = vertices[vIndex].x;
		m_model[mIndex + 2].y = vertices[vIndex].y;
		m_model[mIndex + 2].z = vertices[vIndex].z;
		m_model[mIndex + 2].tu = texcoords[tIndex].x;
		m_model[mIndex + 2].tv = texcoords[tIndex].y;
		m_model[mIndex + 2].nx = normals[nIndex].x;
		m_model[mIndex + 2].ny = normals[nIndex].y;
		m_model[mIndex + 2].nz = normals[nIndex].z;
	}
	
	// Release the four data structures.
	if (vertices)
	{
		delete[] vertices;
		vertices = 0;
	}
	if (texcoords)
	{
		delete[] texcoords;
		texcoords = 0;
	}
	if (normals)
	{
		delete[] normals;
		normals = 0;
	}
	if (faces)
	{
		delete[] faces;
		faces = 0;
	}

	return true;
}

void ModelClass::ReleaseModel()
{
	if(m_model)
	{
		delete [] m_model;
		m_model = 0;
	}

	return;
}