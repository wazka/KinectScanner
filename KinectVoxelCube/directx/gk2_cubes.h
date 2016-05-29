#ifndef __GK2_CUBES_H_
#define __GK2_CUBES_H_

#include "gk2_applicationBase.h"
#include "gk2_camera.h"
#include "gk2_constantBuffer.h"
#include "gk2_solidEffect.h"
#include "gk2_mesh.h"
#include "../Reconstruction.h"

namespace gk2
{
	class Cubes : public ApplicationBase
	{
	public:
		explicit Cubes(HINSTANCE hInstance, VoxelData &scanData);
		virtual ~Cubes();

		static void* operator new(size_t size);
		static void operator delete(void* ptr);

	protected:
		bool LoadContent() override;
		void UnloadContent() override;
		void Update(float dt) override;
		void Render() override;

	private:
		DirectX::XMMATRIX m_projMtx;

		Camera m_camera;
        Mesh m_mesh;

		VoxelData scanData;

		std::shared_ptr<CBMatrix> m_worldCB;
		std::shared_ptr<CBMatrix> m_viewCB;
		std::shared_ptr<CBMatrix> m_projCB;
        std::shared_ptr<SolidEffect> m_solidEffect;
		std::shared_ptr<ID3D11InputLayout> m_layout;
        std::shared_ptr<ID3D11RasterizerState> m_rsCullNone;
        std::shared_ptr<ConstantBuffer<DirectX::XMFLOAT4>> m_surfaceColorCB;

	    void InitializeConstantBuffers();
		void InitializeTextures();
		void InitializeCamera();
		void InitializeRenderStates();
		void CreateScene();
		void UpdateCamera() const;

		void DrawScene();
	};
}

#endif __GK2_CUBES_H_