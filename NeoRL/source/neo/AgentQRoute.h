#pragma once

#include "ComparisonSparseCoder.h"
#include "Predictor.h"

namespace neo {
	class AgentQRoute {
	public:
		struct QConnection {
			float _weight;

			float _trace;

			QConnection()
				: _trace(0.0f)
			{}
		};
		struct LayerDesc {
			cl_int2 _size;

			cl_int _feedForwardRadius, _recurrentRadius, _lateralRadius, _feedBackRadius, _predictiveRadius, _qRadius;

			cl_int _scSolveIter;
			cl_float _scWeightAlpha;
			cl_float _scLateralWeightAlpha;
			cl_float _scThresholdAlpha;
			cl_float _scWeightTraceLambda;
			cl_float _scActiveRatio;
			cl_float _scBoostAlpha;

			cl_float _baseLineDecay;
			cl_float _baseLineSensitivity;

			cl_float _predWeightAlpha;

			cl_float _qAlpha;
			cl_float _qBiasAlpha;
			cl_float _qGammaLambda;
			cl_float _qEluAlpha;

			LayerDesc()
				: _size({ 8, 8 }),
				_feedForwardRadius(5), _recurrentRadius(5), _lateralRadius(5), _feedBackRadius(5), _predictiveRadius(5), _qRadius(6),
				_scSolveIter(10), _scWeightAlpha(0.01f), _scLateralWeightAlpha(0.1f), _scThresholdAlpha(0.005f),
				_scWeightTraceLambda(0.95f), _scActiveRatio(0.01f), _scBoostAlpha(0.1f),
				_baseLineDecay(0.01f), _baseLineSensitivity(4.0f),
				_predWeightAlpha(0.1f),
				_qAlpha(0.005f), _qBiasAlpha(0.005f), _qGammaLambda(0.95f), _qEluAlpha(1.0f)
			{}
		};

		struct Layer {
			ComparisonSparseCoder _sc;
			Predictor _pred;

			DoubleBuffer2D _baseLines;

			cl::Image2D _reward;

			cl::Image2D _scHiddenStatesPrev;

			// Q
			DoubleBuffer2D _qStates;
			DoubleBuffer3D _qWeights;
			DoubleBuffer2D _qBiases;
			cl::Image2D _qErrorTemp;
		};

		static float elu(float x, float alpha) {
			return x >= 0.0f ? x : alpha * (std::exp(x) - 1.0f);
		}

		static float elud(float x, float alpha) {
			return x >= 0.0f ? 1.0f : x + alpha;
		}

	private:
		std::vector<Layer> _layers;
		std::vector<LayerDesc> _layerDescs;

		std::vector<QConnection> _qConnections;
		std::vector<float> _qStates;
		std::vector<float> _scStates;
		std::vector<float> _qErrors;

		std::vector<float> _inputPredictions;
		std::vector<float> _actionPredictions;

		std::vector<float> _inputs;
		std::vector<float> _actions;
		std::vector<float> _actionsExploratory;
		std::vector<float> _actionErrors;

		cl_float _prevValue;

		Predictor _inputPred;
		Predictor _actionPred;

		cl::Kernel _baseLineUpdateKernel;

		cl::Kernel _qForwardKernel;
		cl::Kernel _qForwardFirstLayerKernel;
		cl::Kernel _qBackwardKernel;
		cl::Kernel _qBackwardFirstLayerKernel;
		cl::Kernel _qWeightUpdateKernel;
		cl::Kernel _qWeightUpdateFirstLayerKernel;

		cl::Image2D _inputsImage;
		cl::Image2D _actionsImage;
		cl::Image2D _actionsExploratoryImage;
		cl::Image2D _lastLayerError;
		cl::Image2D _actionErrorsImage;

	public:
		cl_float _predInputWeightAlpha;
		cl_float _predActionWeightAlpha;
		cl_int _qIter;
		cl_float _actionDeriveAlpha;
		cl_float _lastLayerQAlpha;
		cl_float _lastLayerQGammaLambda;

		cl_float _gamma;

		cl_float _explorationPerturbationStdDev;
		cl_float _explorationBreakChance;

		cl_float _actionMomentum;

		AgentQRoute()
			: _predInputWeightAlpha(0.05f),
			_predActionWeightAlpha(0.05f),
			_qIter(1),
			_actionDeriveAlpha(0.08f),
			_lastLayerQAlpha(0.002f), _lastLayerQGammaLambda(0.95f),
			_gamma(0.99f),
			_explorationPerturbationStdDev(0.04f), _explorationBreakChance(0.01f),
			_actionMomentum(0.1f),
			_prevValue(0.0f)
		{}

		void createRandom(sys::ComputeSystem &cs, sys::ComputeProgram &program,
			cl_int2 inputSize, cl_int2 actionSize, cl_int inputPredictorRadius, cl_int actionPredictorRadius,
			cl_int actionFeedForwardRadius, const std::vector<LayerDesc> &layerDescs,
			cl_float2 initWeightRange, cl_float initThreshold,
			std::mt19937 &rng);

		void simStep(float reward, sys::ComputeSystem &cs, std::mt19937 &rng, bool learn = true);

		void setState(int index, float value) {
			_inputs[index] = value;
		}

		void setState(int x, int y, float value) {
			setState(x + y * _layers.front()._sc.getVisibleLayerDesc(0)._size.x, value);
		}

		float getAction(int index) const {
			return _actionsExploratory[index];
		}

		float getAction(int x, int y) const {
			return getAction(x + y * _layers.front()._sc.getVisibleLayerDesc(1)._size.x);
		}

		float getPrediction(int index) const {
			return _inputPredictions[index];
		}

		float getPrediction(int x, int y) const {
			return getPrediction(x + y * _layers.front()._sc.getVisibleLayerDesc(0)._size.x);
		}

		size_t getNumLayers() const {
			return _layers.size();
		}

		const Layer &getLayer(int index) const {
			return _layers[index];
		}

		const LayerDesc &getLayerDescs(int index) const {
			return _layerDescs[index];
		}
	};
}