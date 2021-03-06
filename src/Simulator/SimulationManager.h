#pragma once

#include "Simulator/SimWorld.h"
#include "Simulator/SimNode.h"
#include "Simulator/SimInstance.h"
#include "Simulator/SimDebugDrawer.h"
#include "Simulator/SimInfo.h"
#include "Artifact/EvaluationType.h"
#include "Artifact/SimpleEvaluators.h"
#include "Artifact/AestheticEvaluator.h"
#include "Artifact/EvaluationDispatcher.h"
#include "Graphics/PBRMaterial.h"
#include "Graphics/PhongMaterial.h"
#include "Utils/Scheduler.h"
#include "Utils/ImageSaverThread.h"
#include "Utils/ImageSaver.h"
#include "Utils/FixedQueue.h"
#include "Networking/BufferSender.h"
#include "Networking/NetworkManager.h"
#include "ofMain.h"
#include "ofxShadowMap.h"
#include "ofxOpenCv.h"

typedef std::function<void(uint32_t)> simRunCallback_t;

class SimulationManager
{
public:
    struct SimSettings
    {
        EvaluationType evalType = EvaluationType::Coverage;
        uint32_t canvasResolution = 128;
        uint32_t canvasConvResolution = 64;

        float canvasSize = 4.0f;
        float canvasViewSize = 1.0f;
        float canvasMargin = 4.0f;

        uint32_t maxParallelSims = 1;
        std::string genomeFile = "";
        std::string host = "localhost";
        int inPort = 1024;
        int outPort = 1025;
    };

    void init(SimSettings settings);
    void startSimulation();
    void stopSimulation();

    void update();
    void lateUpdate();

    void shadowPass();
    void draw();
    void dealloc();

    std::string getUniqueSimId();
    const std::string& getStatus();
    const SimSettings& getSettings();

    // Returns ticket that listener can use to check when sim is finished and genome fitness is updated
    int queueSimInstance(SimInfo info);
    void terminateSimInstances();

    void loadShaders();

    bool isInitialized();
    bool isSimulationActive();
    bool isSimulationInstanceActive();

    ofEasyCam* getCamera();
    ofTexture* getPrevArtifactTexture();

    SimCreature* getFocusCreature();
    SimCanvasNode* getFocusCanvas();
    glm::vec3 getFocusOrigin();
    std::string getFocusInfo();
    void shiftFocus();
    void setAutoCam(bool enable);

    glm::ivec2 getCanvasResolution();
    glm::ivec2 getCanvasConvResolution();
    uint32_t getTimeStepsPerUpdate();
    const std::vector<float> getCPGBuffer();

    EvaluationType getEvaluationType();

    bool loadGenomeFromDisk(std::string filename);
    void generateRandomGenome();
    const std::shared_ptr<DirectedGraph>& getSelectedGenome();

    bool bAutoLoadGenome = true;
    bool bDebugDraw = false;
    bool bShadows = true;
    bool bMouseLight = false;
    bool bViewLightSpaceDepth = false;
    bool bViewCanvasEvaluationMask = false;
    bool bFeasibilityChecks = false;
    bool bCanvasSensors = false;
    bool bAxisAlignedAttachments = false;
    bool bSaveArtifactsToDisk = false;
    bool bStoreLastArtifact = false;
    bool bMultiEval = false;

    uint32_t simulationSpeed = 1;

    glm::vec3 lightPosition;
    float lightIntensity = 2000.0f;

    int genomeGenMinNumNodes = 6;
    int genomeGenMinNumConns = 4;

private:
    void setLightUniforms(const std::shared_ptr<ofShader>& shader);
    void setStatus(std::string msg);

    int createSimInstance(SimInfo info);
    void updateSimInstance(SimInstance* instance, double timeStep);

    void performTrueSteps(btScalar timeStep);

    SimSettings _settings;
    EvaluationType _evaluationType;
    EvaluationDispatcher _evaluationDispatcher;

    std::vector<simRunCallback_t> _simulationInstanceCallbackQueue;
    std::vector<SimInstance*> _simulationInstances;
    std::mutex _cbQueueMutex;

    std::string _status = "";
    std::string _uniqueSimId = "_NA";
    std::string _simDir = NTRS_SIMS_DIR;

    std::shared_ptr<DirectedGraph> _selectedGenome;
    std::shared_ptr<SimCreature> _previewCreature;
    std::unique_ptr<SimCanvasNode> _previewCanvas;

    // preview world
    SimWorld* _previewWorld;

    ofEasyCam cam;
    ofxShadowMap _shadowMap;

    // time
    btClock _clock;
    btScalar _startTimeMillis = 0;      // clock time that simulation started (ms)
    btScalar _runTimeMillis = 0;        // clock time simulation has run (ms)
    btScalar _simulationSpeed = 0.0;    // speed of simulation relative to clock time
    btScalar _targetFrameTimeMillis;

    btScalar _timeMillis = 0;
    btScalar _prevTime = 0;
    btScalar _frameTimeMillis = 0;
    btScalar _frameTimeAccumulator = 0;

    // graphics
    std::shared_ptr<ofShader> _terrainShader;
    std::shared_ptr<ofShader> _nodeShader;
    std::shared_ptr<ofShader> _canvasShader;
    std::shared_ptr<ofShader> _canvasSubTextureShader;
    std::shared_ptr<ofShader> _canvasColorShader;
    std::shared_ptr<ofShader> _canvasUpdateShader;

    std::shared_ptr<MaterialBase> _terrainMaterial;
    std::shared_ptr<MaterialBase> _nodeMaterial;
    std::shared_ptr<MaterialBase> _canvasMaterial;

    std::shared_ptr<ofLight> _light;
    float _lightDistanceFromFocus = 32.0f;

    bool bInitialized = false;
    bool bHasSimulationId = false;
    bool bSimulationActive = false;
    bool bStopSimulationQueued = false;
    bool bGenomeLoaded = false;
    bool bAutoCam = false;

    int _simInstanceIdCounter = 0;
    int _simInstanceGridSize = 2;
    uint32_t _simInstanceLimit = 256;
    uint32_t _focusIndex = 0;
    uint32_t _timeStepsPerUpdate = 0;
    uint32_t _maxGenGenomeAttempts = 5000;

    // canvas
    glm::ivec2 _canvasResolution;
    glm::ivec2 _canvasConvResolution;

    cv::Mat _artifactMat;
    ofxCvGrayscaleImage _cvDebugImage;

    ofTexture _prevArtifactTexture;
    ofBufferObject _artifactCopyBuffer;

    // io
    NetworkManager _networkManager;

    ofEventListener _connectionEstablishedListener;
    ofEventListener _connectionClosedListener;
    ofEventListener _infoReceivedListener;
    ofEventListener _pulseReceivedListener;
    ofEventListener _fitnessRequestReceivedListener;
    ofEventListener _fitnessResponseReadyListener;

    ImageSaver _imageSaver;
    FixedQueue _cpgQueue;
};
