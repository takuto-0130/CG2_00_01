#include "ParticleClass.h"
#include "TextureManager.h"
#include "operatorOverload.h"
#include <numbers>
#include <imgui.h>

#include "Object3dBasis.h"

void ParticleClass::CollisionEmit(Vector3 pos, float t)
{
	std::mt19937 random(seedGene());
	emitter_.transform.translate = pos;
	emitter_.isEmit = true;
	particles.splice(particles.end(), Emit(emitter_, random, t));
	emitter_.frequencyTime -= emitter_.frequency;
}

ParticleClass::Particle ParticleClass::MakeNewParticle(std::mt19937& random, const Vector3& translate, float t) {
	Particle parti;

	std::uniform_real_distribution<float> distVec(-2.0f, 2.0f);
	std::uniform_real_distribution<float> distScale(0.2f, 0.6f);
	float scale = distScale(random) * t;
	parti.transform.scale = { scale,scale,scale };
	parti.transform.rotate = { 0.f,0.f,0.f };
	parti.transform.translate = translate;
	parti.velocity = { distVec(random) * t,distVec(random) * t,distVec(random) * t };

	std::uniform_real_distribution<float> distColor(0.0f, 1.0f);
	parti.color = { distColor(random),distColor(random),distColor(random),1.0f };

	parti.lifeTime = 0.7f;
	parti.currentTime = 0;

	return parti;
}

std::list<ParticleClass::Particle> ParticleClass::Emit(const Emitter& emitter, std::mt19937& random, float t) {
	std::list<Particle> particles;
	for (uint32_t count = 0; count < emitter.count; ++count) {
		particles.push_back(MakeNewParticle(random, emitter.transform.translate, t));
	}
	return particles;
}

bool ParticleClass::IsCollision(const AABB& a, const Vector3& point) {
	Vector3 closestPoint = { std::clamp(point.x,a.min.x,a.max.x), std::clamp(point.y,a.min.y,a.max.y), std::clamp(point.z,a.min.z,a.max.z) };
	float distance = Length(closestPoint - point);
	if (distance <= 0) {
		return true;
	}
	return false;
}


void ParticleClass::Initialize()
{

	emitter_.transform.scale = { 1,1,1 };
	emitter_.frequency = 0.5f;
	emitter_.frequencyTime = 0.0f;
	emitter_.count = 1;

	isEmit_ = false;

	ModelManager::GetInstance()->LoadModel("Resources", "enemyParti.obj");
	obj_ = std::make_unique<Object3d>();
	obj_->Initialize();
	obj_->SetModel("enemyParti.obj");
	for (int i = 0; i < 300; ++i) {
		trans_[i].Initialize();
	}
}

void ParticleClass::Update()
{
	std::mt19937 random(seedGene());

	for (std::list<Particle>::iterator partiIterator = particles.begin(); partiIterator != particles.end();) {
		if ((*partiIterator).lifeTime <= (*partiIterator).currentTime) {
			partiIterator = particles.erase(partiIterator);
			continue;
		}



		(*partiIterator).transform.rotate = transform.rotate;
		(*partiIterator).transform.translate += (*partiIterator).velocity * kDeltaTime;
		(*partiIterator).currentTime += kDeltaTime; // 経過時間を足す
		++partiIterator;
	}
}

void ParticleClass::Draw()
{
	numInstance = 0;
	for (std::list<Particle>::iterator partiIterator = particles.begin(); partiIterator != particles.end();) {
		if (numInstance < kNumMaxInstance)
		{
			Matrix4x4 worldMatrixP = MakeAffineMatrix((*partiIterator).transform.scale, (*partiIterator).transform.rotate, (*partiIterator).transform.translate);
			obj_->SetColor({ 1,1,1,0.4f });
			trans_[numInstance].scale_ = (*partiIterator).transform.scale;
			trans_[numInstance].translation_ = (*partiIterator).transform.translate;
			trans_[numInstance].rotation_ = (*partiIterator).transform.rotate;
			trans_[numInstance].TransferMatrix();
			obj_->Draw(trans_[numInstance]);
			++numInstance; // 生きてるパーティクルをカウント
		}
		++partiIterator;
	}
}
