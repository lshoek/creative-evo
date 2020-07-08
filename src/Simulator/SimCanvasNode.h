#pragma once
#include "SimNode.h"
#include "ofBufferObject.h"
#include "ofFbo.h"
#include "ofPixels.h"

class SimCanvasNode : public SimNodeBase
{
public:
	SimCanvasNode(btVector3 position, float size, float extraBounds, int xRes, int yRes, int xNeuralInput, int yNeuralInput, bool bLocalVisionMode, bool bDownSample, btDynamicsWorld* ownerWorld);
	~SimCanvasNode();

	void update();
	void updateConvPixelBuffer();
	void clearConvPixelBuffer();

	virtual void draw() override;
	virtual void drawImmediate() override;

	virtual void addToWorld() override;
	virtual void removeFromWorld() override;

	void addBrushStroke(btVector3 location, float pressure);

	glm::ivec2 getCanvasResolution();

	ofFbo* getCanvasRawFbo();
	ofFbo* getCanvasFbo();

	ofFbo* getConvFbo();
	const ofPixels& getConvPixelBuffer();

	void setCanvasUpdateShader(std::shared_ptr<ofShader> shader);
	void setCanvasColorizeShader(std::shared_ptr<ofShader> shader);
	void setSubTextureShader(std::shared_ptr<ofShader> shader);
	void enableBounds();

private:
	struct BrushCoord {
		glm::vec2 coord;
		float pressure;
		float active;

		static int size() {
			return sizeof(glm::vec2) + 2*sizeof(float);
		}
	};
	//static void deleteSampler(GLuint* sampler) {
	//	glDeleteSamplers(1, sampler);
	//}
	//std::unique_ptr<GLuint, decltype(&deleteSampler)> samplerId{ new GLuint, &deleteSampler };

	void initPlane(btVector3 position, float size);
	void swapPbo();

	// canvas
	glm::ivec2 _canvasRes;
	glm::ivec2 _canvasConvRes;

	float _canvasSize;
	float _margin;

	ofMesh _drawQuad;
	ofMesh _drawQuadConv;

	// brush coords
	ofBufferObject _brushCoordBuffer;
	std::vector<BrushCoord> _brushCoordQueue;
	unsigned int _brushQueueSize = 0;

	BrushCoord _cachedBrushCoord;

	// render buffers
	ofFbo _convFbo;
	ofFbo _convLocalFbo;
	ofFbo _colorFbo;
	ofFbo _fbo[2];
	int iFbo = 0;

	bool _bLocalVisionMode = false;
	bool _bDownSample = true;

	ofPixels _convPixelBuffer;
	ofBufferObject _pixelWriteBuffers[2];
	ofBufferObject* _pboPtr;
	uint32_t iPbo;

	// colors
	ofColor _brushColor;

	// bounds
	std::unique_ptr<SimNode> _bounds[4];
	bool _bBounds = false;

	// special shaders
	std::shared_ptr<ofShader> _updateShader;
	std::shared_ptr<ofShader> _colorizeShader;
	std::shared_ptr<ofShader> _subTextureShader;
};