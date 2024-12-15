#include "ParticleClass.h"

ParticleClass::Particle ParticleClass::MakeNewParticle(std::mt19937& random, const Vector3& translate) {
	Particle parti;

	std::uniform_real_distribution<float> distVec(-1.0f, 1.0f);
	parti.transform.scale = { 1.f,1.f,1.f };
	parti.transform.rotate = { 0.f,0.f,0.f };
	parti.transform.translate = { distVec(random),distVec(random),distVec(random) };
	parti.transform.translate += translate;
	parti.velocity = { distVec(random),distVec(random),distVec(random) };

	std::uniform_real_distribution<float> distColor(0.0f, 1.0f);
	parti.color = { distColor(random),distColor(random),distColor(random),1.0f };

	std::uniform_real_distribution<float> distTime(1.0f, 3.0f);
	parti.lifeTime = distTime(random);
	parti.currentTime = 0;

	return parti;
}

std::list<ParticleClass::Particle> ParticleClass::Emit(const Emitter& emitter, std::mt19937& random) {
	std::list<Particle> particles;
	for (uint32_t count = 0; count < emitter.count; ++count) {
		particles.push_back(MakeNewParticle(random, emitter.transform.translate));
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


void ParticleClass::Initialize(DirectXBasis* dxBasis, SrvManager* srvManager)
{
}

void ParticleClass::Update()
{
	uint32_t numInstance = 0;

	emitter.frequencyTime += kDeltaTime;
	if (emitter.frequency <= emitter.frequencyTime) {
		particles.splice(particles.end(), Emit(emitter, random));
		emitter.frequencyTime -= emitter.frequency;
	}

	for (std::list<Particle>::iterator partiIterator = particles.begin(); partiIterator != particles.end();) {
		if ((*partiIterator).lifeTime <= (*partiIterator).currentTime) {
			partiIterator = particles.erase(partiIterator);
			continue;
		}

		if (isAccel) {
			if (IsCollision(accel.area, (*partiIterator).transform.translate)) {
				(*partiIterator).velocity = accel.acceleration;
			}
		}


		(*partiIterator).transform.rotate = transform.rotate;
		(*partiIterator).transform.translate += (*partiIterator).velocity * kDeltaTime;
		(*partiIterator).currentTime += kDeltaTime; // 経過時間を足す
		if (numInstance < kNumMaxInstance)
		{
			Matrix4x4 worldMatrixP = MakeAffineMatrix((*partiIterator).transform.scale, (*partiIterator).transform.rotate, (*partiIterator).transform.translate);
			if (useBillboard) {
				worldMatrixP = worldMatrixP * billboardMatrix;
			}
			Matrix4x4 WVPMatrix = worldMatrixP * viewMatrix * projectionMatrix;
			instancingData[numInstance].WVP = WVPMatrix;
			instancingData[numInstance].World = worldMatrixP;
			instancingData[numInstance].color = (*partiIterator).color;
			float alpha = 1.0f - ((*partiIterator).currentTime / (*partiIterator).lifeTime);
			instancingData[numInstance].color.w = alpha;
			++numInstance; // 生きてるパーティクルをカウント
		}
		++partiIterator;
	}

}

void ParticleClass::Draw()
{
}
